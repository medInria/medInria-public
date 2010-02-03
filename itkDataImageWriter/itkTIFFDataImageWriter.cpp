#include "medItk.h"

#include "itkTIFFDataImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkTIFFImageIO.h>


itkTIFFDataImageWriter::itkTIFFDataImageWriter(void) : itkDataImageWriterBase()
{
    this->io = itk::TIFFImageIO::New();
}


itkTIFFDataImageWriter::~itkTIFFDataImageWriter(void)
{
}


bool itkTIFFDataImageWriter::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataWriterType("itkTIFFDataImageWriter", itkDataImageWriterBase::s_handled(),
								    createItkTIFFDataImageWriter);
}


QString itkTIFFDataImageWriter::description(void) const
{
    return "itkTIFFDataImageWriter";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkTIFFDataImageWriter(void)
{
    return new itkTIFFDataImageWriter;
}

