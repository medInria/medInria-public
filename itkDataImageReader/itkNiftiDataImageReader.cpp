#include "medItk.h"

#include "itkNiftiDataImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkNiftiImageIO.h>


itkNiftiDataImageReader::itkNiftiDataImageReader(void) : itkDataImageReaderBase()
{
    this->io = itk::NiftiImageIO::New();
}


itkNiftiDataImageReader::~itkNiftiDataImageReader(void)
{
}


bool itkNiftiDataImageReader::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataReaderType("itkNiftiDataImageReader", itkDataImageReaderBase::s_handled(),
								    createItkNiftiDataImageReader);
}


QString itkNiftiDataImageReader::description(void) const
{
    return "itkNiftiDataImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkNiftiDataImageReader(void)
{
    return new itkNiftiDataImageReader;
}

