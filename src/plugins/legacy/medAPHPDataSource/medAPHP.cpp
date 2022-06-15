/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medAPHP.h"

#include <PluginAPHP/QtDcmAPHP.h>
#include <medGroupParameter.h>
#include <medIntParameter.h>
#include <medStringListParameter.h>
#include <medStringParameter.h>
#include <medTriggerParameter.h>

#include <QObject>

std::atomic<int> medAPHP::s_RequestId = -1;
medStringParameter medAPHP::s_Aetitle("AE Title");
medStringParameter medAPHP::s_Hostname("Hostname");
medIntParameter medAPHP::s_Port("TCP Port");

struct medAPHPParametersPrivate
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
    QtDcmInterface *qtdcm;
    medAbstractAnnotation *restFulAPI;

    /* ***********************************************************************/
    /* ***************************** PACS Settings ***************************/
    /* ***********************************************************************/

    /* ***************************** Local ***********************************/
    /* ***********************************************************************/
    medGroupParameter *localSettings;

    /* ***********************************************************************/
    /* ***************************** Server **********************************/
    /* ***********************************************************************/
    medGroupParameter *remoteSettings;
    medStringParameter *remoteAet;
    medStringParameter *remoteHostname;
    medIntParameter *remotePort;

    /* ***********************************************************************/
    /* ********************** Annotation REST API Settings *******************/
    /* ***********************************************************************/
    medGroupParameter *restFulSettings;
    medStringParameter *restFulUrl;

    medTriggerParameter *saveSettingsButton;

    /* ***********************************************************************/
    /* ************************* Filtering Parameters ************************/
    /* ***********************************************************************/
    medGroupParameter *filterPacsSettings;
    medStringParameter *startDate;
    medStringParameter *endDate;
};

