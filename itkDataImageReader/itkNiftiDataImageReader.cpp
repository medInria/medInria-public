#include "itkNiftiDataImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkNiftiImageIO.h>


itkNiftiDataImageReader::itkNiftiDataImageReader(void) : itkDataImageReaderBase()
{
    this->io = itk::NiftiImageIO::New();
}


itkNiftiDataImageReader::~itkNiftiDataImageReader(void)
{
}

QStringList itkNiftiDataImageReader::handled(void) const
{
    return s_handled();
}

QStringList itkNiftiDataImageReader::s_handled(void)
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

bool itkNiftiDataImageReader::registered(void)
{
    return dtkAbstractDataFactory::instance()->registerDataReaderType("itkNiftiDataImageReader", s_handled(),
                                                                      createItkNiftiDataImageReader);
}


QString itkNiftiDataImageReader::description(void) const
{
    return "itkNiftiDataImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkNiftiDataImageReader(void)
{
    return new itkNiftiDataImageReader;
}

