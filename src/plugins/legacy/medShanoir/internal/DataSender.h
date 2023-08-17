#include <QRunnable>
#include <QObject>
#include <QString>

#include <Network.h>
#include <Levels.h>
#include <Authenticater.h>

class DataSender : public QObject, public QRunnable
{
	Q_OBJECT
protected:
    Authenticater & m_auth;
    int m_id;

    QString sendProcessedDataset(QString filepath);

    QJsonObject sendDatasetProcessing(int studyId, Dataset dataset,  QString processingDate, QString processingType);

    bool sendProcessedDatasetContext(ProcessedDataset processedDataset, StudyOverview study, QString subjectName, QString datasetType, QJsonObject datasetProcessing);


public:
    DataSender( int id, Authenticater & auth, QObject *parent = nullptr);

    virtual void run()=0;

    int getId();

signals:
    void dataSent(int /*id*/);
};