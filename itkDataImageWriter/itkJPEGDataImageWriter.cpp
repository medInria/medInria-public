#include "itkJPEGDataImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkJPEGImageIO.h>


itkJPEGDataImageWriter::itkJPEGDataImageWriter(void) : itkDataImageWriterBase()
{
    this->io = itk::JPEGImageIO::New();
}


itkJPEGDataImageWriter::~itkJPEGDataImageWriter(void)
{
}

QStringList itkJPEGDataImageWriter::handled(void) const
{
    return s_handled();
}

QStringList itkJPEGDataImageWriter::s_handled(void)
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

bool itkJPEGDataImageWriter::registered(void)
{
    return dtkAbstractDataFactory::instance()->registerDataWriterType("itkJPEGDataImageWriter", s_handled(),
                                                                      createItkJPEGDataImageWriter);
}


QString itkJPEGDataImageWriter::description(void) const
{
    return "itkJPEGDataImageWriter";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkJPEGDataImageWriter(void)
{
    return new itkJPEGDataImageWriter;
}

