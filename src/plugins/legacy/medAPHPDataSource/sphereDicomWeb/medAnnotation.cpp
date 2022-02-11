/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "medAnnotation.h"

#include <dcmtk/dcmdata/dcuid.h>
#include <QTimer>
#include <QThread>
#include <QEventLoop>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QHttpMultiPart>

int medAnnotation::requestId = -1;

void annotationDownloader::doWork(const QString& requestUrl, int pi_requestId)
{
    m_requestId = pi_requestId;
    m_Manager = new QNetworkAccessManager();
    m_Reply = m_Manager->get(QNetworkRequest(QUrl(requestUrl)));

    QObject::connect(m_Manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(finish(QNetworkReply *)));
    QObject::connect(m_Reply, SIGNAL(errorOccurred(QNetworkReply::NetworkError)), this, SLOT(onErrorOccured(
            QNetworkReply::NetworkError)));
    QObject::connect(m_Reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(inProgress(qint64, qint64)));

}

void annotationDownloader::onErrorOccured(QNetworkReply::NetworkError error)
{
    if (error != QNetworkReply::NoError )
    {
        // TODO To improve, check the value of error and decide what to do !
        emit downloadProgress(m_requestId, medAbstractSource::eRequestStatus::cnxLost);
    }
}

void annotationDownloader::inProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    emit downloadProgress(m_requestId, medAbstractSource::eRequestStatus::pending);
}

void annotationDownloader::finish(QNetworkReply *reply)
{
    switch (reply->error())
    {
        case QNetworkReply::NoError:
        {
            emit downloadProgress(m_requestId, medAbstractSource::eRequestStatus::finish);
            QByteArray data = reply->readAll();
            if (m_TemporaryDir.isValid())
            {
                QDir dir(m_TemporaryDir.path());
                dir.mkdir(  QString::number(m_requestId));
                auto fullFilePath = dir.filePath(QString::number(m_requestId) + "/tmpAnnotation.mha");
                QFile file(fullFilePath);
                file.open(QIODevice::WriteOnly);
                file.write(data);
                file.close();
                emit pathToData(m_requestId, fullFilePath);
            }
            break;
        }

        case QNetworkReply::ConnectionRefusedError:
        default:
            emit downloadProgress(m_requestId, medAbstractSource::eRequestStatus::faild);
            break;
    }

}





medAnnotation::medAnnotation()//: m_Manager(new QNetworkAccessManager)
{

}

medAnnotation::~medAnnotation()
{
//    delete m_Manager;
}

int medAnnotation::isServerAvailable()
{
    int respCode = -1;

    auto manager = QNetworkAccessManager();
    manager.get(QNetworkRequest(QUrl(m_url)));

    QTimer timer;
    timer.setSingleShot(true);
    QEventLoop loop;
    QObject::connect(&timer, &QTimer::timeout, &loop, [&]() {
        loop.exit(-1);
    });
    timer.start(10000);

    QObject::connect(&manager, &QNetworkAccessManager::finished, &loop, [&](QNetworkReply *reply) {

        switch (reply->error())
        {
            case QNetworkReply::ContentNotFoundError:
                loop.quit();
                break;
            case QNetworkReply::ConnectionRefusedError:
            default:
                loop.exit(reply->error());
                break;
        }

    });
    respCode = loop.exec();
    return respCode;

}

QList<QMap<QString, QString>> medAnnotation::findAnnotationMinimalEntries(const QString &seriesInstanceUID)
{
    QList<QMap<QString, QString>> infos;

    QString requestUrl = m_url + "/annotations/series/annotation?series_uid=" + seriesInstanceUID;
    auto manager = QNetworkAccessManager();
    manager.get(QNetworkRequest(QUrl(requestUrl)));

    QTimer timer;
    timer.setSingleShot(true);
    QEventLoop loop;
    QObject::connect(&timer, &QTimer::timeout, &loop, [&]() {
        loop.exit(-1);
    });
    timer.start(10000);

    QObject::connect(&manager, &QNetworkAccessManager::finished, &loop, [&](QNetworkReply *reply) {

        qDebug()<<"reply "<<reply->errorString()<<" "<<reply->error();
        switch (reply->error())
        {
            case QNetworkReply::NoError:
            {
                QString strReply = (QString)reply->readAll();
                //parse json
                QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
                QJsonArray json_array = jsonResponse.array();
                foreach (const QJsonValue &value, json_array) {
                    QMap<QString, QString> info;
                    QJsonObject json_obj = value.toObject();
                    QByteArray meta = json_obj["annotation_meta"].toString().toUtf8();
                    QJsonDocument doc;
                    doc = QJsonDocument::fromJson(meta);
                    if (doc.isObject() && doc["name"].isString())
                    {
                        info.insert("Description", doc["name"].toString());
                    }
                    info.insert("SeriesInstanceUID",json_obj["annotation_uid"].toString());
                    infos.append(info);
                }

                loop.quit();
                break;
            }
            case QNetworkReply::ConnectionRefusedError:
            default:
                loop.exit(reply->error());
                break;
        }

    });
    loop.exec();
    return infos;
}

