#include "itkJPEGDataImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkJPEGImageIO.h>


itkJPEGDataImageReader::itkJPEGDataImageReader(void) : itkDataImageReaderBase()
{
    this->io = itk::JPEGImageIO::New();
}


itkJPEGDataImageReader::~itkJPEGDataImageReader(void)
{
}

QStringList itkJPEGDataImageReader::handled(void) const
{
    return s_handled();
}

QStringList itkJPEGDataImageReader::s_handled(void)
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

bool itkJPEGDataImageReader::registered(void)
{
    return dtkAbstractDataFactory::instance()->registerDataReaderType("itkJPEGDataImageReader", s_handled(),
                                                                      createItkJPEGDataImageReader);
}


QString itkJPEGDataImageReader::description(void) const
{
    return "itkJPEGDataImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkJPEGDataImageReader(void)
{
    return new itkJPEGDataImageReader;
}

