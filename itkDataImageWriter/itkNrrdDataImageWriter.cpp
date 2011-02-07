#include "itkNrrdDataImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkNrrdImageIO.h>


itkNrrdDataImageWriter::itkNrrdDataImageWriter(void) : itkDataImageWriterBase()
{
    this->io = itk::NrrdImageIO::New();
}


itkNrrdDataImageWriter::~itkNrrdDataImageWriter(void)
{
}

QStringList itkNrrdDataImageWriter::handled(void) const
{
    return s_handled();
}

QStringList itkNrrdDataImageWriter::s_handled(void)
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

bool itkNrrdDataImageWriter::registered(void)
{
    return dtkAbstractDataFactory::instance()->registerDataWriterType("itkNrrdDataImageWriter", s_handled(),
                                                                      createItkNrrdDataImageWriter);
}


QString itkNrrdDataImageWriter::description(void) const
{
    return "itkNrrdDataImageWriter";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkNrrdDataImageWriter(void)
{
    return new itkNrrdDataImageWriter;
}

