#include "medAbstractDbController.h"



medDataIndex medAbstractDbController::import( const medDataIndex& index, const medAbstractDbController& controller ) 
{
    dtkSmartPointer<dtkAbstractData> data( controller.read(index) );
    return import(data);
}

void medAbstractDbController::clear (void)
{
}

void medAbstractDbController::remove (const medDataIndex &index)
{
}
