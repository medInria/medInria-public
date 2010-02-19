#include "medItk/medItk.h"

#include "itkVTKDataImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkVTKImageIO.h>


itkVTKDataImageWriter::itkVTKDataImageWriter(void) : itkDataImageWriterBase()
{
    this->io = itk::VTKImageIO::New();
}


itkVTKDataImageWriter::~itkVTKDataImageWriter(void)
{
}


bool itkVTKDataImageWriter::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataWriterType("itkVTKDataImageWriter", itkDataImageWriterBase::s_handled(),
								    createItkVTKDataImageWriter);
}


QString itkVTKDataImageWriter::description(void) const
{
    return "itkVTKDataImageWriter";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkVTKDataImageWriter(void)
{
    return new itkVTKDataImageWriter;
}

