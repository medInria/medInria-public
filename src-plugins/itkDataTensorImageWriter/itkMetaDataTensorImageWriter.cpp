#include "itkMetaDataTensorImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkMetaImageIO.h>


const char itkMetaDataTensorImageWriter::ID[] = "itkMetaDataTensorImageWriter";

itkMetaDataTensorImageWriter::itkMetaDataTensorImageWriter() : itkDataTensorImageWriterBase()
{
    this->io = itk::MetaImageIO::New();
}


itkMetaDataTensorImageWriter::~itkMetaDataTensorImageWriter()
{
}


bool itkMetaDataTensorImageWriter::registered()
{
  return dtkAbstractDataFactory::instance()->registerDataWriterType(
              "itkMetaDataTensorImageWriter",
              itkDataTensorImageWriterBase::s_handled(),
              createItkMetaDataTensorImageWriter);
}

QString itkMetaDataTensorImageWriter::description() const
{
    return tr( "MetaData writer for Tensor images" );
}

QString itkMetaDataTensorImageWriter::identifier() const
{
    return ID;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkMetaDataTensorImageWriter()
{
    return new itkMetaDataTensorImageWriter;
}

