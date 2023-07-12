#ifndef LEVELS
#define LEVELS

#include <QString>
#include <QList>
#include <QDate>


struct Dataset
{
    int id;
    QString name;
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