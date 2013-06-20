#include "itkMetaDataShImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkMetaImageIO.h>

itkMetaDataShImageReader::itkMetaDataShImageReader(void) : itkDataShImageReaderBase()
{
    this->io = itk::MetaImageIO::New();
}

itkMetaDataShImageReader::~itkMetaDataShImageReader(void)
{
}

bool itkMetaDataShImageReader::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataReaderType("itkMetaDataShImageReader", itkDataShImageReaderBase::s_handled(),
                                                                    createItkMetaDataShImageReader);
}

QString itkMetaDataShImageReader::description(void) const
{
    return tr("itk SH 3d image reader");
}

QString itkMetaDataShImageReader::identifier() const
{
    return "itkMetaDataShImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkMetaDataShImageReader(void)
{
    return new itkMetaDataShImageReader;
}

