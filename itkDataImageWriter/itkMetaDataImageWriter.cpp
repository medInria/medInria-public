#include "medItk/medItk.h"

#include "itkMetaDataImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkMetaImageIO.h>


itkMetaDataImageWriter::itkMetaDataImageWriter(void) : itkDataImageWriterBase()
{
    this->io = itk::MetaImageIO::New();
}


itkMetaDataImageWriter::~itkMetaDataImageWriter(void)
{
}


bool itkMetaDataImageWriter::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataWriterType("itkMetaDataImageWriter", itkDataImageWriterBase::s_handled(),
								    createItkMetaDataImageWriter);
}


QString itkMetaDataImageWriter::description(void) const
{
    return "itkMetaDataImageWriter";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkMetaDataImageWriter(void)
{
    return new itkMetaDataImageWriter;
}

