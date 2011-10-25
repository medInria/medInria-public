#include "itkNrrdDataTensorImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkNrrdImageIO.h>


const char itkNrrdDataTensorImageWriter::ID[] = "itkNrrdDataTensorImageWriter";

itkNrrdDataTensorImageWriter::itkNrrdDataTensorImageWriter(void) : itkDataTensorImageWriterBase()
{
    this->io = itk::NrrdImageIO::New();
}


itkNrrdDataTensorImageWriter::~itkNrrdDataTensorImageWriter(void)
{
}


bool itkNrrdDataTensorImageWriter::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataWriterType(
              "itkNrrdDataTensorImageWriter",
              itkDataTensorImageWriterBase::s_handled(),
              createitkNrrdDataTensorImageWriter);
}

QString itkNrrdDataTensorImageWriter::description( void ) const
{
    return tr( "Nrrd writer for Tensor images" );
}

QString itkNrrdDataTensorImageWriter::identifier( void ) const
{
    return ID;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createitkNrrdDataTensorImageWriter(void)
{
    return new itkNrrdDataTensorImageWriter;
}

