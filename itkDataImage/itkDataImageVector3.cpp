#include "medItk.h"

#include "itkDataImageVector3.h"

#include "medITKDataImageMacros.h"

typedef itk::Vector<unsigned char, 3> UCharVectorType;

medImplementITKVectorDataImage (UCharVectorType, 3, Vector3);
