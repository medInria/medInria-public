

#include "medAnnotation.h"

#include <QEventLoop>
#include <QHttpMultiPart>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QTimer>


medAnnotation::medAnnotation()
{
}

medAnnotation::~medAnnotation()
{
    for (AnnotationRequest *request : requestIdMap.values())
    {
        delete request;
    }
    requestIdMap.clear();
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

bool medAnnotation::hasUrl()
{
    return m_url != QString();
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

bool medAnnotation::getAnnotationData(int pi_requestId, const QString &annotation_uid)
{
    bool bRes = true;
    QString requestUrl = m_url + "/annotations/" + annotation_uid;
    auto annotRequest = new AnnotationRequest(pi_requestId);
//    annotRequest->moveToThread(m_Manager.thread());
    annotRequest->getRequest(&m_Manager, requestUrl);
    connect(&m_Manager, SIGNAL(finished(QNetworkReply *)), annotRequest, SLOT(finishDownload(QNetworkReply *)));
    connect(annotRequest, &AnnotationRequest::progress, this, &medAbstractAnnotation::inProgress);

    requestIdMap[pi_requestId] = annotRequest;

    return bRes;
}

bool medAnnotation::isCachedDataPath(int requestId)
{
    bool bRes = false;
    auto request = requestIdMap[requestId];
    if (request && (request->m_requestId == requestId) && (!request->m_path.isEmpty()))
    {
        bRes = true;
        // TODO move signal emission ?
        emit request->progress(requestId, medAbstractSource::eRequestStatus::finish, request->m_path);
    }
    return bRes;
}

bool medAnnotation::addData(int requestId, QVariant dataset, QString name, QString &seriesUid)
{
    bool bRes = true;
    QString requestUrl = m_url +
                         "/annotations/series/" + seriesUid +
                         "/annotation/";
    auto annotationRequest = new AnnotationRequest(requestId);
    annotationRequest->postRequest(&m_Manager, requestUrl, name, dataset);
    connect(&m_Manager, SIGNAL(finished(QNetworkReply *)), annotationRequest, SLOT(finishUpload(QNetworkReply *)));
    connect(annotationRequest, &AnnotationRequest::progress, this, &medAbstractAnnotation::inProgress);

//    QTimer timer;
//    timer.setSingleShot(true);
//    QEventLoop loop;
//    QObject::connect(&timer, &QTimer::timeout, &loop, [&]() {
//        loop.exit(-1);
//    });
//    timer.start(10000);
//
//    QObject::connect(&manager, &QNetworkAccessManager::finished, &loop, [&](QNetworkReply *reply) {
//
////        qDebug()<<"reply "<<reply->errorString()<<" "<<reply->error();
//        switch (reply->error())
//        {
//            case QNetworkReply::NoError:
//            {
//                QString strReply = (QString)reply->readAll();
////                qDebug()<<"reply "<<strReply;
//                res = annotationUid;
//                loop.quit();
//                break;
//            }
//            case QNetworkReply::ConnectionRefusedError:
//            default:
//                loop.exit(reply->error());
//                break;
//        }
//
//    });
//    loop.exec();

    return bRes;
}

void medAnnotation::abortDownload(int pi_requestId)
{
    if (requestIdMap.contains(pi_requestId))
    {
        auto request = requestIdMap[pi_requestId];
        request->abort();
        requestIdMap.remove(pi_requestId);
        delete request;
    }

}

void medAnnotation::updateUrl(QString const & url)
{
    m_url = url;
}



