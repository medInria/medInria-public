#include "itkDataImageReader.h"

#include "medItk.h"

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkLog.h>

#include <itkImage.h>
#include <itkImageFileReader.h>

#include "medItkDataImageReaderMacros.h"

medImplementItkDataImageReader (char,           3, Char3,   CHAR);
medImplementItkDataImageReader (unsigned char,  3, UChar3,  UCHAR);
medImplementItkDataImageReader (short,          3, Short3,  SHORT);
medImplementItkDataImageReader (unsigned short, 3, UShort3, USHORT);
medImplementItkDataImageReader (int,            3, Int3,    INT);
medImplementItkDataImageReader (unsigned int,   3, UInt3,   UINT);
medImplementItkDataImageReader (long,           3, Long3,   LONG);
medImplementItkDataImageReader (unsigned,       3, ULong3,  ULONG);
medImplementItkDataImageReader (float,          3, Float3,  FLOAT);
medImplementItkDataImageReader (double,         3, Double3, DOUBLE);

