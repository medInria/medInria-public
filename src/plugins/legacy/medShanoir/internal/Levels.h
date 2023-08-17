#ifndef LEVELS
#define LEVELS

#include <QString>
#include <QList>
#include <QDate>
#include <QJsonObject>
#include <QJsonArray>

struct ProcessedDataset
{
    int id;
    QString name;
    QString type;
};

struct DatasetProcessing
{
    int id;
    QString type;
    QString date;
    QList<int> inputDatasets; // ids of input datasets
    QList<ProcessedDataset> outputDatasets; // ids of output datasets (processedDatasets)
    int studyId;
};


struct ExportProcessedDataset
{
    QString name;
    QString type;
    QString filepath; 
};

struct DatasetDetails
{
    int id;
    QString name;
    QString type;
    int study_id;
    int subject_id;
    QString subject_name;
};

struct Dataset
{
    int id;
    QString name;
    QString type;
    QList<DatasetProcessing>  processings; // list of processing datasets that already exist
};

struct DatasetAcquisition
{
    int id;
    QString name;
    QList<Dataset> datasets;
};

struct Examination
{
    int id;
    QString comment;
    QDate date;
    QList<DatasetAcquisition> ds_acquisitions;
};

struct Subject
{
    int id;
    QString name;
    QList<Examination> examinations;
};

struct SubjectOverview
{
    int id;
    QString name;
    QString type;
};

struct StudyOverview
{
    int id;
    QString name;
};

struct Study
{
    int id;
    QString name;
    QList<SubjectOverview> subjects;
};

/**
* Finds an element in the provided list of Levels based on the given ID.
* @tparam T The type of elements in the list (a level type).
* @param levels The list of elements to search in. Each element has an id public attribute. 
* @param id The ID to match.
* @return An iterator pointing to the found element, or end() if not found.
*/
template <typename T> 
static inline auto findLevelElement(const QList<T> &levels,int id)
{
    QList<T>::const_iterator it = std::find_if(levels.begin(), levels.end(), [id](const T& level) 
    {
        return level.id == id;
    });
    return it;
}


#endif