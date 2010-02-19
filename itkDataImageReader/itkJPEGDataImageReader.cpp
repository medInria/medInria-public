#include "medItk/medItk.h"

#include "itkJPEGDataImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkJPEGImageIO.h>


itkJPEGDataImageReader::itkJPEGDataImageReader(void) : itkDataImageReaderBase()
{
    this->io = itk::JPEGImageIO::New();
}


itkJPEGDataImageReader::~itkJPEGDataImageReader(void)
{
}


bool itkJPEGDataImageReader::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataReaderType("itkJPEGDataImageReader", itkDataImageReaderBase::s_handled(),
								    createItkJPEGDataImageReader);
}


QString itkJPEGDataImageReader::description(void) const
{
    return "itkJPEGDataImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkJPEGDataImageReader(void)
{
    return new itkJPEGDataImageReader;
}

