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
#include <QObject>

#include <medIntParameter.h>
#include <medStringParameter.h>
#include <medGroupParameter.h>
#include <medTriggerParameter.h>

std::atomic<int>   medAPHP::s_RequestId = -1;
medStringParameter medAPHP::s_Aetitle("AE Title");
medStringParameter medAPHP::s_Hostname("Hostname");
medIntParameter    medAPHP::s_Port("TCP Port");

struct medAPHPParametersPrivate
{
    QString instanceId;
    QString instanceName;
    QStringList levelNames;
    bool isOnline;
    QMap<QString, int> levelKeyToRequestIdMap;

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
    medStringParameter *patientName;
    medStringParameter *patientId;

};


medAPHP::medAPHP(QtDcmInterface *dicomLib, medAbstractAnnotation *annotationAPI): d(new medAPHPParametersPrivate)
{
    timeout = 60000;

    d->levelNames << "PATIENTS" << "STUDY" << "SERIES" << "ANNOTATION";
    d->isOnline = false;

    d->qtdcm = dicomLib;
    d->restFulAPI = annotationAPI;

    d->localSettings = new medGroupParameter("Local dicom settings", this);
    d->localSettings->addParameter(&s_Aetitle);
    d->localSettings->addParameter(&s_Hostname);
    d->localSettings->addParameter(&s_Port);

    d->remoteAet = new medStringParameter("Server AE Title");
    d->remoteHostname = new medStringParameter("Server Hostname");
    d->remotePort = new medIntParameter("Server Port");

    d->remoteSettings = new medGroupParameter("Dicom server Settings", this);
    d->remoteSettings->addParameter(d->remoteAet);
    d->remoteSettings->addParameter(d->remoteHostname);
    d->remoteSettings->addParameter(d->remotePort);

    d->restFulUrl = new medStringParameter("Rest API Anotation URL ");

    d->restFulSettings = new medGroupParameter("annotation API Settings", this);
    d->restFulSettings->addParameter(d->restFulUrl);

    d->saveSettingsButton = new medTriggerParameter("Save", this);
    QObject::connect(d->saveSettingsButton, SIGNAL(triggered()), this, SLOT(onSettingsSaved()));

    d->patientName = new medStringParameter("Patient Name", this);
    d->patientName->setCaption("(0010,0010)");
//  d->  connect(m_PatientName, &medStringParameter::valueChanged, m_DicomLib, &QtDcmInterface::patientNameFilter);
    d->patientId = new medStringParameter("Patient ID", this);
    d->patientId->setCaption("(0010,0020)");
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
        if (bRes)
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
    QList<medAbstractParameter *> listRes;
    listRes << d->patientName << d->patientId;
    return listRes;
}

bool medAPHP::isWriteable()
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
    return true;
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

QStringList medAPHP::getLevelNames()
{
    return d->levelNames;
}

QString medAPHP::getLevelName(unsigned int pi_uiLevel)
{
    QString retVal = "Invalid Level Name";
    if (pi_uiLevel>=0 && pi_uiLevel<static_cast<unsigned int>(d->levelNames.size()))
    {
        retVal = d->levelNames.value((int)pi_uiLevel);
    }
    return retVal;
}

QStringList medAPHP::getMandatoryAttributesKeys(unsigned int pi_uiLevel)
{
    switch (pi_uiLevel)
    {
        case 0:
            return {"id", "description", "patientID"};
        case 1:
            return {"id", "description", "uid"};
        case 2:
            return {"id", "description", "uid"};
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
        key = splittedUri[(int)pi_uiLevel-1];
    }
    else
    {
        key = parentId;
    }

    QList<QMap<QString, QString>> infos;
    switch (pi_uiLevel)
    {
        case 0:
            infos = d->qtdcm->findPatientMinimalEntries();
            if (!infos.empty())
            {
                for (const auto &info : infos)
                {
                    levelMinimalEntries entry;
                    entry.key = info.value("ID");
                    entry.name = info.value("Name");
                    entry.description = info.value("ID");
                    entries.append(entry);
                }
            }
            break;
        case 1:
            infos = d->qtdcm->findStudyMinimalEntries(key);
            if (!infos.empty())
            {
                for (const auto &info : infos)
                {
                    levelMinimalEntries entry;
                    entry.key = info.value("StudyInstanceUID");
                    entry.name = info.value("Description");
                    entry.description = info.value("StudyInstanceUID");
                    entries.append(entry);
                }
            }
            break;
        case 2:
            infos = d->qtdcm->findSeriesMinimalEntries(key);
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

            break;
        case 3:
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
            break;
        default:
            break;
    }

    return entries;
}

