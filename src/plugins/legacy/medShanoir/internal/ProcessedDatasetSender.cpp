#include <QNetworkReply>

#include "ProcessedDatasetSender.h"


ProcessedDatasetSender::ProcessedDatasetSender(int id, Authenticater & auth, StudyOverview study, QString subjectName, Dataset dataset, QString processingDate, QString processingType, ExportProcessedDataset processedDataset, QJsonObject datasetProcessing) :DataSender(id, auth), m_study(study),  m_subjectName(subjectName), m_dataset(dataset), m_processingDate(processingDate), m_processingType(processingType), m_processedDataset(processedDataset), m_datasetProcessing(datasetProcessing), m_success(false)
{
}

void ProcessedDatasetSender::ProcessedDatasetSender::run()
{
    QString path = sendProcessedDataset(m_processedDataset.filepath);
	qDebug() << path;
    m_processedDataset.filepath = path;
    m_success = sendProcessedDatasetContext(m_processedDataset, m_study, m_subjectName, m_dataset.type, m_datasetProcessing);
    emit dataSent(getId());
}

bool ProcessedDatasetSender::isSuccessful()
{
    return m_success;
}

bool ProcessedDatasetSender::sendProcessedDatasetContext(ExportProcessedDataset processedDataset, StudyOverview study, QString subjectName, QString datasetType, QJsonObject datasetProcessing)
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
	

QString ProcessedDatasetSender::sendProcessedDataset(QString filepath)
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