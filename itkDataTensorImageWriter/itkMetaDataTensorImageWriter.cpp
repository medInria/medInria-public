#include "itkMetaDataTensorImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkMetaImageIO.h>


const char itkMetaDataTensorImageWriter::ID[] = "itkMetaDataTensorImageWriter";

itkMetaDataTensorImageWriter::itkMetaDataTensorImageWriter(void) : itkDataTensorImageWriterBase()
{
    this->io = itk::MetaImageIO::New();
}


itkMetaDataTensorImageWriter::~itkMetaDataTensorImageWriter(void)
{
}


bool itkMetaDataTensorImageWriter::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataWriterType(
              "itkMetaDataTensorImageWriter",
              itkDataTensorImageWriterBase::s_handled(),
              createItkMetaDataTensorImageWriter);
}

QString itkMetaDataTensorImageWriter::description( void ) const
{
    return tr( "MetaData writer for Tensor images" );
}

QString itkMetaDataTensorImageWriter::identifier( void ) const
{
    return ID;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkMetaDataTensorImageWriter(void)
{
    return new itkMetaDataTensorImageWriter;
}

