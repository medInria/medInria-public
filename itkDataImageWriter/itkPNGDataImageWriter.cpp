#include "itkPNGDataImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkPNGImageIO.h>


itkPNGDataImageWriter::itkPNGDataImageWriter(void) : itkDataImageWriterBase()
{
    this->io = itk::PNGImageIO::New();
}


itkPNGDataImageWriter::~itkPNGDataImageWriter(void)
{
}

QStringList itkPNGDataImageWriter::handled(void) const
{
    return s_handled();
}

QStringList itkPNGDataImageWriter::s_handled(void)
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

bool itkPNGDataImageWriter::registered(void)
{
    return dtkAbstractDataFactory::instance()->registerDataWriterType("itkPNGDataImageWriter", s_handled(),
                                                                      createItkPNGDataImageWriter);
}


QString itkPNGDataImageWriter::description(void) const
{
    return "itkPNGDataImageWriter";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkPNGDataImageWriter(void)
{
    return new itkPNGDataImageWriter;
}

