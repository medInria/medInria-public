#include "medItk.h"

#include "itkTIFFDataImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkTIFFImageIO.h>


itkTIFFDataImageReader::itkTIFFDataImageReader(void) : itkDataImageReaderBase()
{
    this->io = itk::TIFFImageIO::New();
}


itkTIFFDataImageReader::~itkTIFFDataImageReader(void)
{
}


bool itkTIFFDataImageReader::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataReaderType("itkTIFFDataImageReader", itkDataImageReaderBase::s_handled(),
								    createItkTIFFDataImageReader);
}


QString itkTIFFDataImageReader::description(void) const
{
    return "itkTIFFDataImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkTIFFDataImageReader(void)
{
    return new itkTIFFDataImageReader;
}

