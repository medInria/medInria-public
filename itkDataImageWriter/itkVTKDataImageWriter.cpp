#include "itkVTKDataImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkVTKImageIO.h>


itkVTKDataImageWriter::itkVTKDataImageWriter(void) : itkDataImageWriterBase()
{
    this->io = itk::VTKImageIO::New();
}


itkVTKDataImageWriter::~itkVTKDataImageWriter(void)
{
}

QStringList itkVTKDataImageWriter::handled(void) const
{
    return s_handled();
}

QStringList itkVTKDataImageWriter::s_handled(void)
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

bool itkVTKDataImageWriter::registered(void)
{
    return dtkAbstractDataFactory::instance()->registerDataWriterType("itkVTKDataImageWriter", s_handled(),
                                                                      createItkVTKDataImageWriter);
}


QString itkVTKDataImageWriter::description(void) const
{
    return "itkVTKDataImageWriter";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkVTKDataImageWriter(void)
{
    return new itkVTKDataImageWriter;
}

