#include "itkMetaDataTensorImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkMetaImageIO.h>


itkMetaDataTensorImageWriter::itkMetaDataTensorImageWriter(void) : itkDataTensorImageWriterBase()
{
    this->io = itk::MetaImageIO::New();
}


itkMetaDataTensorImageWriter::~itkMetaDataTensorImageWriter(void)
{
}


bool itkMetaDataTensorImageWriter::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataWriterType("itkMetaDataTensorImageWriter", itkDataTensorImageWriterBase::s_handled(),
								    createItkMetaDataTensorImageWriter);
}


QString itkMetaDataTensorImageWriter::description(void) const
{
    return "itkMetaDataTensorImageWriter";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkMetaDataTensorImageWriter(void)
{
    return new itkMetaDataTensorImageWriter;
}

