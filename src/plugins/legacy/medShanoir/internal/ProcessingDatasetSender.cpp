#include <QNetworkReply>

#include <JsonReaderWriter.h>
#include "ProcessingDatasetSender.h"


ProcessingDatasetSender::ProcessingDatasetSender(int id, Authenticater & auth, DatasetProcessing dspsing) :DataSender(id, auth), m_dspsing(dspsing),m_response(QJsonObject())
{

}

void ProcessingDatasetSender::ProcessingDatasetSender::run()
{
    QJsonObject datasetProcessing =  sendDatasetProcessing(m_dspsing.studyId, m_dspsing.inputDatasets.first(), m_dspsing.date, m_dspsing.type);
	m_response = datasetProcessing;
	emit dataSent(getId());
}

QJsonObject ProcessingDatasetSender::getResponse()
{
	return m_response;
}

QJsonObject ProcessingDatasetSender::sendDatasetProcessing(int studyId, DatasetOverview dataset, QString processingDate, QString processingType)
{
    QString url = m_auth.getBaseURL() + "datasets/datasetProcessing";
  
    QJsonObject inputDatasets;
    inputDatasets.insert("id", dataset.id);
    inputDatasets.insert("type", dataset.type);
	inputDatasets.insert("processings", QJsonArray());

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