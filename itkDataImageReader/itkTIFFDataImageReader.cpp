#include "itkTIFFDataImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkTIFFImageIO.h>


itkTIFFDataImageReader::itkTIFFDataImageReader(void) : itkDataImageReaderBase()
{
    this->io = itk::TIFFImageIO::New();
}


itkTIFFDataImageReader::~itkTIFFDataImageReader(void)
{
}

QStringList itkTIFFDataImageReader::handled(void) const
{
    return s_handled();
}

QStringList itkTIFFDataImageReader::s_handled(void)
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

bool itkTIFFDataImageReader::registered(void)
{
    return dtkAbstractDataFactory::instance()->registerDataReaderType("itkTIFFDataImageReader", s_handled(),
                                                                      createItkTIFFDataImageReader);
}


QString itkTIFFDataImageReader::description(void) const
{
    return "itkTIFFDataImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkTIFFDataImageReader(void)
{
    return new itkTIFFDataImageReader;
}

