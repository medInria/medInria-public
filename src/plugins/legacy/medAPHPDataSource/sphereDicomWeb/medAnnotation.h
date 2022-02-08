/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medAbstractAnnotation.h"

#include <QNetworkAccessManager>
#include <QString>

class restFulWorker : public QThread
{
    Q_OBJECT
public:
    explicit restFulWorker(QString requestUrl): m_requestUrl(requestUrl){};

    void run() override;

signals:
    void inProgress();
    void finished();
    void failed();

private:
    QString m_requestUrl;
};


class medAnnotation : public medAbstractAnnotation
{
    Q_OBJECT
public:
    medAnnotation();
    ~medAnnotation();


    /* ***********************************************************************/
    /* **************************** Check Connection *************************/
    /* ***********************************************************************/
    int isServerAvailable() override;


    /* ***********************************************************************/
    /* **************************** Request Get  *****************************/
    /* ***********************************************************************/
    QList<QMap<QString, QString>> findAnnotationMinimalEntries(const QString &seriesInstanceUID) override;
    int getAnnotationData(const QString &uid) override;
    QString addData(QVariant dataset, QString name, QString &seriesUid) override;


public slots:
    void moveProgression(){};
private:
//    QNetworkAccessManager *m_Manager;
    QMap<int, restFulWorker*> requestIdMap;

};

//        m_Manager = new QNetworkAccessManager(this);
//
//
//        QString url = m_AnnotationUrl->value() ;//+ QString("/annotations");
//        m_Manager->get(QNetworkRequest(QUrl(url)));
//
//        QObject::connect(m_Manager, &QNetworkAccessManager::finished, [&](QNetworkReply *reply){
//            qDebug()<<"reply "<<reply->errorString()<<" "<<reply->error();
//        });
//                         this, &medAPHP::replyFinished);

//        QNetworkRequest postRequest;
//        postRequest.setUrl(QUrl(m_AnnotationUrl->value() +
//        "/annotations/series/1.3.6.1.4.1.14519.5.2.1.3671.4754.216645718571142540899096273555"
//        "/annotation/1.2.826.0.1.3680043.2.1143.2731159993210582763862514613329574197"
//        "?annotation_type=mha&annotation_subtype=&partner=medInria&annotation_meta={\"createdDate\": \"2021-10-15T10:03:47.565\","
//        "        \"id\": \"1.2.826.0.1.3680043.2.1143.4644061347865562232933004322459135589\","
//        "        \"modifiedDate\": \"2021-10-15T10:03:47.565\","
//        "        \"userId\": \"\","
//        "        \"value\": {"
//        "            \"caracteristique\": {"
//        "                \"sequence\": \"0000/1.2.826.0.1.3680043.2.1143.2731159993210582763862514613329574197\""
//        "            },"
//        "            \"mha_filename\": \"1.2.826.0.1.3680043.2.1143.4644061347865562232933004322459135589\","
//        "            \"reference\": \"Daicap_Prostate_/1.2.826.0.1.3680043.2.1143.2731159993210582763862514613329574197_Segmentation\""
//        "        }}"));
//
//        auto postMultiPart = new QHttpMultiPart();
//        postMultiPart->setContentType(QHttpMultiPart::FormDataType);
//
//        postRequest.setRawHeader(QByteArray("Accept"), QByteArray("multipart/form-data; boundary=BoUnDaRyStRiNg"));
//
//        QHttpPart partData;
//        partData.setHeader(QNetworkRequest::ContentDispositionHeader,
//                           QVariant(R"(form-data; name="annotationData"; filename="1.2.826.0.1.3680043.2.1143.2731159993210582763862514613329574197.mha")"));
//        partData.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
//
//        auto mhaFile = new QFile("/Users/castelne/Library/Application Support/inria/medInria/0000000000/1.2.826.0.1.3680043.2.1143.2731159993210582763862514613329574197.mha");
//        mhaFile->open(QIODevice::ReadOnly);
//        partData.setBodyDevice(mhaFile);
//
//        QHttpPart jsonData;
//        jsonData.setHeader(QNetworkRequest::ContentDispositionHeader,
//                           QVariant(R"(form-data; name="jsonData"; filename="1.2.826.0.1.3680043.2.1143.2731159993210582763862514613329574197.json")"));
//        jsonData.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
//
//        auto jSonfile = new QFile("/Users/castelne/Library/Application Support/inria/medInria/0000000000/1.2.826.0.1.3680043.2.1143.2731159993210582763862514613329574197/1.2.826.0.1.3680043.2.1143.2731159993210582763862514613329574197.json");
//        jSonfile->open(QIODevice::ReadOnly);
//        jsonData.setBodyDevice(jSonfile);
//
//
//        postMultiPart->append(jsonData);
//        postMultiPart->append(partData);
//        m_Manager->post(postRequest, postMultiPart);
//        if (respCode == 0)
//        {
//            bRes = true;
//        }
//        else
//        {
//            bRes = false;
//        }
