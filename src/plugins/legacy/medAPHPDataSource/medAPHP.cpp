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

std::atomic<int> medAPHP::s_RequestId = -1;

medAPHP::medAPHP(QtDcmInterface *dicomLib, medAbstractAnnotation *annotationAPI):
                    m_LevelNames({"PATIENT","STUDY","SERIES","ANNOTATION"}),
                    m_isOnline(false), m_DicomLib(dicomLib), m_AnnotationAPI(annotationAPI)
{
    timeout = 60000;
    m_Aetitle = new medStringParameter("AE Title", this);
    m_Aetitle->setCaption("AE Title (Application Entity Title) used by the plugin (AE) to identify itself. ");
    m_Aetitle->setValue("MEDINRIA");

    m_Hostname = new medStringParameter("Hostname", this);
    m_Hostname->setCaption("Local Hostname of the plugin");
    m_Hostname->setValue("localhost");

    m_Port = new medIntParameter("TCP Port", this);
    m_Port->setCaption("Local TCP Port of the plugin");
    m_Port->setValue(2010);

    m_ServerAet = new medStringParameter("Server AE Title");
    m_ServerAet->setCaption("AE Title (Application Entity Title) used by the plugin (AE) to identify remote PACS");
    m_ServerAet->setValue("SERVER"); //Local Sphere Pacs

    m_ServerHostname = new medStringParameter("Server Hostname");
    m_ServerHostname->setCaption("Hostname of the PACS to access");
    m_ServerHostname->setValue("localhost");

    m_ServerPort = new medIntParameter("Server Port");
    m_ServerPort->setCaption("TCP port of the PACS to access");
    m_ServerPort->setValue(11112);

    m_AnnotationUrl = new medStringParameter("Rest API Anotation URL ");
    m_AnnotationUrl->setCaption("URL to access to Annotation Rest API");
    m_AnnotationUrl->setValue("http://127.0.0.1:5555");

    m_DicomLib->setConnectionParameters(m_Aetitle->value(), m_Hostname->value(), m_Port->value(),
                                        m_ServerAet->value(),m_ServerHostname->value(),
                                        m_ServerPort->value());
    m_AnnotationAPI->setUrl(m_AnnotationUrl->value());

    m_PatientName = new medStringParameter("Patient Name", this);
    m_PatientName->setCaption("(0010,0010)");
//    connect(m_PatientName, &medStringParameter::valueChanged, m_DicomLib, &QtDcmInterface::patientNameFilter);
    m_PatientId = new medStringParameter("Patient ID", this);
    m_PatientId->setCaption("(0010,0020)");
//    connect(m_PatientId, &medStringParameter::valueChanged, m_DicomLib, &QtDcmInterface::patientIdFilter);
}

bool medAPHP::initialization(const QString &pi_instanceId)
{
    bool bRes = !pi_instanceId.isEmpty();
    if (bRes)
    {
        m_instanceId = pi_instanceId;
    }
    return bRes;
}

bool medAPHP::setInstanceName(const QString &pi_instanceName)
{
    bool bRes = !pi_instanceName.isEmpty();
    if (bRes)
    {
        m_instanceName = pi_instanceName;
    }
    return bRes;
}

bool medAPHP::connect(bool pi_bEnable)
{
    bool bRes = false;
    if (pi_bEnable)
    {
        int respCode = m_DicomLib->sendEcho();
        bRes = !respCode;
        if (bRes)
        {
            respCode = m_AnnotationAPI->isServerAvailable();
            bRes = !respCode;
        }
        m_isOnline = bRes;
    }
    else
    {
        m_isOnline = false;
        bRes = true;
    }
    return bRes;
}

QList<medAbstractParameter *> medAPHP::getAllParameters()
{
    return {};
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
    listRes << m_PatientName << m_PatientId;
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
    return m_isOnline;
}

bool medAPHP::isFetchByMinimalEntriesOrMandatoryAttributes()
{
    return true;
}

QString medAPHP::getInstanceName()
{
    return m_instanceName;
}

QString medAPHP::getInstanceId()
{
    return m_instanceId;
}

unsigned int medAPHP::getLevelCount()
{
    return m_LevelNames.size();
}

QStringList medAPHP::getLevelNames()
{
    return m_LevelNames;
}

QString medAPHP::getLevelName(unsigned int pi_uiLevel)
{
    QString retVal = "Invalid Level Name";
    if (pi_uiLevel>=0 && pi_uiLevel<static_cast<unsigned int>(m_LevelNames.size()))
    {
        retVal = m_LevelNames.value((int)pi_uiLevel);
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
            infos = m_DicomLib->findPatientMinimalEntries();
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
            infos = m_DicomLib->findStudyMinimalEntries(key);
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
            infos = m_DicomLib->findSeriesMinimalEntries(key);
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
            infos = m_AnnotationAPI->findAnnotationMinimalEntries(key);
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
        QObject::connect(m_DicomLib, &QtDcmInterface::pathToData, &loop, [&](int id, const QString &path) {
            if (iRequestId == id)
            {
                iPath = QVariant(path);
                loop.quit();
            }
        });
    }
    else
    {
        QObject::connect(m_AnnotationAPI, &medAbstractAnnotation::pathToData, &loop, [&](int id, const QString &path) {
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
    int iRequest = -1;

    if (pi_uiLevel > 0 && pi_uiLevel < 3)
    {
        iRequest = getQtDcmAsyncData(pi_uiLevel, key);
    }
    else if (pi_uiLevel == 3)
    {
        iRequest = m_AnnotationAPI->getAnnotationData(key);
        if (iRequest != -1)
        {
            pendingRequestId[key] = iRequest;

            QObject::connect(m_AnnotationAPI, &medAbstractAnnotation::inProgress, this,
                             [=](int requestId, medAbstractSource::eRequestStatus status) {
                switch (status)
                {
                    case eRequestStatus::finish:
                    {
                        QString key = pendingRequestId.key(iRequest);
                        pendingRequestId.remove(key);
                        break;
                    }
                    case eRequestStatus::pending:
                    {
                        break;
                    }
                    case eRequestStatus::faild:
                    default:
                    {
                        QString key = pendingRequestId.key(iRequest);
                        pendingRequestId.remove(key);
                        break;
                    }
                }
                emit progress(iRequest, status);

                }, Qt::UniqueConnection);
        }
    }
    return iRequest;
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

    if (m_DicomLib->moveRequest(s_RequestId++, queryLevel, key))
    {
        iRes = s_RequestId;
//        pendingRequestId[key] = iRequestId;

        QObject::connect(m_DicomLib, &QtDcmInterface::moveProgress, this, [=](int pi_requestId, int status) {
            eRequestStatus requestStatus;
            switch (status)
            {
                case 0: //moveStatus::OK
                {
                    requestStatus = finish;
//                    QString key = pendingRequestId.key(pi_requestId);
//                    pendingRequestId.remove(key);
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
//                    QString key = pendingRequestId.key(iRequestId);
//                    pendingRequestId.remove(key);
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
            keyRes = m_AnnotationAPI->addData(data, name, parentUri.last());

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
    m_DicomLib->stopMove(pi_iRequest);

}