#include "itkMetaDataSHImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkMetaImageIO.h>

itkMetaDataSHImageReader::itkMetaDataSHImageReader(void) : itkDataSHImageReaderBase()
{
    this->io = itk::MetaImageIO::New();
}

itkMetaDataSHImageReader::~itkMetaDataSHImageReader(void)
{
}

bool itkMetaDataSHImageReader::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataReaderType("itkMetaDataSHImageReader", itkDataSHImageReaderBase::s_handled(),
                                                                    createItkMetaDataSHImageReader);
}

QString itkMetaDataSHImageReader::description(void) const
{
    return tr("itk SH 3d image reader");
}

QString itkMetaDataSHImageReader::identifier() const
{
    return "itkMetaDataSHImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkMetaDataSHImageReader(void)
{
    return new itkMetaDataSHImageReader;
}

