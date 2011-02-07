#include "itkVTKDataImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkVTKImageIO.h>


itkVTKDataImageReader::itkVTKDataImageReader(void) : itkDataImageReaderBase()
{
    this->io = itk::VTKImageIO::New();
}


itkVTKDataImageReader::~itkVTKDataImageReader(void)
{
}

QStringList itkVTKDataImageReader::handled(void) const
{
    return s_handled();
}

QStringList itkVTKDataImageReader::s_handled(void)
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

bool itkVTKDataImageReader::registered(void)
{
    return dtkAbstractDataFactory::instance()->registerDataReaderType("itkVTKDataImageReader", s_handled(),
                                                                      createItkVTKDataImageReader);
}


QString itkVTKDataImageReader::description(void) const
{
    return "itkVTKDataImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkVTKDataImageReader(void)
{
    return new itkVTKDataImageReader;
}

