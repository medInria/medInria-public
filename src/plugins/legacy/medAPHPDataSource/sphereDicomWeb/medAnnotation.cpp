

#include "medAnnotation.h"
#include "annotationDownloader.h"

#include <dcmtk/dcmdata/dcuid.h>
#include <QHttpMultiPart>

medAnnotation::medAnnotation()
{

}

medAnnotation::~medAnnotation()
{
    for (annotationDownloader *downloader : requestIdMap.values())
    {
        delete downloader;
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

    auto downloader = new annotationDownloader();
    downloader->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, downloader, &QObject::deleteLater);
    connect(this, &medAnnotation::operate, downloader, &annotationDownloader::doWork);
    connect(downloader, &annotationDownloader::downloadProgress, this, &medAbstractAnnotation::inProgress);
    connect(downloader, &annotationDownloader::pathToData, this, &medAbstractAnnotation::pathToData);
    workerThread.start();

    requestIdMap[pi_requestId] = downloader;
    emit operate(requestUrl, pi_requestId);

    return bRes;
}

bool medAnnotation::isCachedDataPath(int requestId)
{
    bool bRes = false;
    auto downloader = requestIdMap[requestId];
    if (downloader && (downloader->m_requestId == requestId) && (!downloader->m_path.isEmpty()))
    {
        bRes = true;
        // TODO move signal emission ?
        emit downloader->pathToData(requestId, downloader->m_path);
    }
    return bRes;
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

void medAnnotation::abortDownload(int pi_requestId)
{
    if (requestIdMap.contains(pi_requestId))
    {
        auto downloader = requestIdMap[pi_requestId];
        downloader->abort();
        requestIdMap.remove(pi_requestId);
        delete downloader;
    }

}

void medAnnotation::updateUrl(QString const & url)
{
    m_url = url;
}



