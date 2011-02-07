#include "itkBMPDataImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkBMPImageIO.h>


itkBMPDataImageReader::itkBMPDataImageReader(void) : itkDataImageReaderBase()
{
    this->io = itk::BMPImageIO::New();
}


itkBMPDataImageReader::~itkBMPDataImageReader(void)
{
}

QStringList itkBMPDataImageReader::handled(void) const
{
    return s_handled();
}

QStringList itkBMPDataImageReader::s_handled(void)
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

bool itkBMPDataImageReader::registered(void)
{
    return dtkAbstractDataFactory::instance()->registerDataReaderType("itkBMPDataImageReader", s_handled(),
                                                                      createItkBMPDataImageReader);
}


QString itkBMPDataImageReader::description(void) const
{
    return "itkBMPDataImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkBMPDataImageReader(void)
{
    return new itkBMPDataImageReader;
}

