#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <JsonReaderWriter.h>
#include <QJsonDocument>
#include <QDateTime>

#include "DataSender.h"

DataSender::DataSender(int id, Authenticater & auth, QObject *parent) : QObject(parent), m_id(id), m_auth(auth)
{
}

int DataSender::getId()
{
    return m_id;
}

QString DataSender::sendProcessedDataset(QString filepath)
{
    QString url = m_auth.getBaseURL() + "import/importer/upload_processed_dataset/";
    Network net;
    QNetworkReply *reply = net.httpPostFileSender(url, { {"Authorization", "Bearer " + m_auth.getCurrentAccessToken().toUtf8()} }, filepath);

    QString path;

    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200) {
        path = reply->readAll();
        qDebug().noquote().nospace() << "Processed dataset upload successful ("<<path<<")";
    } else {
        qDebug() << "Processed dataset upload failed. Status code:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    }
    return path;
}


QJsonObject DataSender::sendDatasetProcessing(int studyId, Dataset dataset, QString processingDate, QString processingType)
{
    QString url = m_auth.getBaseURL() + "datasets/datasetProcessing";
  
    QJsonObject inputDatasets;
    inputDatasets.insert("id", dataset.id);
    inputDatasets.insert("type", dataset.type);

    QJsonObject data;
    data.insert("datasetProcessingType", processingType);
    data.insert("inputDatasets", QJsonArray() << inputDatasets);
    data.insert("outputDatasets", QJsonArray());
    data.insert("processingDate", processingDate);
    data.insert("studyId", studyId);

    QJsonObject header;
    header.insert("Authorization", ("Bearer " + m_auth.getCurrentAccessToken()));
    header.insert("Content-Type", "application/json");

    Network net;
    QNetworkReply *reply = net.httpPostFormDataSender(url,header, data);

    return JsonReaderWriter::qbytearrayToQJson(reply->readAll());
}


bool DataSender::sendProcessedDatasetContext(ExportProcessedDataset processedDataset, StudyOverview study, QString subjectName, QString datasetType, QJsonObject datasetProcessing)
{
    QJsonObject data;
    data["subjectName"] =  subjectName;
    data["studyName"] = study.name;
    data["studyId"] = study.id;
    data["datasetType"] = datasetType;
    data["processedDatasetFilePath"] = processedDataset.filepath;
    data["processedDatasetType"] = processedDataset.type;
    data["processedDatasetName"] = processedDataset.name;

    data["datasetProcessing"] = datasetProcessing;

    data["timestamp"] = QDateTime::currentMSecsSinceEpoch();

    QJsonObject header;
    header.insert("Authorization", ("Bearer " + m_auth.getCurrentAccessToken()));
    header.insert("Content-Type", "application/json");

    Network net;
    QString url = m_auth.getBaseURL() + "datasets/datasets/processedDataset";
    QNetworkReply *reply =  net.httpPostFormDataSender(url, header, data); 

    int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    return code==200;
}