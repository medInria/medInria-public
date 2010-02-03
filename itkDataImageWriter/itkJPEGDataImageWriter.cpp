#include "medItk.h"

#include "itkJPEGDataImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkJPEGImageIO.h>


itkJPEGDataImageWriter::itkJPEGDataImageWriter(void) : itkDataImageWriterBase()
{
    this->io = itk::JPEGImageIO::New();
}


itkJPEGDataImageWriter::~itkJPEGDataImageWriter(void)
{
}


bool itkJPEGDataImageWriter::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataWriterType("itkJPEGDataImageWriter", itkDataImageWriterBase::s_handled(),
								    createItkJPEGDataImageWriter);
}


QString itkJPEGDataImageWriter::description(void) const
{
    return "itkJPEGDataImageWriter";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkJPEGDataImageWriter(void)
{
    return new itkJPEGDataImageWriter;
}

