#include "itkPNGDataImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkPNGImageIO.h>


itkPNGDataImageReader::itkPNGDataImageReader(void) : itkDataImageReaderBase()
{
    this->io = itk::PNGImageIO::New();
}


itkPNGDataImageReader::~itkPNGDataImageReader(void)
{
}

QStringList itkPNGDataImageReader::handled(void) const
{
    return s_handled();
}

QStringList itkPNGDataImageReader::s_handled(void)
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

bool itkPNGDataImageReader::registered(void)
{
    return dtkAbstractDataFactory::instance()->registerDataReaderType("itkPNGDataImageReader", s_handled(),
                                                                      createItkPNGDataImageReader);
}


QString itkPNGDataImageReader::description(void) const
{
    return "itkPNGDataImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkPNGDataImageReader(void)
{
    return new itkPNGDataImageReader;
}

