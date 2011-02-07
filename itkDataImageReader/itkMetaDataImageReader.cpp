#include "itkMetaDataImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkMetaImageIO.h>

itkMetaDataImageReader::itkMetaDataImageReader(void) : itkDataImageReaderBase()
{
    this->io = itk::MetaImageIO::New();
}

itkMetaDataImageReader::~itkMetaDataImageReader(void)
{

}

QStringList itkMetaDataImageReader::handled(void) const
{
    return s_handled();
}

QStringList itkMetaDataImageReader::s_handled(void)
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

bool itkMetaDataImageReader::registered(void)
{
    return dtkAbstractDataFactory::instance()->registerDataReaderType("itkMetaDataImageReader", s_handled(), createItkMetaDataImageReader);
}

QString itkMetaDataImageReader::description(void) const
{
    return "itkMetaDataImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkMetaDataImageReader(void)
{
    return new itkMetaDataImageReader;
}