medAPHP::medAPHP(QtDcmInterface *dicomLib, medAbstractAnnotation *annotationAPI) : d(new medAPHPParametersPrivate)
{
    timeout = 60000;

    d->levelNames << "PATIENTS"
                  << "STUDY"
                  << "SERIES"
                  << "ANNOTATION";
    d->isOnline = false;

    d->qtdcm = dicomLib;
    d->restFulAPI = annotationAPI;

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

    d->restFulUrl = new medStringParameter("Rest API Anotation URL ");

    d->restFulSettings = new medGroupParameter("annotation API Settings", this);
    d->restFulSettings->setCaption("Settings related to remote restFul API connexion");
    d->restFulSettings->setDescription("Settings related to remote restFul API connexion");
    d->restFulSettings->addParameter(d->restFulUrl);

    d->saveSettingsButton = new medTriggerParameter("Apply parameters", this);
    d->saveSettingsButton->setCaption("Apply");
    d->saveSettingsButton->setDescription("Update APHP Parameters");
    QObject::connect(d->saveSettingsButton, &medTriggerParameter::pushed, this, &medAPHP::onSettingsSaved);

    d->patientLevelAttributes[DCM_PatientID] = "";
    d->patientLevelAttributes[DCM_PatientName] = "";
    d->patientLevelAttributes[DCM_PatientSex] = "";
    medStringParameter *patientID = new medStringParameter("Patient ID", this);
    patientID->setCaption("Patient ID");
    QObject::connect(patientID, &medStringParameter::valueEdited, [&](QString const &value)
                     {
        if (value.isEmpty()) 
        {
            d->patientLevelAttributes[DCM_PatientID] = "";
        } else 
        {
            d->patientLevelAttributes[DCM_PatientID] = "*" + value + "*";
        } });
    medStringParameter *patientName = new medStringParameter("Patient Name", this);
    patientName->setCaption("Patient Name");
    QObject::connect(patientName, &medStringParameter::valueEdited, [&](QString const &value)
                     {
        if (value.isEmpty()) 
        {
            d->patientLevelAttributes[DCM_PatientName] = "";
        } else 
        {
            d->patientLevelAttributes[DCM_PatientName] = "*" + value + "*";
        } });
    medStringListParameter *gender = new medStringListParameter("Gender", this);
    gender->addItems({"", "M", "F"});
    gender->setCaption("Gender");
    QObject::connect(gender, &medStringListParameter::valueChanged, [&, gender](int const &value)
                     {
        if (value == 0) 
        {
            d->patientLevelAttributes[DCM_PatientSex] = "";
        } else 
        {
            d->patientLevelAttributes[DCM_PatientSex] = gender->value();
        } });

    d->studyLevelAttributes[DCM_StudyDescription] = "";
    d->studyLevelAttributes[DCM_StudyDate] = "";
    medStringParameter *studyDescription = new medStringParameter("Study Description", this);
    studyDescription->setCaption("Study Description");
    QObject::connect(studyDescription, &medStringParameter::valueEdited, [&](QString const &value)
                     {
        if (value == 0) 
        {
            d->studyLevelAttributes[DCM_StudyDescription] = "";
        } else 
        {
            d->studyLevelAttributes[DCM_StudyDescription] = "*" + value + "*";
        } });

    d->startDate = new medStringParameter("Start Study Date", this);
    d->startDate->setCaption("Start Study Date");
    d->startDate->setDefaultRepresentation(1);

    d->endDate = new medStringParameter("End Study Date", this);
    d->endDate->setCaption("End Study Date");
    d->endDate->setDefaultRepresentation(1);
    QDate currentDate = QDate::currentDate();
    d->startDate->setValue(currentDate.addYears(-100).toString("yyyyMMdd"));
    d->endDate->setValue(currentDate.toString("yyyyMMdd"));
    QObject::connect(d->startDate, &medStringParameter::valueChanged, this, &medAPHP::computeDateRange);
    QObject::connect(d->endDate, &medStringParameter::valueChanged, this, &medAPHP::computeDateRange);

    d->seriesLevelAttributes[DCM_SeriesDescription] = "";
    d->seriesLevelAttributes[DCM_Modality] = "";

    medStringParameter *seriesDescription = new medStringParameter("Series Description", this);
    seriesDescription->setCaption("Series Description");

    QObject::connect(seriesDescription, &medStringParameter::valueEdited, [&](QString const &value)
                     {
        if (value.isEmpty()) 
        {
            d->seriesLevelAttributes[DCM_SeriesDescription] = "";
        } else 
        {
            d->seriesLevelAttributes[DCM_SeriesDescription] = "*" + value + "*";
        } });

    medStringListParameter *modality = new medStringListParameter("Modality", this);
    modality->addItems({"", "MR", "CT", "PET"});
    modality->setCaption("Modality");
    QObject::connect(modality, &medStringListParameter::valueChanged, [&, modality](int const &value)
                     {
        if (value == 0) 
        {
            d->seriesLevelAttributes[DCM_Modality] = "";
        } else 
        {
            d->seriesLevelAttributes[DCM_Modality] = modality->value();
        } });

    medGroupParameter *patientFilter = new medGroupParameter("Patient Level Filters", this);
    patientFilter->setCaption("Patient Level");
    patientFilter->setDescription("Settings related to Patient Level");
    patientFilter->addParameter(patientName);
    patientFilter->addParameter(patientID);
    patientFilter->addParameter(gender);

    medGroupParameter *studyFilter = new medGroupParameter("Study Level Filters", this);
    studyFilter->setCaption("Study Level");
    studyFilter->setDescription("Settings related to Study Level");
    studyFilter->addParameter(studyDescription);
    studyFilter->addParameter(d->startDate);
    studyFilter->addParameter(d->endDate);

    medGroupParameter *seriesFilter = new medGroupParameter("Series Level Filters", this);
    seriesFilter->setCaption("Series Level");
    seriesFilter->setDescription("Settings related to Series Level");
    seriesFilter->addParameter(seriesDescription);
    seriesFilter->addParameter(modality);

    d->filterPacsSettings = new medGroupParameter("filter PACS Settings", this);
    d->filterPacsSettings->setCaption("PACS C-Find Parameters");
    d->filterPacsSettings->setDescription("Settings related to filter C-Find on PACS");
    d->filterPacsSettings->addParameter(patientFilter);
    d->filterPacsSettings->addParameter(studyFilter);
    d->filterPacsSettings->addParameter(seriesFilter);
}

medAPHP::~medAPHP()
{
    delete d->qtdcm;
    delete d->restFulAPI;
}

bool medAPHP::initialization(const QString &pi_instanceId)
{
    bool bRes = !pi_instanceId.isEmpty();
    if (bRes)
    {
        d->instanceId = pi_instanceId;
    }
    return bRes;
}

