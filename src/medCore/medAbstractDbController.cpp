#include "medAbstractDbController.h"

medDataIndex medAbstractDbController::import( const medDataIndex& index, const medAbstractDbController& controller ) 
{
    dtkAbstractData* data = controller.read(index);
    return import(data);
}
