#include "medItk.h"

#include "itkDataImageVector3.h"

#include <itkVector.h>
#include <itkImage.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <itkScalarImageToHistogramGenerator.h>
#include <itkRGBPixel.h>
#include <itkGreyColormapFunctor.h>
#include <itkScalarToRGBColormapImageFilter.h>
#include <itkExtractImageFilter.h>
#include <itkShrinkImageFilter.h>
#include <itkDiscreteGaussianImageFilter.h>

#include <dtkCore/dtkAbstractDataFactory.h>

#include <QtGui>

#include "medITKDataImageMacros.h"

typedef itk::Vector<unsigned char, 3> UCharVectorType;

medImplementITKVectorDataImage (UCharVectorType, 3, Vector3);
