#include "itkPNGDataImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkPNGImageIO.h>


itkPNGDataImageWriter::itkPNGDataImageWriter(void) : itkDataImageWriterBase()
{
    this->io = itk::PNGImageIO::New();
}


itkPNGDataImageWriter::~itkPNGDataImageWriter(void)
{
}


bool itkPNGDataImageWriter::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataWriterType("itkPNGDataImageWriter", itkDataImageWriterBase::s_handled(),
								    createItkPNGDataImageWriter);
}


QString itkPNGDataImageWriter::description(void) const
{
    return "itkPNGDataImageWriter";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkPNGDataImageWriter(void)
{
    return new itkPNGDataImageWriter;
}

