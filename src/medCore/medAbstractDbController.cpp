#include "medAbstractDbController.h"
#include "medDataIndex.h"


void medAbstractDbController::import( const medDataIndex& index, const medAbstractDbController& controller )
{
    qDebug() << "DEBUG : entering medAbstractDbController::import(const medDataIndex& index, const medAbstractDbController& controller )";
    dtkSmartPointer<dtkAbstractData> data( controller.read(index) );
    import(data);
}

void medAbstractDbController::import(const QString& file,bool indexWithoutCopying)
{
    //qDebug() << "DEBUG : entering medAbstractDbController::import(const QString& file,bool indexWithoutCopying)";
}

void medAbstractDbController::exportDataToFile(dtkAbstractData *data, const QString &filename)
{
    //qDebug() << "DEBUG : entering medAbstractDbController::exportDataToFile(dtkAbstractData *data, const QString &filename)";
}

void medAbstractDbController::clear()
{
}

void medAbstractDbController::remove (const medDataIndex &index)
{
}
