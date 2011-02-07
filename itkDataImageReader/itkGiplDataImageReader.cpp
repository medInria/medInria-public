#include "itkGiplDataImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkGiplImageIO.h>


itkGiplDataImageReader::itkGiplDataImageReader(void) : itkDataImageReaderBase()
{
    this->io = itk::GiplImageIO::New();
}


itkGiplDataImageReader::~itkGiplDataImageReader(void)
{
}

QStringList itkGiplDataImageReader::handled(void) const
{
    return s_handled();
}

QStringList itkGiplDataImageReader::s_handled(void)
{
    return QStringList()  << "itkDataImageChar3" << "itkDataImageChar4"
            << "itkDataImageUChar3" << "itkDataImageUChar4"
            << "itkDataImageShort3" << "itkDataImageShort4"
            << "itkDataImageUShort3" << "itkDataImageUShort4"
            << "itkDataImageInt3" << "itkDataImageInt4"
            << "itkDataImageUInt3" << "itkDataImageUInt4"
            << "itkDataImageLong3" << "itkDataImageLong4"
            << "itkDataImageULong3" << "itkDataImageULong4"
            << "itkDataImageFloat3" << "itkDataImageFloat4"
            << "itkDataImageDouble3" << "itkDataImageDouble4"
            << "itkDataImageRGB3";
}

bool itkGiplDataImageReader::registered(void)
{
    return dtkAbstractDataFactory::instance()->registerDataReaderType("itkGiplDataImageReader", s_handled(),
                                                                      createItkGiplDataImageReader);
}


QString itkGiplDataImageReader::description(void) const
{
    return "itkGiplDataImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkGiplDataImageReader(void)
{
    return new itkGiplDataImageReader;
}

