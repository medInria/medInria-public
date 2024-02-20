/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medPACS.h"

#include "PACSOperations/dcmPACS.h"
#include <medGroupParameter.h>
#include <medIntParameter.h>
#include <medStringListParameter.h>
#include <medStringParameter.h>
#include <medTriggerParameter.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include <QDate>
#include <QObject>

std::atomic<int> medPACS::s_RequestId = 0;
medStringParameter medPACS::s_Aetitle("AE Title");
medStringParameter medPACS::s_Hostname("Hostname");
medIntParameter medPACS::s_Port("TCP Port");

struct medPACSParametersPrivate
{
    QString instanceId;
    QString instanceName;
    QStringList levelNames;
    bool isOnline;
    QMap<QString, int> levelKeyToRequestIdMap;
    QMap<int, QVariant> requestIdToResultsMap;

    QMap<DcmTagKey, QString> patientLevelAttributes;
    QMap<DcmTagKey, QString> studyLevelAttributes;
    QMap<DcmTagKey, QString> seriesLevelAttributes;

    /* ***********************************************************************/
    /* ******************** External Injected Dependencies *******************/
    /* ***********************************************************************/
    dcmPACS *pacs;
    RequestManager *rqManager;
    dcmWeb *dicomWeb;
    dcmRetrieveData *retrieveData;

    /* ***********************************************************************/
    /* ***************************** PACS Settings ***************************/
    /* ***********************************************************************/

    /* ***************************** Local ***********************************/
    /* ***********************************************************************/
    medGroupParameter *localSettings;

    /* ***************************** Server **********************************/
    /* ***********************************************************************/
    medGroupParameter *remoteSettings;
    medStringParameter *remoteAet;
    medStringParameter *remoteHostname;
    medIntParameter *remotePort;


    /* ***********************************************************************/
    /* ***************************** PACS Communication protocol *************/
    /* ***********************************************************************/
    medGroupParameter *protocolSettings;
    medStringListParameter *protocols;
    bool isDcmtkProtocol;

    /* ********************** DICOMweb API Settings **************************/
    /* ***********************************************************************/
    medGroupParameter *webAPISettings;
    medStringParameter *webAPIUrl;


    /* ***********************************************************************/
    /* ************************* Filtering Parameters ************************/
    /* ***********************************************************************/
    medGroupParameter *filterFileSettings;
    medStringParameter *selectJsonFile;
    QString jsonFilePath;
    
    medGroupParameter *filterPacsSettings;
    medGroupParameter *patientFilter;
    medGroupParameter *studyFilter;
    medGroupParameter *seriesFilter;

    medStringParameter *startDate;
    medStringParameter *endDate;

    medTriggerParameter *resetFilterParameters;

    /* ***********************************************************************/
    /* ************************* Apply Settings ******************************/
    /* ***********************************************************************/
    medTriggerParameter *saveSettingsButton;
};

