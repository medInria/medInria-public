#include "ProcessedDatasetSender.h"


ProcessedDatasetSender::ProcessedDatasetSender(int id, Authenticater & auth, StudyOverview study, QString subjectName, Dataset dataset, QString processingDate, QString processingType, ExportProcessedDataset processedDataset) :DataSender(id, auth), m_study(study),  m_subjectName(subjectName), m_dataset(dataset), m_processingDate(processingDate), m_processingType(processingType), m_processedDataset(processedDataset), m_success(false)
{
}

void ProcessedDatasetSender::ProcessedDatasetSender::run()
{
    QString path = sendProcessedDataset(m_processedDataset.filepath);
    QJsonObject datasetProcessing =  sendDatasetProcessing(m_study.id, m_dataset, m_processingDate, m_processingType);
    m_processedDataset.filepath = path;
    m_success = sendProcessedDatasetContext(m_processedDataset, m_study, m_subjectName, m_dataset.type, datasetProcessing);
    emit dataSent(getId());
}

bool ProcessedDatasetSender::isSuccessful()
{
    return m_success;
}
	