void medAPHP::onSettingsSaved()
{
    // WARn : Is it usefull ?
    d->qtdcm->updateLocalParameters(s_Aetitle.value(), s_Hostname.value(), s_Port.value());
    d->qtdcm->updateRemoteParameters(d->remoteAet->value(), d->remoteHostname->value(), d->remotePort->value());
    d->restFulAPI->updateUrl(d->restFulUrl->value());
}

void medAPHP::onFiltersApplied()
{
    qDebug() << "here we go";
}

void medAPHP::computeDateRange()
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

bool medAPHP::setInstanceName(const QString &pi_instanceName)
{
    bool bRes = !pi_instanceName.isEmpty();
    if (bRes)
    {
        d->instanceName = pi_instanceName;
    }
    return bRes;
}

bool medAPHP::connect(bool pi_bEnable)
{
    bool bRes = false;
    if (pi_bEnable)
    {
        d->qtdcm->updateLocalParameters(s_Aetitle.value(), s_Hostname.value(), s_Port.value());
        d->qtdcm->updateRemoteParameters(d->remoteAet->value(), d->remoteHostname->value(), d->remotePort->value());
        d->restFulAPI->updateUrl(d->restFulUrl->value());

        int respCode = d->qtdcm->sendEcho();
        bRes = !respCode;
        if (bRes && d->restFulAPI->hasUrl())
        {
            respCode = d->restFulAPI->isServerAvailable();
            bRes = !respCode;
        }
        d->isOnline = bRes;
    }
    else
    {
        d->isOnline = false;
        bRes = true;
    }
    return bRes;
}

QList<medAbstractParameter *> medAPHP::getAllParameters()
{
    return {d->localSettings, d->remoteSettings, d->restFulSettings, d->saveSettingsButton};
}

QList<medAbstractParameter *> medAPHP::getCipherParameters()
{
    return {};
}

QList<medAbstractParameter *> medAPHP::getVolatilParameters()
{
    return {};
}

QList<medAbstractParameter *> medAPHP::getFilteringParameters()
{
    return {d->filterPacsSettings};
}

bool medAPHP::isWritable()
{
    return true;
}

bool medAPHP::isLocal()
{
    return false;
}

bool medAPHP::isCached()
{
    return false;
}

bool medAPHP::isOnline()
{
    return d->isOnline;
}

bool medAPHP::isFetchByMinimalEntriesOrMandatoryAttributes()
{
    return false;
}

QString medAPHP::getInstanceName()
{
    return d->instanceName;
}

QString medAPHP::getInstanceId()
{
    return d->instanceId;
}

unsigned int medAPHP::getLevelCount()
{
    return d->levelNames.size();
}

unsigned int medAPHP::getLevelDesiredWritable()
{
    return 3;
}

QStringList medAPHP::getLevelNames()
{
    return d->levelNames;
}

QString medAPHP::getLevelName(unsigned int pi_uiLevel)
{
    QString retVal = "Invalid Level Name";
    if (pi_uiLevel >= 0 && pi_uiLevel < static_cast<unsigned int>(d->levelNames.size()))
    {
        retVal = d->levelNames.value((int)pi_uiLevel);
    }
    return retVal;
}

bool medAPHP::isLevelWritable(unsigned int pi_uiLevel)
{
    return pi_uiLevel == 3;
}

QStringList medAPHP::getMandatoryAttributesKeys(unsigned int pi_uiLevel)
{
    switch (pi_uiLevel)
    {
        case 0:
            return {"id", "description", "patientID", "gender", "birthdate"};
        case 1:
            return {"id", "description", "uid", "study date"};
        case 2:
            return {"id", "description", "uid", "modality", "institution name", "acquisition number", "number of series related instances"};
        case 3:
            return {"id", "description", "uid"};
        default:
            return QStringList();
    }
}

QList<medAbstractSource::levelMinimalEntries> medAPHP::getMinimalEntries(unsigned int pi_uiLevel, QString parentId)
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
        case 3:
            if (d->restFulAPI->hasUrl())
            {
                infos = d->restFulAPI->findAnnotationMinimalEntries(key);
                if (!infos.empty())
                {
                    for (const auto &info : infos)
                    {
                        levelMinimalEntries entry;
                        entry.key = info.value("SeriesInstanceUID");
                        entry.name = info.value("Description");
                        entry.description = info.value("SeriesInstanceUID");
                        entries.append(entry);
                    }
                }
            }
            break;
        default:
            break;
    }

    return entries;
}

