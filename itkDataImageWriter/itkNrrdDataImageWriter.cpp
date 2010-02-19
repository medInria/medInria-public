#include "medItk/medItk.h"

#include "itkNrrdDataImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkNrrdImageIO.h>


itkNrrdDataImageWriter::itkNrrdDataImageWriter(void) : itkDataImageWriterBase()
{
    this->io = itk::NrrdImageIO::New();
}


itkNrrdDataImageWriter::~itkNrrdDataImageWriter(void)
{
}


bool itkNrrdDataImageWriter::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataWriterType("itkNrrdDataImageWriter", itkDataImageWriterBase::s_handled(),
								    createItkNrrdDataImageWriter);
}


QString itkNrrdDataImageWriter::description(void) const
{
    return "itkNrrdDataImageWriter";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkNrrdDataImageWriter(void)
{
    return new itkNrrdDataImageWriter;
}

