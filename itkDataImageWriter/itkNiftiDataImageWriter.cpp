#include "itkNiftiDataImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkNiftiImageIO.h>


itkNiftiDataImageWriter::itkNiftiDataImageWriter(void) : itkDataImageWriterBase()
{
    this->io = itk::NiftiImageIO::New();
}


itkNiftiDataImageWriter::~itkNiftiDataImageWriter(void)
{
}

QStringList itkNiftiDataImageWriter::handled(void) const
{
    return s_handled();
}

QStringList itkNiftiDataImageWriter::s_handled(void)
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

bool itkNiftiDataImageWriter::registered(void)
{
    return dtkAbstractDataFactory::instance()->registerDataWriterType("itkNiftiDataImageWriter", s_handled(),
                                                                      createItkNiftiDataImageWriter);
}


QString itkNiftiDataImageWriter::description(void) const
{
    return "itkNiftiDataImageWriter";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkNiftiDataImageWriter(void)
{
    return new itkNiftiDataImageWriter;
}

