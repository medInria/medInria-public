#include "itkNiftiDataImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkNiftiImageIO.h>


itkNiftiDataImageWriter::itkNiftiDataImageWriter(void) : itkDataImageWriterBase()
{
    this->io = itk::NiftiImageIO::New();
}


itkNiftiDataImageWriter::~itkNiftiDataImageWriter(void)
{
}


bool itkNiftiDataImageWriter::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataWriterType("itkNiftiDataImageWriter", itkDataImageWriterBase::s_handled(),
								    createItkNiftiDataImageWriter);
}


QString itkNiftiDataImageWriter::description(void) const
{
    return "itkNiftiDataImageWriter";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkNiftiDataImageWriter(void)
{
    return new itkNiftiDataImageWriter;
}

