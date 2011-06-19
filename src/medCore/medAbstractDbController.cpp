#include "medAbstractDbController.h"



medDataIndex medAbstractDbController::import( const medDataIndex& index, const medAbstractDbController& controller ) 
{
    dtkAbstractData* data = controller.read(index).data();
    return import(data);
}

void medAbstractDbController::clear (void)
{
}

void medAbstractDbController::remove (const medDataIndex &index)
{
}
