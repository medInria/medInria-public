#include "itkGiplDataImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkGiplImageIO.h>


itkGiplDataImageWriter::itkGiplDataImageWriter(void) : itkDataImageWriterBase()
{
    this->io = itk::GiplImageIO::New();
}


itkGiplDataImageWriter::~itkGiplDataImageWriter(void)
{
}

QStringList itkGiplDataImageWriter::handled(void) const
{
    return s_handled();
}

QStringList itkGiplDataImageWriter::s_handled(void)
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

bool itkGiplDataImageWriter::registered(void)
{
    return dtkAbstractDataFactory::instance()->registerDataWriterType("itkGiplDataImageWriter", s_handled(),
                                                                      createItkGiplDataImageWriter);
}


QString itkGiplDataImageWriter::description(void) const
{
    return "itkGiplDataImageWriter";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkGiplDataImageWriter(void)
{
    return new itkGiplDataImageWriter;
}