medPACS::medPACS(dcmPACS *dicomLib) : d(new medPACSParametersPrivate)
{
    qRegisterMetaType<medAbstractSource::eRequestStatus>("medAbstractSource::eRequestStatus");

//    timeout = 60000;

    d->levelNames << "PATIENTS"
                  << "STUDY"
                  << "SERIES"
                  << "ANNOTATION";
    d->isOnline = false;

    d->pacs = dicomLib;
    d->rqManager = new RequestManager(this);
    d->dicomWeb = new dcmWeb(this, d->rqManager);

    QObject::connect(d->rqManager, &RequestManager::responseHttpGet, d->dicomWeb, &dcmWeb::syncRequestSlot, Qt::ConnectionType::QueuedConnection);

    d->retrieveData = new dcmRetrieveData(this, d->rqManager, d->dicomWeb);

    QObject::connect(d->rqManager, &RequestManager::responseHttpGet, d->retrieveData, &dcmRetrieveData::asyncGetSlot, Qt::ConnectionType::QueuedConnection);


    // Communication settings
    d->localSettings = new medGroupParameter("Local dicom settings", this);
    d->localSettings->setDescription("Settings related to local PACS connexion");
    d->localSettings->setCaption("Settings related to local PACS connexion");
    d->localSettings->addParameter(&s_Aetitle);
    d->localSettings->addParameter(&s_Hostname);
    d->localSettings->addParameter(&s_Port);
    s_Aetitle.setParent(nullptr);
    s_Hostname.setParent(nullptr);
    s_Port.setParent(nullptr);
    s_Port.setRange(0, 1000000);

    d->remoteAet = new medStringParameter("Server AE Title");
    d->remoteHostname = new medStringParameter("Server Hostname");
    d->remotePort = new medIntParameter("Server Port");
    d->remotePort->setRange(0, 1000000);
    d->remoteSettings = new medGroupParameter("Dicom server Settings", this);
    d->remoteSettings->setCaption("Settings related to remote PACS connexion");
    d->remoteSettings->setDescription("Settings related to remote PACS connexion");
    d->remoteSettings->addParameter(d->remoteAet);
    d->remoteSettings->addParameter(d->remoteHostname);
    d->remoteSettings->addParameter(d->remotePort);

    d->webAPISettings = new medGroupParameter("DICOMweb Settings", this);
    d->webAPISettings->setCaption("Settings related to remote DICOMweb API connexion");
    d->webAPISettings->setDescription("Settings related to remote DICOMweb API connexion");
    d->webAPIUrl = new medStringParameter("DICOMweb API URL");
    d->webAPISettings->addParameter(d->webAPIUrl);

    d->protocolSettings = new medGroupParameter("Dicom protocol Settings", this);
    d->protocolSettings->setCaption("Settings related to PACS communication protocol");
    d->protocolSettings->setDescription("Settings related to PACS communication protocol");
    d->protocols = new medStringListParameter("Protocols", this);
    d->protocols->addItems({"Select one", "DCMTK", "DICOMweb"});
    d->isDcmtkProtocol = true;
    d->protocols->setCaption("PACS Protocol Communication");
    d->protocolSettings->addParameter(d->protocols);

    d->filterFileSettings = new medGroupParameter("Filter File Settings", this);
    d->filterFileSettings->setCaption("Settings related to a filter parameter file");
    d->filterFileSettings->setDescription("Settings related to a filter parameter file");
    d->selectJsonFile = new medStringParameter("Filtering parameters", this);
    d->selectJsonFile->setCaption("Select filter file");
    d->selectJsonFile->setDefaultRepresentation(5);
    d->filterFileSettings->addParameter(d->selectJsonFile);
    d->jsonFilePath = "";

    d->saveSettingsButton = new medTriggerParameter("Apply parameters", this);
    d->saveSettingsButton->setCaption("Apply");
    d->saveSettingsButton->setDescription("Update PACS Parameters");
    QObject::connect(d->saveSettingsButton, &medTriggerParameter::pushed, this, &medPACS::onSettingsSaved);


    // Filtering parameters
    d->patientLevelAttributes[DCM_PatientID] = "";
    d->patientLevelAttributes[DCM_PatientName] = "";
    d->patientLevelAttributes[DCM_PatientSex] = "";
    medStringParameter *patientID = new medStringParameter("PatientID", this);
    patientID->setCaption("PatientID");
    QObject::connect(patientID, &medStringParameter::valueEdited, [&](QString const &value)
    {
        if (value.isEmpty()) 
        {
            d->patientLevelAttributes[DCM_PatientID] = "";
        }
        else 
        {
            d->patientLevelAttributes[DCM_PatientID] = "*" + value + "*";
        }
    });
    medStringParameter *patientName = new medStringParameter("PatientName", this);
    patientName->setCaption("PatientName");
    QObject::connect(patientName, &medStringParameter::valueEdited, [&](QString const &value)
    {
        if (value.isEmpty()) 
        {
            d->patientLevelAttributes[DCM_PatientName] = "";
        }
        else 
        {
            d->patientLevelAttributes[DCM_PatientName] = "*" + value + "*";
        }
    });
    medStringListParameter *gender = new medStringListParameter("Gender", this);
    gender->addItems({" ", "M", "F", "O"});
    gender->setCaption("Gender");
    QObject::connect(gender, &medStringListParameter::valueChanged, [&, gender](int const &value)
    {
        if (value == 0) 
        {
            d->patientLevelAttributes[DCM_PatientSex] = "";
        }
        else 
        {
            d->patientLevelAttributes[DCM_PatientSex] = gender->value();
        }
    });

    d->studyLevelAttributes[DCM_StudyDescription] = "";
    d->studyLevelAttributes[DCM_StudyDate] = "";
    medStringParameter *studyDescription = new medStringParameter("StudyDescription", this);
    studyDescription->setCaption("StudyDescription");
    QObject::connect(studyDescription, &medStringParameter::valueEdited, [&](QString const &value)
    {
        if (value == 0) 
        {
            d->studyLevelAttributes[DCM_StudyDescription] = "";
        }
        else 
        {
            d->studyLevelAttributes[DCM_StudyDescription] = "*" + value + "*";
        }
    });

    d->startDate = new medStringParameter("Start StudyDate", this);
    d->startDate->setCaption("Start StudyDate");
    d->startDate->setDefaultRepresentation(1);

    d->endDate = new medStringParameter("End StudyDate", this);
    d->endDate->setCaption("End StudyDate");
    d->endDate->setDefaultRepresentation(1);
    QDate currentDate = QDate::currentDate();
    d->startDate->setValue(currentDate.addYears(-100).toString("yyyyMMdd"));
    d->endDate->setValue(currentDate.toString("yyyyMMdd"));
    QObject::connect(d->startDate, &medStringParameter::valueChanged, this, &medPACS::computeDateRange);
    QObject::connect(d->endDate, &medStringParameter::valueChanged, this, &medPACS::computeDateRange);

    d->seriesLevelAttributes[DCM_SeriesDescription] = "";
    d->seriesLevelAttributes[DCM_Modality] = "";

    medStringParameter *seriesDescription = new medStringParameter("SeriesDescription", this);
    seriesDescription->setCaption("SeriesDescription");

    QObject::connect(seriesDescription, &medStringParameter::valueEdited, [&](QString const &value)
    {
        if (value.isEmpty()) 
        {
            d->seriesLevelAttributes[DCM_SeriesDescription] = "";
        }
        else 
        {
            d->seriesLevelAttributes[DCM_SeriesDescription] = "*" + value + "*";
        }
    });

    medStringListParameter *modality = new medStringListParameter("Modality", this);
    modality->addItems({" ", "MR", "CT", "PET"});
    modality->setCaption("Modality");
    QObject::connect(modality, &medStringListParameter::valueChanged, [&, modality](int const &value)
    {
        if (value == 0) 
        {
            d->seriesLevelAttributes[DCM_Modality] = "";
        }
        else 
        {
            d->seriesLevelAttributes[DCM_Modality] = modality->value();
        }
    });

    d->resetFilterParameters = new medTriggerParameter("Reset parameters", this);
    d->resetFilterParameters->setCaption("Reset");
    d->resetFilterParameters->setDescription("Default Filter Parameters");

    d->patientFilter = new medGroupParameter("Patient Level Filters", this);
    d->patientFilter->setCaption("Patient Level");
    d->patientFilter->setDescription("Settings related to Patient Level");
    d->patientFilter->addParameter(patientName);
    d->patientFilter->addParameter(patientID);
    d->patientFilter->addParameter(gender);

    d->studyFilter = new medGroupParameter("Study Level Filters", this);
    d->studyFilter->setCaption("Study Level");
    d->studyFilter->setDescription("Settings related to Study Level");
    d->studyFilter->addParameter(studyDescription);
    d->studyFilter->addParameter(d->startDate);
    d->studyFilter->addParameter(d->endDate);

    d->seriesFilter = new medGroupParameter("Series Level Filters", this);
    d->seriesFilter->setCaption("Series Level");
    d->seriesFilter->setDescription("Settings related to Series Level");
    d->seriesFilter->addParameter(seriesDescription);
    d->seriesFilter->addParameter(modality);

    QObject::connect(d->resetFilterParameters, &medTriggerParameter::pushed, [&]{
        resetFilterParameterValues(d->patientFilter->value(), d->patientLevelAttributes);
        resetFilterParameterValues(d->studyFilter->value(), d->studyLevelAttributes);
        resetFilterParameterValues(d->seriesFilter->value(), d->seriesLevelAttributes);
    });

    d->filterPacsSettings = new medGroupParameter("filter PACS Settings", this);
    d->filterPacsSettings->setCaption("PACS C-Find Parameters");
    d->filterPacsSettings->setDescription("Settings related to filter C-Find on PACS");
    d->filterPacsSettings->addParameter(d->patientFilter);
    d->filterPacsSettings->addParameter(d->studyFilter);
    d->filterPacsSettings->addParameter(d->seriesFilter);
    d->filterPacsSettings->addParameter(d->resetFilterParameters);
}

