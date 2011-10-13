#include "medAbstractDbController.h"
#include "medDataIndex.h"


void medAbstractDbController::import( const medDataIndex& index, const medAbstractDbController& controller )
{
    qDebug() << "DEBUG : entering medAbstractDbController::import(const medDataIndex& index, const medAbstractDbController& controller )";
    dtkSmartPointer<dtkAbstractData> data( controller.read(index) );
    import(data);
}

void medAbstractDbController::clear (void)
{
}

void medAbstractDbController::remove (const medDataIndex &index)
{
}
