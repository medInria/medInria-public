#include "itkDataImageWriter.h"

#include "medItk.h"

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkLog.h>

//#include <itkVector.h>
#include <itkImage.h>
#include <itkImageFileWriter.h>

#include "medItkDataImageWriterMacros.h"

typedef itk::Vector<unsigned char, 3> UCharVectorType;
typedef itk::RGBPixel<unsigned char>  RGBPixelType;

medImplementItkDataImageWriterMacro (char,           3, Char3);
medImplementItkDataImageWriterMacro (unsigned char,  3, UChar3);
medImplementItkDataImageWriterMacro (short,          3, Short3);
medImplementItkDataImageWriterMacro (unsigned short, 3, UShort3);
medImplementItkDataImageWriterMacro (int,            3, Int3);
medImplementItkDataImageWriterMacro (unsigned int,   3, UInt3);
medImplementItkDataImageWriterMacro (long,           3, Long3);
medImplementItkDataImageWriterMacro (unsigned long,  3, ULong3);
medImplementItkDataImageWriterMacro (float,          3, Float3);
medImplementItkDataImageWriterMacro (double,         3, Double3);
medImplementItkDataImageWriterMacro (UCharVectorType, 3, Vector3);
medImplementItkDataImageWriterMacro (RGBPixelType, 3, RGB3);