medPACS::~medPACS()
{
    delete d->pacs;
    delete d->dicomWeb;
}

bool medPACS::initialization(const QString &pi_instanceId)
{
    bool bRes = !pi_instanceId.isEmpty();
    if (bRes)
    {
        d->instanceId = pi_instanceId;
    }
    return bRes;
}

void medPACS::onSettingsSaved()
{
    // WARn : Is it usefull ?
    d->pacs->updateLocalParameters(s_Aetitle.value(), s_Hostname.value(), s_Port.value());
    d->pacs->updateRemoteParameters(d->remoteAet->value(), d->remoteHostname->value(), d->remotePort->value());
    d->dicomWeb->updateUrl(d->webAPIUrl->value());
    d->retrieveData->updateUrl(d->webAPIUrl->value());

    // Additional filtering parameters
    if(!d->selectJsonFile->value().isEmpty())
    {
        readJsonFile(d->selectJsonFile->value());
    }

    // PACS communication protocol
    if(d->protocols->getIndex() == 1)
    {
        d->isDcmtkProtocol = true;
    }
    else if(d->protocols->getIndex() == 2)
    {
        d->isDcmtkProtocol = false;
    }
}

void medPACS::computeDateRange()
{
    QDate startDate = QDate::fromString(d->startDate->value(), "yyyyMMdd");
    QDate endDate = QDate::fromString(d->endDate->value(), "yyyyMMdd");
    if (startDate > endDate)
    {
        d->studyLevelAttributes[DCM_StudyDate] = "";
    }
    else if (startDate == endDate)
    {
        d->studyLevelAttributes[DCM_StudyDate] = d->startDate->value();
    }
    else
    {
        QString dateRange = d->startDate->value() + "-" + d->endDate->value();
        d->studyLevelAttributes[DCM_StudyDate] = dateRange;
    }
}

bool medPACS::setInstanceName(const QString &pi_instanceName)
{
    bool bRes = !pi_instanceName.isEmpty();
    if (bRes)
    {
        d->instanceName = pi_instanceName;
    }
    return bRes;
}

const DcmDictEntry * medPACS::retrieveDcmTag(const QString &key)
{
    const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
    const DcmDictEntry *dicent;

    QString tagName(key.toUtf8().data());
    if(!tagName.contains(","))
    { // search in tag dictionary with Tag Name
        dicent = globalDataDict.findEntry(key.toUtf8().data());
    }
    else
    { // search in tag dictionary with Tag Key
        QStringList tags = tagName.split(",");
        bool ok;
        uint16_t group = tags[0].toUInt(&ok, 16);
        uint16_t element = tags[1].toUInt(&ok, 16);
        DcmTagKey tagKey(group, element);
        dicent = globalDataDict.findEntry(tagKey, NULL);
    }
    dcmDataDict.rdunlock();

    return dicent;
}

// with the idea of being also used with tagKey 'PatientBirthTime'
void medPACS::computeBirthDateRange(const DcmTagKey &tagKey, const QString &start, const QString &end)
{
    QDate startDate = QDate::fromString(start, "yyyyMMdd");
    QDate endDate = QDate::fromString(end, "yyyyMMdd");
    if (startDate > endDate)
    {
        d->patientLevelAttributes[tagKey] = "";
    }
    else if (startDate == endDate)
    {
        d->patientLevelAttributes[tagKey] = start;
    }
    else
    {
        QString dateRange = start + "-" + end;
        d->patientLevelAttributes[tagKey] = dateRange;
    }
}

void medPACS::addPatientFilterParameters(const QJsonObject &patientObject)
{
    medStringParameter *filterParameter;

    for(auto key : patientObject.keys())
    {
        const DcmDictEntry *dicent = retrieveDcmTag(key);
        if(dicent != NULL)
        {
            DcmTagKey dcmTagKey = dicent->getKey();
            DcmTag dcmTag(dcmTagKey);

            if(!d->patientLevelAttributes.contains(dcmTagKey))
            {
                d->patientLevelAttributes[dcmTagKey] = "";

                QString tagName(dcmTag.getTagName());
                if(tagName.compare("PatientBirthDate") == 0)
                {
                    medStringParameter *startDateFilterParameter = new medStringParameter("Start " + tagName, this);
                    startDateFilterParameter->setCaption("Start " + tagName);
                    QDate currentDate = QDate::currentDate();
                    startDateFilterParameter->setValue(currentDate.addYears(-100).toString("yyyyMMdd"));
                    startDateFilterParameter->setDefaultRepresentation(1);

                    medStringParameter *endDateFilterParameter = new medStringParameter("End " + tagName, this);
                    endDateFilterParameter->setCaption("End " + tagName);
                    endDateFilterParameter->setValue(currentDate.toString("yyyyMMdd"));
                    endDateFilterParameter->setDefaultRepresentation(1);

                    d->patientFilter->addParameter(startDateFilterParameter);
                    d->patientFilter->addParameter(endDateFilterParameter);

                    QObject::connect(startDateFilterParameter, &medStringParameter::valueChanged, [&, dcmTagKey, startDateFilterParameter, endDateFilterParameter]()
                    {
                        computeBirthDateRange(dcmTagKey, startDateFilterParameter->value(), endDateFilterParameter->value());
                    });
                    QObject::connect(endDateFilterParameter, &medStringParameter::valueChanged, [&, dcmTagKey, startDateFilterParameter, endDateFilterParameter]()
                    {
                        computeBirthDateRange(dcmTagKey, startDateFilterParameter->value(), endDateFilterParameter->value());
                    });
                }
                else
                {
                    filterParameter = new medStringParameter(dcmTag.getTagName(), this);
                    filterParameter->setCaption(dcmTag.getTagName());
                    QObject::connect(filterParameter, &medStringParameter::valueEdited, [&, filterParameter](QString const &value)
                    {
                        if(value.isEmpty())
                        {
                            d->patientLevelAttributes[dcmTagKey] = "";
                        }else{
                            d->patientLevelAttributes[dcmTagKey] = value;
                        } 
                    });
                    d->patientFilter->addParameter(filterParameter);
                }
            }
            else
            {
                qDebug() << "Dicom tag" << dcmTag.getTagName() << "already exists as a filter key";
            }
        }
        else
        {
            qDebug() << "Wrong Dicom tag key:" << key;
        }
    }
}

