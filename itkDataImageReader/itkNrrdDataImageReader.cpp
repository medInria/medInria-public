#include "medItk.h"

#include "itkNrrdDataImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkNrrdImageIO.h>


itkNrrdDataImageReader::itkNrrdDataImageReader(void) : itkDataImageReaderBase()
{
    this->io = itk::NrrdImageIO::New();
}


itkNrrdDataImageReader::~itkNrrdDataImageReader(void)
{
}


bool itkNrrdDataImageReader::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataReaderType("itkNrrdDataImageReader", itkDataImageReaderBase::s_handled(),
								    createItkNrrdDataImageReader);
}


QString itkNrrdDataImageReader::description(void) const
{
    return "itkNrrdDataImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkNrrdDataImageReader(void)
{
    return new itkNrrdDataImageReader;
}

