#include "medAbstractDbController.h"



void medAbstractDbController::import( const medDataIndex& index, const medAbstractDbController& controller )
{
    dtkAbstractData* data = controller.read(index).data();
    import(data);
}

void medAbstractDbController::clear (void)
{
}

void medAbstractDbController::remove (const medDataIndex &index)
{
}