int medAnnotation::getAnnotationData(const QString &annotation_uid)
{
    QString requestUrl = m_url + "/annotations/" + annotation_uid;

    auto downloader = new annotationDownloader();
    downloader->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, downloader, &QObject::deleteLater);
    connect(this, &medAnnotation::operate, downloader, &annotationDownloader::doWork);
    connect(downloader, &annotationDownloader::downloadProgress, this, &medAbstractAnnotation::inProgress);
    connect(downloader, &annotationDownloader::pathToData, this, &medAbstractAnnotation::pathToData);
    workerThread.start();

    requestIdMap[medAnnotation::requestId++] = downloader;
    emit operate(requestUrl, medAnnotation::requestId);

    return medAnnotation::requestId;
}

QString medAnnotation::addData(QVariant dataset, QString name, QString &seriesUid)
{
    QString res;
    char uid[100];
    dcmGenerateUniqueIdentifier(uid, SITE_SERIES_UID_ROOT);
    QString annotationUid = QString(uid);

    auto manager = QNetworkAccessManager();
    QNetworkRequest postRequest;
    QUrl url(m_url +
    "/annotations/series/" + seriesUid +
    "/annotation/" + annotationUid);
    QUrlQuery query;

    query.addQueryItem("annotation_type", "mha");
    query.addQueryItem("annotation_subtype", "");
    query.addQueryItem("partner", "medInria");
    QJsonObject obj;
    obj["name"] = name;
    QJsonDocument doc(obj);
    auto data = doc.toJson(QJsonDocument::Compact);
//    qDebug()<<"doc "<<doc.toJson(QJsonDocument::Compact);
//    QByteArray data = doc.toJson(QJsonDocument::Compact).toBase64();
    query.addQueryItem("annotation_meta", data);
    url.setQuery(query.query());
    postRequest.setUrl(url);


//    QString request = m_url +"/annotations/series/" +
//                      seriesUid +
//                      "/annotation/" +
//                      annotationUid +
//                      R"(?annotation_type=mha&annotation_subtype=&partner=medInria&annotation_meta={name: foo})" ;//+
//                      //name + R"(}")";
//
//    postRequest.setUrl(QUrl(request));

    auto postMultiPart = new QHttpMultiPart();
    postMultiPart->setContentType(QHttpMultiPart::FormDataType);

    postRequest.setRawHeader(QByteArray("Accept"), QByteArray("multipart/form-data; boundary=BoUnDaRyStRiNg"));

    QHttpPart partData;
    QString filename = annotationUid + ".mha";
    QString header = R"(form-data; name="annotationData"; filename=")" + annotationUid + R"(.mha")";
    partData.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QVariant(header));
    partData.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));

    auto mhaFile = new QFile(dataset.toString());
    bool bOpen = mhaFile->open(QIODevice::ReadOnly);
    partData.setBodyDevice(mhaFile);

    postMultiPart->append(partData);
    manager.post(postRequest, postMultiPart);

    QTimer timer;
    timer.setSingleShot(true);
    QEventLoop loop;
    QObject::connect(&timer, &QTimer::timeout, &loop, [&]() {
        loop.exit(-1);
    });
    timer.start(10000);

    QObject::connect(&manager, &QNetworkAccessManager::finished, &loop, [&](QNetworkReply *reply) {

//        qDebug()<<"reply "<<reply->errorString()<<" "<<reply->error();
        switch (reply->error())
        {
            case QNetworkReply::NoError:
            {
                QString strReply = (QString)reply->readAll();
//                qDebug()<<"reply "<<strReply;
                res = annotationUid;
                loop.quit();
                break;
            }
            case QNetworkReply::ConnectionRefusedError:
            default:
                loop.exit(reply->error());
                break;
        }

    });
    loop.exec();

    return annotationUid;
}


