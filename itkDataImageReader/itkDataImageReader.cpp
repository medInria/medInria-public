#include "itkDataImageReader.h"

#include "medItk.h"

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkLog.h>

#include <itkVector.h>
#include <itkImage.h>
#include <itkImageFileReader.h>

#include "medItkDataImageReaderMacros.h"

typedef itk::Vector<unsigned char, 3> UCharVectorType;
typedef itk::RGBPixel<unsigned char>  RGBPixelType;

medImplementItkDataImageReader (char,           3, Char3,   CHAR, SCALAR);
medImplementItkDataImageReader (unsigned char,  3, UChar3,  UCHAR, SCALAR);
medImplementItkDataImageReader (short,          3, Short3,  SHORT, SCALAR);
medImplementItkDataImageReader (unsigned short, 3, UShort3, USHORT, SCALAR);
medImplementItkDataImageReader (int,            3, Int3,    INT, SCALAR);
medImplementItkDataImageReader (unsigned int,   3, UInt3,   UINT, SCALAR);
medImplementItkDataImageReader (long,           3, Long3,   LONG, SCALAR);
medImplementItkDataImageReader (unsigned,       3, ULong3,  ULONG, SCALAR);
medImplementItkDataImageReader (float,          3, Float3,  FLOAT, SCALAR);
medImplementItkDataImageReader (double,         3, Double3, DOUBLE, SCALAR);
medImplementItkDataImageReader (UCharVectorType, 3, Vector3, UCHAR, VECTOR);
medImplementItkDataImageReader (RGBPixelType, 3, RGB3, UCHAR, RGB);