void medPACS::addStudyFilterParameters(const QJsonObject &studyObject)
{
    medStringParameter *filterParameter;

    for(auto key : studyObject.keys())
    {
        const DcmDictEntry *dicent = retrieveDcmTag(key);
        if(dicent != NULL)
        {
            DcmTagKey dcmTagKey = dicent->getKey();
            DcmTag dcmTag(dcmTagKey);

            if(!d->studyLevelAttributes.contains(dcmTagKey))
            {
                d->studyLevelAttributes[dcmTagKey] = "";

                filterParameter = new medStringParameter(dcmTag.getTagName(), this);
                filterParameter->setCaption(dcmTag.getTagName());
                QObject::connect(filterParameter, &medStringParameter::valueEdited, [&, dcmTagKey](QString const &value)
                {
                    if(value.isEmpty())
                    {
                        d->studyLevelAttributes[dcmTagKey] = "";
                    }
                    else
                    {
                        d->studyLevelAttributes[dcmTagKey] = value;
                    } 
                });
                d->studyFilter->addParameter(filterParameter);

            }
            else
            {
                qDebug() << "Dicom tag" << dcmTag.getTagName() << "already exists as a filter key";            
            }
        }
        else
        {
            qDebug() << "Wrong Dicom tag key:" << key;
        }
    }
}

void medPACS::addSeriesFilterParameters(const QJsonObject &seriesObject)
{
    medStringParameter *filterParameter;

    for(auto key : seriesObject.keys())
    {
        const DcmDictEntry *dicent = retrieveDcmTag(key);
        if(dicent != NULL)
        {
            DcmTagKey dcmTagKey = dicent->getKey();
            DcmTag dcmTag(dcmTagKey);

            if(!d->seriesLevelAttributes.contains(dcmTagKey))
            {
                d->seriesLevelAttributes[dcmTagKey] = "";
                
                filterParameter = new medStringParameter(dcmTag.getTagName(), this);
                filterParameter->setCaption(dcmTag.getTagName());
                QObject::connect(filterParameter, &medStringParameter::valueEdited, [&, dcmTagKey](QString const &value)
                {
                    if(value.isEmpty())
                    {
                        d->seriesLevelAttributes[dcmTagKey] = "";
                    }else{
                        d->seriesLevelAttributes[dcmTagKey] = value;
                    } 
                });
                d->seriesFilter->addParameter(filterParameter);

            }
            else
            {
                qDebug() << "Dicom tag" << dcmTag.getTagName() << "already exists as a filter key";
            }
        }
        else
        {
            qDebug() << "Wrong Dicom tag key:" << key;
        }
    }
}

void medPACS::readJsonFile(const QString &filePath)
{
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Cannot read file" << file.fileName();
        return;
    }

    if(filePath.compare(d->jsonFilePath) != 0)
    {
        d->jsonFilePath = filePath;

        QString content(file.readAll());
        file.close();

        if(!content.isEmpty())
        {
            // Add filter parameters from json file
            QJsonDocument jsonDoc = QJsonDocument::fromJson(content.toUtf8());
            QJsonObject jsonFilterObj = jsonDoc.object();

            // instead of switch/case because of case sensitive
            for(auto levelKey : jsonFilterObj.keys())
            {
                if(levelKey.compare("Patient", Qt::CaseInsensitive) == 0)
                {
                    addPatientFilterParameters(jsonFilterObj.value(levelKey).toObject());
                }
                else if(levelKey.compare("Study", Qt::CaseInsensitive) == 0)
                {
                    addStudyFilterParameters(jsonFilterObj.value(levelKey).toObject());
                }
                else if(levelKey.compare("Series", Qt::CaseInsensitive) == 0)
                {
                    addSeriesFilterParameters(jsonFilterObj.value(levelKey).toObject());
                }
                else
                {
                    qDebug() << "Invalid Level Name:" << levelKey;
                }
            }
        }
    }
    else
    {
        qDebug() << "File filter keys already loaded : " << filePath; 
    }
}

void medPACS::resetFilterParameterValues(QList<medAbstractParameter*> levelFilter, QMap<DcmTagKey, QString> &levelAttributes)
{
    QDate currentDate = QDate::currentDate();
    for(auto param : levelFilter)
    {
        if(param->id().compare("Modality") == 0 || param->id().compare("Gender") == 0)
        {
            medStringListParameter *listParam = reinterpret_cast<medStringListParameter *>(param);
            listParam->setIndex(0);
        }
        else
        {
            medStringParameter *strParam = reinterpret_cast<medStringParameter *>(param);

            if(strParam->id().contains("Start "))
            {
                strParam->setValue(currentDate.addYears(-100).toString("yyyyMMdd"));
            }
            else if(strParam->id().contains("End "))
            {
                strParam->setValue(currentDate.toString("yyyyMMdd"));
            }
            else
            {
                // set the value to " " before, otherwise the connect that resets the interface won't run
                strParam->setValue(" ");
                // remove space to avoid altering filters
                strParam->setValue("");
            }
        }
    }

    for(auto key : levelAttributes.keys())
    {
        levelAttributes[key] = "";
    }
}

bool medPACS::connect(bool pi_bEnable)
{
    bool bRes = false;
    if (pi_bEnable)
    {
        // update parameters
        d->pacs->updateLocalParameters(s_Aetitle.value(), s_Hostname.value(), s_Port.value());
        d->pacs->updateRemoteParameters(d->remoteAet->value(), d->remoteHostname->value(), d->remotePort->value());
        d->dicomWeb->updateUrl(d->webAPIUrl->value());
        d->retrieveData->updateUrl(d->webAPIUrl->value());
        
        if(!d->selectJsonFile->value().isEmpty())
        {
            readJsonFile(d->selectJsonFile->value());
        }

        // PACS communication protocol
        if(d->protocols->getIndex() == 1)
        {
            d->isDcmtkProtocol = true;
        }
        else if(d->protocols->getIndex() == 2)
        {
            d->isDcmtkProtocol = false;
        }

       
        int respCode;
        if(d->isDcmtkProtocol)
        { // connection to remote PACS
            respCode = d->pacs->cEcho();
            bRes = !respCode;
        }
        else
        { // connection to remote DICOMweb API
            if(d->dicomWeb->hasUrl())
            {
                respCode = d->dicomWeb->isServerAvailable();
                bRes = !respCode;
            }
        }

        d->isOnline = bRes;
    }
    else
    {
        d->isOnline = false;
        bRes = true;
    }
    emit connectionStatus(d->isOnline);
    return bRes;
}

