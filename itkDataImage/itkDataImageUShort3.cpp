#include "medItk.h"

#include "itkDataImageUShort3.h"

#include <itkVector.h>
#include <itkImage.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <itkScalarImageToHistogramGenerator.h>
#include <itkRGBPixel.h>
#include <itkGreyColormapFunctor.h>
#include <itkScalarToRGBColormapImageFilter.h>
#include <itkExtractImageFilter.h>
#include <itkShrinkImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkDiscreteGaussianImageFilter.h>

#include <dtkCore/dtkAbstractDataFactory.h>

#include <QtGui>

#include "medITKDataImageMacros.h"

medImplementITKDataImage (unsigned short, 3, UShort3);
