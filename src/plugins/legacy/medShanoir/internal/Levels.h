#ifndef LEVELS
#define LEVELS

#include <QString>
#include <QList>
#include <QDate>
#include <QJsonObject>
#include <QJsonArray>


/**
 * @brief This file contains the data structures used to represent the different tree-view elements of shanoir, in order to make them easier to manipulate.
 * @note You may see a lot of structs with the same attributes. It is because here we only manipulate the necessary data, and not the whole data that is returned by the server.
 * However, if a specific data came to change, or if we need to add a new attribute, we will only have to change the corresponding struct, and not the whole code.
 * Moreover, the struct names are explicit, so it is easy to understand what they represent.
 * @note the keyword Overview from certain struct names means that the element is manipulated as a child element, contained by a parent element.
 * @note the keyword Details from certain struct names means that the element is manipulated as an element itself, and not as a child element.
 * @note the keywork Export from certain struct names means that the element is manipulated as an element that does not exists yet in the tree-view, and is going to be exported to the server.
*/

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


struct DatasetOverview
{
    int id;
    QString name;
    QString type;
};

struct DatasetProcessingOverview
{
    int id;
    QString type;
    QString date;
    QList<DatasetOverview> inputDatasets; 
    QList<DatasetOverview> outputDatasets; 
    int studyId;
};

struct Dataset
{
    int id;
    QString name;
    QString type;
    QList<DatasetProcessingOverview>  processings; // list of processing datasets that already exist
};

struct DatasetAcquisitionOverview
{
    int id;
    QString name;
    QList<Dataset> datasets;
};

struct ExaminationOverview
{
    int id;
    QString comment;
    QDate date;
    QList<DatasetAcquisitionOverview> ds_acquisitions;
};

struct SubjectDetails
{
    int id;
    QString name;
    QList<ExaminationOverview> examinations;
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

struct StudyDetails
{
    int id;
    QString name;
    QList<SubjectOverview> subjects;
};

/**
* Finds an element in the provided list of Levels based on the given ID.
* @param T The type of elements in the list (a level type).
* @param levels The list of elements to search in. Each element has an id public attribute. 
* @param id The ID to match.
* @return An iterator pointing to the found element, or end() if not found.
*/
template <typename T> 
static inline auto findLevelElement(const QList<T> &levels, int id)
{
    QList<T>::const_iterator it = std::find_if(levels.begin(), levels.end(), [id](const T& level) 
    {
        return level.id == id;
    });
	if (it == levels.end())
	{
		it = nullptr;
	}
    return it;
}


#endif