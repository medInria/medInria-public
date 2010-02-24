#include "itkBMPDataImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkBMPImageIO.h>


itkBMPDataImageReader::itkBMPDataImageReader(void) : itkDataImageReaderBase()
{
    this->io = itk::BMPImageIO::New();
}


itkBMPDataImageReader::~itkBMPDataImageReader(void)
{
}


bool itkBMPDataImageReader::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataReaderType("itkBMPDataImageReader", itkDataImageReaderBase::s_handled(),
								    createItkBMPDataImageReader);
}


QString itkBMPDataImageReader::description(void) const
{
    return "itkBMPDataImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkBMPDataImageReader(void)
{
    return new itkBMPDataImageReader;
}

