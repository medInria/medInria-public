#include "annotationRequest.h"
#include <QHttpMultiPart>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlQuery>
#include <QUrlQuery>
#include <dcmtk/dcmdata/dcuid.h>

AnnotationRequest::AnnotationRequest(int requestId)
{
    m_requestId = requestId;
    m_AnnotationUid = "";
}

void AnnotationRequest::getRequest(QNetworkAccessManager *manager, const QString &requestUrl)
{
    QNetworkRequest myRequest = QNetworkRequest(QUrl(requestUrl));

    m_Reply = manager->get(myRequest);
    connect(m_Reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onErrorOccured(
            QNetworkReply::NetworkError)));
    connect(m_Reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(inProgress(qint64, qint64)));
}


void AnnotationRequest::postRequest(QNetworkAccessManager *manager, QString &requestUrl, const QString &name,
                                    QVariant dataset)
{
    QNetworkRequest postRequest;
    char uid[100];
    dcmGenerateUniqueIdentifier(uid, SITE_SERIES_UID_ROOT);
    m_AnnotationUid = QString(uid);
    requestUrl += m_AnnotationUid;
    QUrl url(requestUrl);
    QUrlQuery query;

    query.addQueryItem("annotation_type", "mha");
    query.addQueryItem("annotation_subtype", "");
    query.addQueryItem("partner", "medInria");
    QJsonObject obj;
    obj["name"] = name;
    QJsonDocument doc(obj);
    auto data = doc.toJson(QJsonDocument::Compact);
    query.addQueryItem("annotation_meta", data);
    url.setQuery(query.query());
    postRequest.setUrl(url);

    m_PostMultiPart.setContentType(QHttpMultiPart::FormDataType);

    postRequest.setRawHeader(QByteArray("Accept"), QByteArray("multipart/form-data; boundary=BoUnDaRyStRiNg"));

    QHttpPart partData;
    QString filename = m_AnnotationUid + ".mha";
    QString header = R"(form-data; name="annotationData"; filename=")" + m_AnnotationUid + R"(.mha")";
    partData.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QVariant(header));
    partData.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));

    m_File.setFileName(dataset.toString());
    bool bOpen = m_File.open(QIODevice::ReadOnly);
    if (bOpen)
    {
        partData.setBodyDevice(&m_File);
        m_File.setParent(&m_PostMultiPart);
        m_PostMultiPart.append(partData);
        m_Reply = manager->post(postRequest, &m_PostMultiPart);
        connect(m_Reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onErrorOccured(
                QNetworkReply::NetworkError)));
        connect(m_Reply, SIGNAL(uploadProgress(qint64, qint64)), this, SLOT(inProgress(qint64, qint64)));
    }
    else
    {
        emit progress(m_requestId, medAbstractSource::eRequestStatus::faild);
    }
}

void AnnotationRequest::finishDownload(QNetworkReply *reply)
{
    switch (reply->error())
    {
        case QNetworkReply::NoError:
        {
            QByteArray data = reply->readAll();
            if (m_TemporaryDir.isValid())
            {
                QDir dir(m_TemporaryDir.path());
                dir.mkdir(  QString::number(m_requestId));
                m_path = dir.filePath(QString::number(m_requestId) + "/tmpAnnotation.mha");
                QFile file(m_path);
                file.open(QIODevice::WriteOnly);
                file.write(data);
                file.close();
                QVariant data(m_path);
                emit progress(m_requestId, medAbstractSource::eRequestStatus::finish, data);

            }
            break;
        }

        case QNetworkReply::ConnectionRefusedError:
        default:
            emit progress(m_requestId, medAbstractSource::eRequestStatus::faild);
            break;
    }

}

void AnnotationRequest::finishUpload(QNetworkReply *reply)
{
    switch (reply->error())
    {
        case QNetworkReply::NoError:
        {
            QString strReply = (QString)reply->readAll();
                qDebug()<<"reply "<<strReply;
            QVariant data(m_AnnotationUid);
            emit progress(m_requestId, medAbstractSource::eRequestStatus::finish, data);
            break;
        }
        case QNetworkReply::ConnectionRefusedError:
        default:
            emit progress(m_requestId, medAbstractSource::eRequestStatus::faild);
            break;
    }

}

void AnnotationRequest::onErrorOccured(QNetworkReply::NetworkError error)
{
    if (error != QNetworkReply::NoError )
    {
        // TODO To improve, check the value of error and decide what to do !
        emit progress(m_requestId, medAbstractSource::eRequestStatus::cnxLost);
    }

}

void AnnotationRequest::inProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    emit progress(m_requestId, medAbstractSource::eRequestStatus::pending);
}

void AnnotationRequest::abort()
{
    if (m_Reply)
    {
        m_Reply->abort();
    }
}


