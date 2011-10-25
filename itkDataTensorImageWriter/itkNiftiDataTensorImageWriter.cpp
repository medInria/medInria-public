#include "itkNiftiDataTensorImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkNiftiImageIO.h>


const char itkNiftiDataTensorImageWriter::ID[] = "itkNiftiDataTensorImageWriter";

itkNiftiDataTensorImageWriter::itkNiftiDataTensorImageWriter(void) : itkDataTensorImageWriterBase()
{
    this->io = itk::NiftiImageIO::New();
}


itkNiftiDataTensorImageWriter::~itkNiftiDataTensorImageWriter(void)
{
}


bool itkNiftiDataTensorImageWriter::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataWriterType(
              "itkNiftiDataTensorImageWriter",
              itkDataTensorImageWriterBase::s_handled(),
              createitkNiftiDataTensorImageWriter);
}

QString itkNiftiDataTensorImageWriter::description( void ) const
{
    return tr( "Nifti writer for Tensor images" );
}

QString itkNiftiDataTensorImageWriter::identifier( void ) const
{
    return ID;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createitkNiftiDataTensorImageWriter(void)
{
    return new itkNiftiDataTensorImageWriter;
}