QList<medAbstractSource::levelMinimalEntries> medAPHP::getPatientMinimalEntries(const QList<QMap<DcmTagKey, QString>> &infosMap)
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

QList<medAbstractSource::levelMinimalEntries> medAPHP::getStudyMinimalEntries(const QList<QMap<DcmTagKey, QString>> &infosMap, bool isSeriesFiltered)
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

QList<medAbstractSource::levelMinimalEntries> medAPHP::getSeriesMinimalEntries(const QList<QMap<DcmTagKey, QString>> &infosMap)
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

QList<QMap<QString, QString>> medAPHP::getMandatoryAttributes(unsigned int pi_uiLevel, QString parentId)
{
    QList<QMap<QString, QString>> attributesRes;
    QString parentKey;
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
    auto studyLevelValues = d->studyLevelAttributes.values();
    studyLevelValues.removeAll("");

    QMap<DcmTagKey, QString> tags;
    switch (pi_uiLevel)
    {
        case 0:
        {
            if (seriesLevelValues.isEmpty() && studyLevelValues.isEmpty())
            {
                auto infosPatient = cFindPatient(parentKey);
                attributesRes = getPatientMandatoryAttributes(infosPatient);
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
            bool isSeriesFiltered = !seriesLevelValues.isEmpty();
            attributesRes = getStudyMandatoryAttributes(infosStudy, isSeriesFiltered);
        }
        break;
        case 2:
        {
            auto infosSeries = cFindSeries(parentKey);
            attributesRes = getSeriesMandatoryAttributes(infosSeries);
        }
        break;
        case 3:
            if (d->restFulAPI->hasUrl())
            {
                auto infos = d->restFulAPI->findAnnotationMinimalEntries(parentKey);
                if (!infos.empty())
                {
                    for (const auto &info : infos)
                    {
                        QMap<QString, QString> attributes;
                        attributes["id"] = info.value("SeriesInstanceUID");
                        attributes["description"] = info.value("Description");
                        attributes["uid"] = info.value("SeriesInstanceUID");
                        attributesRes.append(attributes);
                    }
                }
            }
            break;
        default:
            break;
    }

    return attributesRes;
}

QList<QMap<DcmTagKey, QString>> medAPHP::cFindPatient(const QString &patientID)
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
    tags[DCM_PatientBirthDate] = "";
    patientsRes = d->qtdcm->cFind(tags);
    return patientsRes;
}

QList<QMap<DcmTagKey, QString>> medAPHP::cFindStudy(const QString &studyInstanceUID, const QString &patientID)
{
    QList<QMap<DcmTagKey, QString>> studyRes;
    QMap<DcmTagKey, QString> tags;

    tags[DCM_QueryRetrieveLevel] = "STUDY";
    for (auto key : d->studyLevelAttributes.keys())
    {
        tags.insert(key, d->studyLevelAttributes.value(key));
    }
    tags[DCM_PatientID] = patientID;
    tags[DCM_StudyInstanceUID] = studyInstanceUID;
    studyRes = d->qtdcm->cFind(tags);
    return studyRes;
}

QList<QMap<DcmTagKey, QString>> medAPHP::cFindSeries(const QString &studyInstanceUID)
{
    QList<QMap<DcmTagKey, QString>> seriesRes;
    QMap<DcmTagKey, QString> tags;

    tags[DCM_QueryRetrieveLevel] = "SERIES";
    for (auto key : d->seriesLevelAttributes.keys())
    {
        tags.insert(key, d->seriesLevelAttributes.value(key));
    }
    tags[DCM_StudyInstanceUID] = studyInstanceUID;
    tags[DCM_SeriesInstanceUID] = "";
    tags[DCM_InstitutionName] = "";
    tags[DCM_AcquisitionNumber] = "";
    tags[DCM_NumberOfSeriesRelatedInstances] = "";
    seriesRes = d->qtdcm->cFind(tags);
    return seriesRes;
}

