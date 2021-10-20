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
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QHttpMultiPart>
#include <QFile>

medAPHP::medAPHP(QtDcmInterface *dicomLib): m_DicomLib(dicomLib)
{
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
        m_instanceId = pi_instanceName;
    }
    return bRes;
}

bool medAPHP::connect(bool pi_bEnable)
{
    bool bRes = false;
    if (pi_bEnable)
    {
        int respCode = m_DicomLib->sendEcho(m_Aetitle->value(), m_Hostname->value(),
                            m_ServerAet->value(), m_ServerHostname->value(),
                            m_ServerPort->value());

        m_Manager = new QNetworkAccessManager(this);

        QObject::connect(m_Manager, &QNetworkAccessManager::finished,
                         this, &medAPHP::replyFinished);

//        QString url = m_AnnotationUrl->value() + QString("/annotations/series/annotation");
//        m_Manager->get(QNetworkRequest(QUrl(url)));

        QNetworkRequest postRequest;
        postRequest.setUrl(QUrl(m_AnnotationUrl->value() +
        "/annotations/series/1.3.6.1.4.1.14519.5.2.1.3671.4754.216645718571142540899096273555"
        "/annotation/1.2.826.0.1.3680043.2.1143.4644061347865562232933004322459135589"
        "?annotation_type=json&annotation_subtype=&partner=medInria"));

        auto postMultiPart = new QHttpMultiPart();
        postMultiPart->setContentType(QHttpMultiPart::FormDataType);

        postRequest.setRawHeader(QByteArray("Accept"), QByteArray("multipart/form-data; boundary=BoUnDaRyStRiNg"));

        QHttpPart partData;
        partData.setHeader(QNetworkRequest::ContentDispositionHeader,
                           QVariant(R"(form-data; name="annotationData"; filename="1.2.826.0.1.3680043.2.1143.4644061347865562232933004322459135589.json")"));
        partData.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));

        auto file = new QFile("/Users/castelne/Library/Application Support/inria/medInria/0000000000/1.2.826.0.1.3680043.2.1143.2731159993210582763862514613329574197/1.2.826.0.1.3680043.2.1143.4644061347865562232933004322459135589.json");
        file->open(QIODevice::ReadOnly);
        partData.setBodyDevice(file);

        postMultiPart->append(partData);
        m_Manager->post(postRequest, postMultiPart);

        if (respCode == 0)
        {
            bRes = true;
            emit progress(0, eRequestStatus::finish);
        }
        else
        {
            bRes = false;
            emit abort(0);
        }
    }
    else
    {
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

bool medAPHP::isWriteable()
{
    return false;
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
    return false;
}

QString medAPHP::getInstanceName()
{
    return {};
}

QString medAPHP::getInstanceId()
{
    return {};
}

unsigned int medAPHP::getLevelCount()
{
    return 0;
}

QStringList medAPHP::getLevelNames()
{
    return {};
}

QString medAPHP::getLevelName(unsigned int pi_uiLevel)
{
    return {};
}

QStringList medAPHP::getMandatoryAttributesKeys(unsigned int pi_uiLevel)
{
    return {};
}

QStringList medAPHP::getAdditionalAttributesKeys(unsigned int pi_uiLevel)
{
    return {};
}

QList<medAbstractSource::levelMinimalEntries> medAPHP::getMinimalEntries(unsigned int pi_uiLevel, QString parentId)
{
    return {};
}

QList<QMap<QString, QString>> medAPHP::getMandatoryAttributes(unsigned int pi_uiLevel, int id)
{
    return {};
}

QList<QMap<QString, QString>> medAPHP::getAdditionalAttributes(unsigned int pi_uiLevel, int id)
{
    return {};
}

QString medAPHP::getDirectData(unsigned int pi_uiLevel, QString key)
{
    return {};
}

int medAPHP::getAssyncData(unsigned int pi_uiLevel, QString id)
{
    return 0;
}

bool medAPHP::addData(void *data, QString uri)
{
    return false;
}

void medAPHP::abort(int pi_iRequest)
{

}

void medAPHP::replyFinished(QNetworkReply *reply)
{
    if(reply->error())
    {
        qDebug() << "ERROR!";
        qDebug() << reply->errorString();
    }
    else
    {
        qDebug() << reply->header(QNetworkRequest::ContentTypeHeader).toString();
        qDebug() << reply->header(QNetworkRequest::LastModifiedHeader).toDateTime().toString();;
        qDebug() << reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();
        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug() << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
        QString strReply = (QString)reply->readAll();

        //parse json
        qDebug() << "Response:" << strReply;
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
        QJsonArray json_array = jsonResponse.array();

        foreach (const QJsonValue &value, json_array) {
            QJsonObject json_obj = value.toObject();
            qDebug() << json_obj["annotation_uid"].toString();
            qDebug() << json_obj["annotation_type"].toString();
            qDebug() << json_obj["annotation_subtype"].toString();
            qDebug() << json_obj["series_uid"].toString();
        }
        auto file = new QFile("/Users/castelne/developer/tmp/AnnotRestApi.txt");
        if(file->open(QFile::Append))
        {
            file->write(reply->readAll());
            file->flush();
            file->close();
        }
        delete file;
    }

    reply->deleteLater();
}
