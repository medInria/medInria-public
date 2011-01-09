#include "itkNiftiDataTensorImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkNiftiImageIO.h>


itkNiftiDataTensorImageReader::itkNiftiDataTensorImageReader(void) : itkDataTensorImageReaderBase()
{
    this->io = itk::NiftiImageIO::New();
}


itkNiftiDataTensorImageReader::~itkNiftiDataTensorImageReader(void)
{
}


bool itkNiftiDataTensorImageReader::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataReaderType("itkNiftiDataTensorImageReader", itkDataTensorImageReaderBase::s_handled(),
								    createItkNiftiDataTensorImageReader);
}


QString itkNiftiDataTensorImageReader::description(void) const
{
    return "itkNiftiDataTensorImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkNiftiDataTensorImageReader(void)
{
    return new itkNiftiDataTensorImageReader;
}