QList<QMap<QString, QString>> medAPHP::getMandatoryAttributes(unsigned int pi_uiLevel, QString parentId)
{
    return {};
}

bool medAPHP::getAdditionalAttributes(unsigned int pi_uiLevel, QString id, datasetAttributes4 &po_attributes)
{
    return false;
}

QVariant medAPHP::getDirectData(unsigned int pi_uiLevel, QString key)
{
    QVariant iPath;
    int iRequestId = getAssyncData(pi_uiLevel, key);

    timer.setSingleShot(true);
    QEventLoop loop;
    QObject::connect(&timer, &QTimer::timeout, &loop, [&]() {
        loop.exit(-12);
    });
    timer.start(timeout);

    if (pi_uiLevel!=3)
    {
        QObject::connect(d->qtdcm, &QtDcmInterface::pathToData, &loop, [&](int id, const QString &path) {
            if (iRequestId == id)
            {
                iPath = QVariant(path);
                loop.quit();
            }
        });
    }
    else
    {
        QObject::connect(d->restFulAPI, &medAbstractAnnotation::pathToData, &loop, [&](int id, const QString &path) {
            if (iRequestId == id)
            {
                iPath = QVariant(path);
                loop.quit();
            }
        });
    }
    int status = loop.exec();
    if (status == 0)
    {
        qDebug() << "Ok path to Data = " << iPath.toString();
    }
    else
    {
        qDebug()<<"no path to data "<<status;
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
            if (d->restFulAPI->isCahedDataPath(requestId))
            {
                iRes = requestId;
            }
        }
        else if (pi_uiLevel == 3)
        {
            if (d->restFulAPI->isCahedDataPath(requestId))
            {
                iRes = requestId;
            }
        }

    }
    if (iRes==-1)
    {
        d->levelKeyToRequestIdMap[levelKey] = s_RequestId;
        if (pi_uiLevel > 0 && pi_uiLevel < 3)
        {
            iRes = getQtDcmAsyncData(pi_uiLevel, key);
        }
        else if (pi_uiLevel == 3)
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

        QObject::connect(d->restFulAPI, &medAbstractAnnotation::inProgress, this,
                         [=](int pi_requestId, eRequestStatus status) {
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

            }, Qt::UniqueConnection);
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

        QObject::connect(d->qtdcm, &QtDcmInterface::moveProgress, this, [=](int pi_requestId, int status) {
            eRequestStatus requestStatus;
            switch (status)
            {
                case 0: //moveStatus::OK
                {
                    requestStatus = finish;
                    break;
                }
                case 1: //moveStatus::PENDING
                {
                    requestStatus = pending;
                    timer.setInterval(timeout);
                    break;
                }
                case -1: //moveStatus::KO
                default:
                {
                    requestStatus = faild;
                    break;
                }
            }
            emit progress(pi_requestId, requestStatus);

        }, Qt::UniqueConnection);
    }

    return iRes;
}

QString medAPHP::addData(QVariant data, QStringList parentUri, QString name)
{
    QString keyRes;

    int level  = parentUri.size() - 1;

    switch (level)
    {
        case 0: //Patient
        case 1: //Study
        case 2://Series
            break;
        case 3:// Annotations
        {
            keyRes = d->restFulAPI->addData(data, name, parentUri.last());

            break;
        }
        default: //Unknown level
        {
            break;
        }
    }

    return keyRes;
}

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
    else if (level == 3)
    {
        d->restFulAPI->abortDownload(pi_iRequest);
    }
}
