#include "itkTIFFDataImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkTIFFImageIO.h>


itkTIFFDataImageWriter::itkTIFFDataImageWriter(void) : itkDataImageWriterBase()
{
    this->io = itk::TIFFImageIO::New();
}


itkTIFFDataImageWriter::~itkTIFFDataImageWriter(void)
{
}

QStringList itkTIFFDataImageWriter::handled(void) const
{
    return s_handled();
}

QStringList itkTIFFDataImageWriter::s_handled(void)
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

bool itkTIFFDataImageWriter::registered(void)
{
    return dtkAbstractDataFactory::instance()->registerDataWriterType("itkTIFFDataImageWriter", s_handled(),
                                                                      createItkTIFFDataImageWriter);
}


QString itkTIFFDataImageWriter::description(void) const
{
    return "itkTIFFDataImageWriter";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkTIFFDataImageWriter(void)
{
    return new itkTIFFDataImageWriter;
}