QList<QMap<QString, QString>> medAPHP::getPatientMandatoryAttributes(const QList<QMap<DcmTagKey, QString>> &infosMap)
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

QList<QMap<QString, QString>> medAPHP::getStudyMandatoryAttributes(const QList<QMap<DcmTagKey, QString>> &infosMap, bool isSeriesFiltered)
{
    QList<QMap<QString, QString>> attributesRes;

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
            attributes["description"] = info[DCM_StudyDescription];
            attributes["uid"] = info[DCM_StudyInstanceUID];
            attributes["study date"] = info[DCM_StudyDate];
            attributesRes.append(attributes);
        }
    }

    return attributesRes;
}

QList<QMap<QString, QString>> medAPHP::getSeriesMandatoryAttributes(const QList<QMap<DcmTagKey, QString>> &infosMap)
{
    QList<QMap<QString, QString>> attributesRes;

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
    return attributesRes;
}

bool medAPHP::getAdditionalAttributes(unsigned int pi_uiLevel, QString id, datasetAttributes4 &po_attributes)
{
    return false;
}

QVariant medAPHP::getDirectData(unsigned int pi_uiLevel, QString key)
{
    // return QVariant();
    QVariant iPath;
    int iRequestId = getAssyncData(pi_uiLevel, key);

    timer.setSingleShot(true);
    QEventLoop loop;
    QObject::connect(&timer, &QTimer::timeout, &loop, [&]()
                     { loop.exit(-12); });
    timer.start(timeout);

    if (pi_uiLevel != 3)
    {
        QObject::connect(d->qtdcm, &QtDcmInterface::pathToData, &loop, [&](int id, const QString &path)
                         {
            if (iRequestId == id)
            {
                iPath = QVariant(path);
                loop.quit();
            } });
    }
    else
    {
        QObject::connect(d->restFulAPI, &medAbstractAnnotation::pathToData, &loop, [&](int id, const QString &path)
                         {
            if (iRequestId == id)
            {
                iPath = QVariant(path);
                loop.quit();
            } });
    }
    int status = loop.exec();
    if (status == 0)
    {
        qDebug() << "Ok path to Data = " << iPath.toString();
    }
    else
    {
        qDebug() << "no path to data " << status;
    }
    return iPath;
}

int medAPHP::getAssyncData(unsigned int pi_uiLevel, QString key)
{
    int iRes = -1;
    s_RequestId++;
    QString levelKey = QString::number(pi_uiLevel) + ":" + key;
    if (d->levelKeyToRequestIdMap.contains(levelKey))
    {
        int requestId = d->levelKeyToRequestIdMap[levelKey];
        if (pi_uiLevel > 0 && pi_uiLevel < 3)
        {
            if (d->qtdcm->isCachedDataPath(requestId))
            {
                iRes = requestId;
            }
        }
        else if (pi_uiLevel == 3 && !(d->restFulUrl->value().isEmpty()))
        {
            if (d->restFulAPI->isCachedDataPath(requestId))
            {
                iRes = requestId;
            }
        }
    }
    if (iRes == -1)
    {
        d->levelKeyToRequestIdMap[levelKey] = s_RequestId;
        if (pi_uiLevel > 0 && pi_uiLevel < 3)
        {
            iRes = getQtDcmAsyncData(pi_uiLevel, key);
        }
        else if (pi_uiLevel == 3 && !(d->restFulUrl->value().isEmpty()))
        {
            iRes = getAnnotationAsyncData(key);
        }
    }
    return iRes;
}

