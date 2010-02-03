#include "medItk.h"

#include "itkGiplDataImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkGiplImageIO.h>


itkGiplDataImageReader::itkGiplDataImageReader(void) : itkDataImageReaderBase()
{
    this->io = itk::GiplImageIO::New();
}


itkGiplDataImageReader::~itkGiplDataImageReader(void)
{
}


bool itkGiplDataImageReader::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataReaderType("itkGiplDataImageReader", itkDataImageReaderBase::s_handled(),
								    createItkGiplDataImageReader);
}


QString itkGiplDataImageReader::description(void) const
{
    return "itkGiplDataImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkGiplDataImageReader(void)
{
    return new itkGiplDataImageReader;
}

