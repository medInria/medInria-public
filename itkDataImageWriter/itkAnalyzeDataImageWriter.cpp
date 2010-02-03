#include "medItk.h"

#include "itkAnalyzeDataImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkAnalyzeImageIO.h>


itkAnalyzeDataImageWriter::itkAnalyzeDataImageWriter(void) : itkDataImageWriterBase()
{
    this->io = itk::AnalyzeImageIO::New();
}


itkAnalyzeDataImageWriter::~itkAnalyzeDataImageWriter(void)
{
}


bool itkAnalyzeDataImageWriter::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataWriterType("itkAnalyzeDataImageWriter", itkDataImageWriterBase::s_handled(),
								    createItkAnalyzeDataImageWriter);
}


QString itkAnalyzeDataImageWriter::description(void) const
{
    return "itkAnalyzeDataImageWriter";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkAnalyzeDataImageWriter(void)
{
    return new itkAnalyzeDataImageWriter;
}