int medAPHP::getAnnotationAsyncData(const QString &key)
{
    int iRes = -1;
    if (d->restFulAPI->getAnnotationData(s_RequestId, key))
    {
        iRes = s_RequestId;

        QObject::connect(d->restFulAPI, &medAbstractAnnotation::pathToData, this, [&](int id, const QString &path)
                         { d->requestIdToResultsMap[id] = QVariant(path); });

        QObject::connect(
            d->restFulAPI, &medAbstractAnnotation::inProgress, this,
            [=](int pi_requestId, eRequestStatus status)
            {
                switch (status)
                {
                    case pending:
                    {
                        timer.setInterval(timeout);
                        break;
                    }
                    case finish:
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

int medAPHP::getQtDcmAsyncData(unsigned int pi_uiLevel, const QString &key)
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

    if (d->qtdcm->moveRequest(s_RequestId, queryLevel, key))
    {
        iRes = s_RequestId;

        QObject::connect(d->qtdcm, &QtDcmInterface::pathToData, this, [&](int id, const QString &path)
                         { d->requestIdToResultsMap[id] = QVariant(path); });

        QObject::connect(
            d->qtdcm, &QtDcmInterface::moveProgress, this, [=](int pi_requestId, int status)
            {
                eRequestStatus requestStatus;
                switch (status)
                {
                case 0: // moveStatus::OK
                {
                    requestStatus = finish;
                    break;
                }
                case 1: // moveStatus::PENDING
                {
                    requestStatus = pending;
                    timer.setInterval(timeout);
                    break;
                }
                case -1: // moveStatus::KO
                default:
                {
                    requestStatus = faild;
                    break;
                }
                }
                emit progress(pi_requestId, requestStatus); },
            Qt::UniqueConnection);
    }

    return iRes;
}

//
// QString medAPHP::addData(QVariant data, QStringList parentUri, QString name)
//{
//    QString keyRes;
//
//    int level  = parentUri.size() - 1;
//
//    switch (level)
//    {
//        case 0: //Patient
//        case 1: //Study
//        case 2://Series
//            break;
//        case 3:// Annotations
//        {
//            keyRes = d->restFulAPI->addData(data, name, parentUri.last());
//
//            break;
//        }
//        default: //Unknown level
//        {
//            break;
//        }
//    }
//
//    return keyRes;
//}

void medAPHP::abort(int pi_iRequest)
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
        d->qtdcm->stopMove(pi_iRequest);
    }
    else if (level == 3 && !(d->restFulUrl->value().isEmpty()))
    {
        d->restFulAPI->abortDownload(pi_iRequest);
    }
}

bool medAPHP::createPath(QList<levelMinimalEntries> &pio_path, const medAbstractSource::datasetAttributes4 &pi_attributes,
                         unsigned int pi_uiLevel, QString parentKey)
{
    return false;
}

int medAPHP::getIOInterface()
{
    return IO_FILE;
}

QMap<QString, QStringList> medAPHP::getTypeAndFormat()
{
    QMap<QString, QStringList> supportedTypeAndFormats;
    supportedTypeAndFormats["Image"] = QStringList({".mha"});

    return supportedTypeAndFormats;
}

bool medAPHP::addDirectData(QVariant data, medAbstractSource::levelMinimalEntries &pio_minimalEntries,
                            unsigned int pi_uiLevel, QString parentKey)
{
    return false;
}

int medAPHP::addAssyncData(QVariant data, medAbstractSource::levelMinimalEntries &pio_minimalEntries,
                           unsigned int pi_uiLevel, QString parentKey)
{
    int iRes = -1;
    if (pi_uiLevel == 3)
    {
        s_RequestId++;
        QString name = pio_minimalEntries.name;
        QString key = d->restFulAPI->addData(data, name, parentKey);
        d->requestIdToResultsMap[s_RequestId] = QVariant(key);
    }
    return iRes;
}

bool medAPHP::createFolder(medAbstractSource::levelMinimalEntries &pio_minimalEntries,
                           const medAbstractSource::datasetAttributes4 &pi_attributes, unsigned int pi_uiLevel,
                           QString parentKey)
{
    return false;
}

bool medAPHP::alterMetaData(const medAbstractSource::datasetAttributes4 &pi_attributes, unsigned int pi_uiLevel, QString key)
{
    return false;
}

bool medAPHP::getThumbnail(QPixmap &po_thumbnail, unsigned int pi_uiLevel, QString key)
{
    return false;
}

bool medAPHP::setThumbnail(QPixmap &pi_thumbnail, unsigned int pi_uiLevel, QString key)
{
    return false;
}

bool medAPHP::commitData(QVariant data, medAbstractSource::levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel,
                         QString parentKey)
{
    return false;
}

QVariant medAPHP::getAsyncResults(int pi_iRequest)
{
    QVariant dataRes;
    if (d->requestIdToResultsMap.contains(pi_iRequest))
    {
        dataRes = d->requestIdToResultsMap[pi_iRequest];
    }
    return dataRes;
}

int medAPHP::push(unsigned int pi_uiLevel, QString key)
{
    return 0;
}