QList<medAbstractParameter *> medPACS::getAllParameters()
{
    return {d->localSettings, d->remoteSettings, d->webAPISettings, d->protocolSettings, d->filterFileSettings, d->saveSettingsButton};
}

QList<medAbstractParameter *> medPACS::getCipherParameters()
{
    return {};
}

QList<medAbstractParameter *> medPACS::getVolatilParameters()
{
    return {};
}

QList<medAbstractParameter *> medPACS::getFilteringParameters()
{
    return {d->filterPacsSettings};
}

bool medPACS::isWritable()
{
    return false;
}

bool medPACS::isLocal()
{
    return false;
}

bool medPACS::isCached()
{
    return false;
}

bool medPACS::isOnline()
{
    return d->isOnline;
}

bool medPACS::isFetchByMinimalEntriesOrMandatoryAttributes()
{
    return false;
}

QString medPACS::getInstanceName()
{
    return d->instanceName;
}

QString medPACS::getInstanceId()
{
    return d->instanceId;
}

unsigned int medPACS::getLevelCount()
{
    return d->levelNames.size();
}

unsigned int medPACS::getLevelDesiredWritable()
{
    return 3;
}

QStringList medPACS::getLevelNames()
{
    return d->levelNames;
}

QString medPACS::getLevelName(unsigned int pi_uiLevel)
{
    QString retVal = "Invalid Level Name";
    if (pi_uiLevel >= 0 && pi_uiLevel < static_cast<unsigned int>(d->levelNames.size()))
    {
        retVal = d->levelNames.value((int)pi_uiLevel);
    }
    return retVal;
}

bool medPACS::isLevelWritable(unsigned int pi_uiLevel)
{
    return pi_uiLevel == 3;
}

QStringList medPACS::getMandatoryAttributesKeys(unsigned int pi_uiLevel)
{
    switch (pi_uiLevel)
    {
        case 0:
            return {"id", "description", "patientID", "gender", "birthdate"};
        case 1:
            return {"id", "description", "uid", "study date"};
        case 2:
            return {"id", "description", "uid", "modality", "institution name", "acquisition number", "number of series related instances"};
        // ANNOTATION
        case 3:
            return {"id", "description", "uid"};
        default:
            return QStringList();
    }
}

QList<medAbstractSource::levelMinimalEntries> medPACS::getMinimalEntries(unsigned int pi_uiLevel, QString parentId)
{
    QList<levelMinimalEntries> entries;
    QString key;
    if (parentId.contains("/"))
    {
        QStringList splittedUri = parentId.split("/");
        key = splittedUri[(int)pi_uiLevel - 1];
    }
    else
    {
        key = parentId;
    }

    auto seriesLevelValues = d->seriesLevelAttributes.values();
    seriesLevelValues.removeAll("");
    auto studyLevelValues = d->studyLevelAttributes.values();
    studyLevelValues.removeAll("");

    QList<QMap<QString, QString>> infos;
    switch (pi_uiLevel)
    {
        case 0:
        {
            if (seriesLevelValues.isEmpty() && studyLevelValues.isEmpty())
            {
                auto infosPatient = cFindPatient(key);
                entries = getPatientMinimalEntries(infosPatient);
            }
            else
            {
                if (seriesLevelValues.isEmpty())
                {
                    auto infosStudy = cFindStudy("");
                    QStringList patientIDs;
                    for (const auto &infoStudy : infosStudy)
                    {
                        auto currentPatientID = infoStudy[DCM_PatientID];
                        if (!patientIDs.contains(currentPatientID))
                        {
                            auto infosPatient = cFindPatient(currentPatientID);
                            entries.append(getPatientMinimalEntries(infosPatient));
                        }
                        patientIDs.append(currentPatientID);
                    }
                }
                else
                {
                    auto infosSeries = cFindSeries("");
                    QStringList studyInstanceUIDs;
                    QStringList patientIDs;

                    for (const auto &infoSeries : infosSeries)
                    {
                        auto currentStudyUID = infoSeries[DCM_StudyInstanceUID];
                        if (!studyInstanceUIDs.contains(currentStudyUID))
                        {
                            auto infosStudy = cFindStudy(currentStudyUID);
                            auto currentPatientID = infosStudy.first()[DCM_PatientID];
                            if (!patientIDs.contains(currentPatientID))
                            {
                                auto infosPatient = cFindPatient(currentPatientID);
                                entries.append(getPatientMinimalEntries(infosPatient));
                            }
                            patientIDs.append(currentPatientID);
                        }
                        studyInstanceUIDs.append(currentStudyUID);
                    }
                }
            }
        }
        break;
        case 1:
        {
            auto infosStudy = cFindStudy("", key);
            bool isSeriesFiltered = !seriesLevelValues.isEmpty();
            entries = getStudyMinimalEntries(infosStudy, isSeriesFiltered);
        }
        break;
        case 2:
        {
            auto infosSeries = cFindSeries(key);
            entries = getSeriesMinimalEntries(infosSeries);
        }
        break;
        // ANNOTATION
        case 3:
            break;
        default:
            break;
    }

    return entries;
}

QList<medAbstractSource::levelMinimalEntries> medPACS::getPatientMinimalEntries(const QList<QMap<DcmTagKey, QString>> &infosMap)
{
    QList<levelMinimalEntries> entries;
    for (const auto &info : infosMap)
    {
        levelMinimalEntries entry;
        entry.key = info[DCM_PatientID];
        entry.name = info[DCM_PatientName];
        entry.description = info[DCM_PatientID];
        entries.append(entry);
    }
    return entries;
}

