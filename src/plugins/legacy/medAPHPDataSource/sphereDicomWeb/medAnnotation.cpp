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

static int requestId = -1;

void restFulWorker::run()
{

    auto manager = new QNetworkAccessManager();
    auto reply = manager->get(QNetworkRequest(QUrl(m_requestUrl)));

    QObject::connect(manager, &QNetworkAccessManager::finished, [&](QNetworkReply *reply) {

        qDebug()<<"reply "<<reply->errorString()<<" "<<reply->error();
        switch (reply->error())
        {
            case QNetworkReply::NoError:
            {
                QByteArray data = reply->readAll();
                emit finished();
                break;
            }

            case QNetworkReply::ConnectionRefusedError:
            default:
                emit failed();
                break;
        }

    });

    QObject::connect(reply, &QNetworkReply::downloadProgress, [this](qint64 bytesReceived, qint64 bytesTotal){
        qDebug()<<"in progress";
        emit inProgress();
    });

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
                    QString meta = json_obj["annotation_meta"].toString();
                    meta = QString(QByteArray::fromBase64(meta.toLatin1()));
                    qDebug()<<"meta "<<meta;
                    QJsonDocument doc;
                    doc = QJsonDocument::fromJson(QByteArray::fromBase64(meta.toLatin1()));
                    if (doc.isObject())
                    {
                        QJsonObject obj = doc.object();
                        for (auto key : obj.keys())
                        {
                            qDebug()<<key<<" : "<<obj[key];
                        }
                    }
                    info.insert("SeriesInstanceUID",json_obj["annotation_uid"].toString());
                    // TODO Find Description in metadata sent by APHP EDS
                    info.insert("Description", json_obj["partner"].toString());
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

    auto worker = new restFulWorker(requestUrl);

    requestIdMap[requestId++] = worker;
    connect( worker, &restFulWorker::inProgress, worker,[=](){
        emit getProgress(requestId, medAbstractSource::eRequestStatus::pending);
    });
    connect( worker, &restFulWorker::finished, worker,[=](){
        emit getProgress(requestId, medAbstractSource::eRequestStatus::finish);
    });
    connect( worker, &restFulWorker::failed, worker,[=](){
        emit getProgress(requestId, medAbstractSource::eRequestStatus::faild);
    });
    worker->start();

    return requestId;
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
    QByteArray data = doc.toJson(QJsonDocument::Compact).toBase64();
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

        qDebug()<<"reply "<<reply->errorString()<<" "<<reply->error();
        switch (reply->error())
        {
            case QNetworkReply::NoError:
            {
                QString strReply = (QString)reply->readAll();
                qDebug()<<"reply "<<strReply;
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


