#include "medItk.h"

#include "itkBMPDataImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkBMPImageIO.h>


itkBMPDataImageWriter::itkBMPDataImageWriter(void) : itkDataImageWriterBase()
{
    this->io = itk::BMPImageIO::New();
}


itkBMPDataImageWriter::~itkBMPDataImageWriter(void)
{
}


bool itkBMPDataImageWriter::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataWriterType("itkBMPDataImageWriter", itkDataImageWriterBase::s_handled(),
								    createItkBMPDataImageWriter);
}


QString itkBMPDataImageWriter::description(void) const
{
    return "itkBMPDataImageWriter";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkBMPDataImageWriter(void)
{
    return new itkBMPDataImageWriter;
}