QList<medAbstractSource::levelMinimalEntries> medPACS::getStudyMinimalEntries(const QList<QMap<DcmTagKey, QString>> &infosMap, bool isSeriesFiltered)
{
    QList<levelMinimalEntries> entries;
    if (isSeriesFiltered)
    {
        for (const auto &info : infosMap)
        {
            auto infosSeries = cFindSeries(info[DCM_StudyInstanceUID]);
            if (!infosSeries.empty())
            {
                levelMinimalEntries entry;
                entry.key = info[DCM_StudyInstanceUID];
                entry.name = info[DCM_StudyDescription];
                entry.description = info[DCM_StudyInstanceUID];
                entries.append(entry);
            }
        }
    }
    else
    {
        for (const auto &info : infosMap)
        {
            levelMinimalEntries entry;
            entry.key = info[DCM_StudyInstanceUID];
            entry.name = info[DCM_StudyDescription];
            entry.description = info[DCM_StudyInstanceUID];
            entries.append(entry);
        }
    }
    return entries;
}

QList<medAbstractSource::levelMinimalEntries> medPACS::getSeriesMinimalEntries(const QList<QMap<DcmTagKey, QString>> &infosMap)
{
    QList<levelMinimalEntries> entries;

    for (const auto &info : infosMap)
    {
        levelMinimalEntries entry;
        entry.key = info[DCM_SeriesInstanceUID];
        entry.name = info[DCM_SeriesDescription];
        entry.description = info[DCM_SeriesInstanceUID];
        entries.append(entry);
    }
    return entries;
}

QList<QMap<QString, QString>> medPACS::getMandatoryAttributes(unsigned int pi_uiLevel, QString parentId)
{
    QList<QMap<QString, QString>> attributesRes;
    QString parentKey;

    qDebug() << "getMandatoryAttributes(" << parentId << ")";

    if (parentId.contains("/"))
    {
        QStringList splittedUri = parentId.split("/");
        parentKey = splittedUri[(int)pi_uiLevel - 1];
    }
    else
    {
        parentKey = parentId;
    }

    auto seriesLevelValues = d->seriesLevelAttributes.values();
    seriesLevelValues.removeAll("");
    bool isSeriesFiltered = !seriesLevelValues.isEmpty();

    auto studyLevelValues = d->studyLevelAttributes.values();
    studyLevelValues.removeAll("");
    bool isStudiesFiltered = !studyLevelValues.isEmpty();

    auto patientLevelValues = d->patientLevelAttributes.values();
    patientLevelValues.removeAll("");
    bool isPatientFiltered = !patientLevelValues.isEmpty();

    QMap<DcmTagKey, QString> tags;
    switch (pi_uiLevel)
    {
        case 0:
        {
            if (!isSeriesFiltered && !isStudiesFiltered)
            {
                auto infosPatient = cFindPatient(parentKey);
                attributesRes = getPatientMandatoryAttributes(infosPatient);
            }
            else
            {
                if (!isSeriesFiltered)
                {
                    auto infosStudy = cFindStudy("");
                    QStringList patientIDs;
                    for (const auto &infoStudy : infosStudy)
                    {
                        auto currentPatientID = infoStudy[DCM_PatientID];
                        if (!patientIDs.contains(currentPatientID))
                        {
                            auto infosPatient = cFindPatient(currentPatientID);
                            attributesRes.append(getPatientMandatoryAttributes(infosPatient));
                        }
                        patientIDs.append(currentPatientID);
                    }
                }
                else
                {
                    auto infosSeries = cFindSeries("");
                    QStringList studyInstanceUIDs;
                    QStringList patientIDs;

                    for (const auto &infoSeries : infosSeries)
                    {
                        auto currentStudyUID = infoSeries[DCM_StudyInstanceUID];
                        if (!studyInstanceUIDs.contains(currentStudyUID))
                        {
                            auto infosStudy = cFindStudy(currentStudyUID);
                            auto currentPatientID = infosStudy.first()[DCM_PatientID];
                            if (!patientIDs.contains(currentPatientID))
                            {
                                auto infosPatient = cFindPatient(currentPatientID);
                                attributesRes.append(getPatientMandatoryAttributes(infosPatient));
                            }
                            patientIDs.append(currentPatientID);
                        }
                        studyInstanceUIDs.append(currentStudyUID);
                    }
                }
            }
        }
        break;
        case 1:
        {
            auto infosStudy = cFindStudy("", parentKey);
            attributesRes = getStudyMandatoryAttributes(infosStudy, isPatientFiltered, isSeriesFiltered);
        }
        break;
        case 2:
        {
            auto infosSeries = cFindSeries(parentKey);
            attributesRes = getSeriesMandatoryAttributes(infosSeries, isPatientFiltered, isStudiesFiltered);
        }
        break;
        // ANNOTATION
        case 3:
            break;
        default:
            break;
    }

    return attributesRes;
}

QList<QMap<DcmTagKey, QString>> medPACS::cFindPatient(const QString &patientID)
{
    QList<QMap<DcmTagKey, QString>> patientsRes;
    QMap<DcmTagKey, QString> tags;
    tags[DCM_QueryRetrieveLevel] = "PATIENT";
    for (auto key : d->patientLevelAttributes.keys())
    {
        tags.insert(key, d->patientLevelAttributes.value(key));
    }

    if (!patientID.isEmpty())
    {
        tags[DCM_PatientID] = patientID;
    }

    // optional tag
    if(!tags.contains(DCM_PatientBirthDate))
    {
        tags[DCM_PatientBirthDate] = "";
    }

    if(d->isDcmtkProtocol)
    {
        patientsRes = d->pacs->cFind(tags);
    }
    else
    {
        qDebug() << "Find patient DICOMweb";
        patientsRes = d->dicomWeb->patientSearchService(tags);
    }

    return patientsRes;
}

QList<QMap<DcmTagKey, QString>> medPACS::cFindStudy(const QString &studyInstanceUID, const QString &patientID)
{
    QList<QMap<DcmTagKey, QString>> studyRes;
    QMap<DcmTagKey, QString> tags;

    tags[DCM_QueryRetrieveLevel] = "STUDY";
    for (auto key : d->studyLevelAttributes.keys())
    {
        tags.insert(key, d->studyLevelAttributes.value(key));
    }

    tags[DCM_PatientID] = patientID;

    if(!studyInstanceUID.isEmpty())
    {
        tags[DCM_StudyInstanceUID] = studyInstanceUID;
    }

    if(d->isDcmtkProtocol)
    {
        studyRes = d->pacs->cFind(tags);
    }
    else
    {
        qDebug() << "Find study DICOMweb";
        studyRes = d->dicomWeb->studySearchService(tags);
    }

    return studyRes;
}

