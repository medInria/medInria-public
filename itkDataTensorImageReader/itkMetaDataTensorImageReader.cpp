#include "itkMetaDataTensorImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkMetaImageIO.h>


itkMetaDataTensorImageReader::itkMetaDataTensorImageReader(void) : itkDataTensorImageReaderBase()
{
    this->io = itk::MetaImageIO::New();
}


itkMetaDataTensorImageReader::~itkMetaDataTensorImageReader(void)
{
}


bool itkMetaDataTensorImageReader::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataReaderType("itkMetaDataTensorImageReader", itkDataTensorImageReaderBase::s_handled(),
								    createItkMetaDataTensorImageReader);
}


QString itkMetaDataTensorImageReader::description(void) const
{
    return "itkMetaDataTensorImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkMetaDataTensorImageReader(void)
{
    return new itkMetaDataTensorImageReader;
}

