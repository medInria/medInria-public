/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "medAnnotation.h"

#include <QTimer>
#include <QThread>
#include <QEventLoop>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

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

                    info.insert("SeriesInstanceUID",json_obj["annotation_uid"].toString());
                    // TODO Find Description in metadata sent by APHP EDS
                    info.insert("Description", "TBD");
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