QList<QMap<DcmTagKey, QString>> medPACS::cFindSeries(const QString &studyInstanceUID)
{
    QList<QMap<DcmTagKey, QString>> seriesRes;
    QMap<DcmTagKey, QString> tags;

    tags[DCM_QueryRetrieveLevel] = "SERIES";
    for (auto key : d->seriesLevelAttributes.keys())
    {
        tags.insert(key, d->seriesLevelAttributes.value(key));
    }
    
    //qDebug() << studyInstanceUID << "vs" << tags[DCM_StudyInstanceUID];
    if (!studyInstanceUID.isEmpty())
    {
        tags[DCM_StudyInstanceUID] = studyInstanceUID;
    }
    
    // optional tags
    if(!tags.contains(DCM_SeriesInstanceUID))
    {
        tags[DCM_SeriesInstanceUID] = "";
    }
    if(!tags.contains(DCM_InstitutionName))
    {
        tags[DCM_InstitutionName] = "";
    }
    if(!tags.contains(DCM_AcquisitionNumber))
    {
        tags[DCM_AcquisitionNumber] = "";
    }
    if(!tags.contains(DCM_NumberOfSeriesRelatedInstances))
    {
        tags[DCM_NumberOfSeriesRelatedInstances] = "";
    }

    qDebug() << "cFindSeries(" << studyInstanceUID << ")";
    for(auto key : tags.keys()){
        qDebug() << key.toString().c_str() << "=" << tags.value(key);
    }

    if(d->isDcmtkProtocol)
    {
        seriesRes = d->pacs->cFind(tags);
    }
    else
    {
        qDebug() << "Find series DICOMweb";
        seriesRes = d->dicomWeb->seriesSearchService(tags);
    }

    return seriesRes;
}

QList<QMap<QString, QString>> medPACS::getPatientMandatoryAttributes(const QList<QMap<DcmTagKey, QString>> &infosMap)
{
    QList<QMap<QString, QString>> attributesRes;
    for (const auto &info : infosMap)
    {
        QMap<QString, QString> attributes;
        attributes["id"] = info[DCM_PatientID];
        attributes["description"] = info[DCM_PatientName];
        attributes["patientID"] = info[DCM_PatientID];
        attributes["gender"] = info[DCM_PatientSex];
        attributes["birthdate"] = info[DCM_PatientBirthDate];
        attributesRes.append(attributes);
    }
    return attributesRes;
}

QList<QMap<QString, QString>> medPACS::getStudyMandatoryAttributes(QList<QMap<DcmTagKey, QString>> &infosMap, bool isPatientFiltered, bool isSeriesFiltered)
{
    QList<QMap<QString, QString>> attributesRes;

    if (isPatientFiltered)
    {
        QList<QMap<DcmTagKey, QString>> infosStudy = infosMap;
        for (const auto &info : infosStudy)
        {
            auto infosPatient = cFindPatient(info[DCM_PatientID]);
            if (infosPatient.empty())
            {
                infosMap.removeOne(info);
            }
        }
    }

    if (isSeriesFiltered)
    {
        for (const auto &info : infosMap)
        {
            auto infosSeries = cFindSeries(info[DCM_StudyInstanceUID]);
            if (!infosSeries.empty())
            {
                QMap<QString, QString> attributes;
                attributes["id"] = info[DCM_StudyInstanceUID];
                attributes["description"] = info[DCM_StudyDescription];
                attributes["uid"] = info[DCM_StudyInstanceUID];
                attributes["study date"] = info[DCM_StudyDate];
                attributesRes.append(attributes);
            }
        }
    }
    else
    {
        for (const auto &info : infosMap)
        {
            QMap<QString, QString> attributes;
            attributes["id"] = info[DCM_StudyInstanceUID];
            qDebug() << "getStudyMandatoryAttributes ->" << attributes["id"];
            attributes["description"] = info[DCM_StudyDescription];
            attributes["uid"] = info[DCM_StudyInstanceUID];
            attributes["study date"] = info[DCM_StudyDate];
            attributesRes.append(attributes);
        }
    }

    return attributesRes;
}

QList<QMap<QString, QString>> medPACS::getSeriesMandatoryAttributes(QList<QMap<DcmTagKey, QString>> &infosMap, bool isPatientFiltered, bool isStudiesFiltered)
{
    QList<QMap<QString, QString>> attributesRes;

    if (isStudiesFiltered || isPatientFiltered)
    {
        QList<QMap<DcmTagKey, QString>> infosSeries = infosMap;
        for (const auto &info : infosSeries)
        {
            auto infosStudy = cFindStudy(info[DCM_StudyInstanceUID]);
            if (infosStudy.empty())
            {
                infosMap.removeOne(info);
            }
            else
            {
                for (const auto &infoS : infosStudy)
                {
                    auto infosPatient = cFindPatient(infoS[DCM_PatientID]);
                    if (infosPatient.empty())
                    {
                        infosMap.removeOne(info);
                    }
                    else
                    {
                        QMap<QString, QString> attributes;
                        attributes["id"] = info[DCM_SeriesInstanceUID];
                        attributes["description"] = info[DCM_SeriesDescription];
                        attributes["uid"] = info[DCM_SeriesInstanceUID];
                        attributes["modality"] = info[DCM_Modality];
                        attributes["institution name"] = info[DCM_InstitutionName];
                        attributes["acquisition number"] = info[DCM_AcquisitionNumber];
                        attributes["number of series related instances"] = info[DCM_NumberOfSeriesRelatedInstances];
                        attributesRes.append(attributes);
                    }
                }
            }
        }
    }
    else
    {
        for (const auto &info : infosMap)
        {
            QMap<QString, QString> attributes;
            attributes["id"] = info[DCM_SeriesInstanceUID];
            attributes["description"] = info[DCM_SeriesDescription];
            attributes["uid"] = info[DCM_SeriesInstanceUID];
            attributes["modality"] = info[DCM_Modality];
            attributes["institution name"] = info[DCM_InstitutionName];
            attributes["acquisition number"] = info[DCM_AcquisitionNumber];
            attributes["number of series related instances"] = info[DCM_NumberOfSeriesRelatedInstances];
            attributesRes.append(attributes);
        }
    }
    return attributesRes;
}

