#include "medItk.h"

#include "itkAnalyzeDataImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkAnalyzeImageIO.h>


itkAnalyzeDataImageReader::itkAnalyzeDataImageReader(void) : itkDataImageReaderBase()
{
    this->io = itk::AnalyzeImageIO::New();
}


itkAnalyzeDataImageReader::~itkAnalyzeDataImageReader(void)
{
}


bool itkAnalyzeDataImageReader::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataReaderType("itkAnalyzeDataImageReader", itkDataImageReaderBase::s_handled(),
								    createItkAnalyzeDataImageReader);
}


QString itkAnalyzeDataImageReader::description(void) const
{
    return "itkAnalyzeDataImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkAnalyzeDataImageReader(void)
{
    return new itkAnalyzeDataImageReader;
}

