#include "itkFiltersProcessBase.h"

itkFiltersProcessBase::itkFiltersProcessBase() : dtkAbstractProcess()
{
}


itkFiltersProcessBase::~itkFiltersProcessBase()
{
    
}

void itkFiltersProcessBase::emitProgress(int progress)
{
    emit progressed(progress);
}