bool medPACS::getAdditionalAttributes(unsigned int pi_uiLevel, QString id, datasetAttributes &po_attributes)
{
    return false;
}

QVariant medPACS::getDirectData(unsigned int pi_uiLevel, QString key)
{
    QVariant iPath;

    return iPath;
}

int medPACS::getAssyncData(unsigned int pi_uiLevel, QString key)
{
    int iRes = -1;
    s_RequestId++;
    QString levelKey = QString::number(pi_uiLevel) + ":" + key;
    if (d->levelKeyToRequestIdMap.contains(levelKey))
    {
        int requestId = d->levelKeyToRequestIdMap[levelKey];
        if (pi_uiLevel > 0 && pi_uiLevel < 3)
        {
            if(d->isDcmtkProtocol)
            {
                if (d->pacs->isCachedDataPath(requestId))
                {
                    iRes = requestId;
                }
            }
            else
            {
                if(!d->webAPIUrl->value().isEmpty())
                {
                    if(d->retrieveData->isCachedDataPath(requestId))
                    {
                        iRes = requestId;
                    }
                }
            }
        }
    }
    if (iRes == -1)
    {
        d->levelKeyToRequestIdMap[levelKey] = s_RequestId;
        if (pi_uiLevel > 0 && pi_uiLevel < 3)
        {
            if(d->isDcmtkProtocol)
            {
                iRes = getPACSAsyncData(pi_uiLevel, key);
            }else
            {
                if(!d->webAPIUrl->value().isEmpty())
                {
                    iRes = getDicomWebAsyncData(pi_uiLevel, key);
                }
            }
        }
    }
    return iRes;
}

int medPACS::getPACSAsyncData(unsigned int pi_uiLevel, const QString &key)
{
    int iRes = -1;
    QString queryLevel;
    switch (pi_uiLevel)
    {
        case 0:
            queryLevel = "PATIENT";
            break;
        case 1:
            queryLevel = "STUDY";
            break;
        case 2:
            queryLevel = "SERIES";
            break;
        default:
            return iRes;
    }

    if (d->pacs->cMove(s_RequestId, queryLevel, key))
    {
        iRes = s_RequestId;

        QObject::connect(d->pacs, &dcmPACS::moveProgress, this, [=](int pi_requestId, eRequestStatus status, const QString &path)
        {
            switch (status)
            {
                case 0:
                    d->requestIdToResultsMap[pi_requestId] = QVariant(path);
                    break;
                case pending:
                    break;
                case faild:
                    default:
                    break;
            }
            emit progress(pi_requestId, status); 
        },
        Qt::UniqueConnection);
    }

    return iRes;
}

int medPACS::getDicomWebAsyncData(unsigned int pi_uiLevel, const QString &key)
{
    int iRes = -1;
    if (d->retrieveData->retrieveDataService(s_RequestId, pi_uiLevel, key))
    {
        iRes = s_RequestId;

        QObject::connect(d->retrieveData, &dcmRetrieveData::inProgress, this, [=](int pi_requestId, eRequestStatus status, const QString &path)
        {
            switch (status)
            {
                case finish:
                    d->requestIdToResultsMap[pi_requestId] = QVariant(path);
                    break;
                case pending:
                    break;
                case faild:
                default:
                    break;
            }
            emit progress(pi_requestId, status);
        },
        Qt::UniqueConnection);
    }

    return iRes;
}

void medPACS::abort(int pi_iRequest)
{
    auto keyLevel = d->levelKeyToRequestIdMap.key(pi_iRequest);
    int level = -1;
    if (!keyLevel.isEmpty())
    {
        int size = keyLevel.indexOf(":");
        level = keyLevel.left(size).toInt();
    }
    if (level > 0 && level < 3)
    {
        if(d->isDcmtkProtocol)
        {
            d->pacs->stopMove(pi_iRequest);
        }
        else
        {
            d->retrieveData->abortDownload(pi_iRequest);
        }
    }
}

bool medPACS::createPath(QList<levelMinimalEntries> &pio_path, const medAbstractSource::datasetAttributes &pi_attributes,
                         unsigned int pi_uiLevel, QString parentKey)
{
    return false;
}

int medPACS::getIOInterface()
{
    return IO_FILE;
}

QMap<QString, QStringList> medPACS::getTypeAndFormat()
{
    QMap<QString, QStringList> supportedTypeAndFormats;
    supportedTypeAndFormats["Image"] = QStringList({".mha"});

    return supportedTypeAndFormats;
}

bool medPACS::addDirectData(QVariant data, medAbstractSource::levelMinimalEntries &pio_minimalEntries,
                            unsigned int pi_uiLevel, QString parentKey)
{
    return false;
}

int medPACS::addAssyncData(QVariant data, medAbstractSource::levelMinimalEntries &pio_minimalEntries,
                           unsigned int pi_uiLevel, QString parentKey)
{
    int iRes = -1;

    return iRes;
}

bool medPACS::createFolder(medAbstractSource::levelMinimalEntries &pio_minimalEntries,
                           const medAbstractSource::datasetAttributes &pi_attributes, unsigned int pi_uiLevel,
                           QString parentKey)
{
    return false;
}

bool medPACS::alterMetaData(const medAbstractSource::datasetAttributes &pi_attributes, unsigned int pi_uiLevel, QString key)
{
    return false;
}

bool medPACS::getThumbnail(QPixmap &po_thumbnail, unsigned int pi_uiLevel, QString key)
{
    return false;
}

bool medPACS::setThumbnail(QPixmap &pi_thumbnail, unsigned int pi_uiLevel, QString key)
{
    return false;
}

bool medPACS::commitData(QVariant data, medAbstractSource::levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel,
                         QString parentKey)
{
    return false;
}

QVariant medPACS::getAsyncResults(int pi_iRequest)
{
    QVariant dataRes;

    if (d->requestIdToResultsMap.contains(pi_iRequest))
    {
        dataRes = d->requestIdToResultsMap[pi_iRequest];
    }
    
    return dataRes;
}

int medPACS::push(unsigned int pi_uiLevel, QString key)
{
    return 0;
}
