#include "itkBinaryMask.h"

const char itkBinaryMaskName[] = "itkBinaryMask";

bool itkBinaryMask::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataType("itkBinaryMask", createITKBinaryMask);
}


dtkAbstractData *createITKBinaryMask(){
    return new itkBinaryMask;
}