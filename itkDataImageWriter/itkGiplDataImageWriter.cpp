#include "medItk/medItk.h"

#include "itkGiplDataImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkGiplImageIO.h>


itkGiplDataImageWriter::itkGiplDataImageWriter(void) : itkDataImageWriterBase()
{
    this->io = itk::GiplImageIO::New();
}


itkGiplDataImageWriter::~itkGiplDataImageWriter(void)
{
}


bool itkGiplDataImageWriter::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataWriterType("itkGiplDataImageWriter", itkDataImageWriterBase::s_handled(),
								    createItkGiplDataImageWriter);
}


QString itkGiplDataImageWriter::description(void) const
{
    return "itkGiplDataImageWriter";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkGiplDataImageWriter(void)
{
    return new itkGiplDataImageWriter;
}

