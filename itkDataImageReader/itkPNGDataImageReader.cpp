#include "medItk.h"

#include "itkPNGDataImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkPNGImageIO.h>


itkPNGDataImageReader::itkPNGDataImageReader(void) : itkDataImageReaderBase()
{
    this->io = itk::PNGImageIO::New();
}


itkPNGDataImageReader::~itkPNGDataImageReader(void)
{
}


bool itkPNGDataImageReader::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataReaderType("itkPNGDataImageReader", itkDataImageReaderBase::s_handled(),
								    createItkPNGDataImageReader);
}


QString itkPNGDataImageReader::description(void) const
{
    return "itkPNGDataImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkPNGDataImageReader(void)
{
    return new itkPNGDataImageReader;
}

