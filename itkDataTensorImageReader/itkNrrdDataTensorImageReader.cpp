#include "itkNrrdDataTensorImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkNrrdImageIO.h>


itkNrrdDataTensorImageReader::itkNrrdDataTensorImageReader(void) : itkDataTensorImageReaderBase()
{
    this->io = itk::NrrdImageIO::New();
}


itkNrrdDataTensorImageReader::~itkNrrdDataTensorImageReader(void)
{
}


bool itkNrrdDataTensorImageReader::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataReaderType("itkNrrdDataTensorImageReader", itkDataTensorImageReaderBase::s_handled(),
								    createItkNrrdDataTensorImageReader);
}


QString itkNrrdDataTensorImageReader::description(void) const
{
    return "itkNrrdDataTensorImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkNrrdDataTensorImageReader(void)
{
    return new itkNrrdDataTensorImageReader;
}

