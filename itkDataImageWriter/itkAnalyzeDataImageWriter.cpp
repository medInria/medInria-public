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

QStringList itkAnalyzeDataImageWriter::handled(void) const
{
  return s_handled();
}

QStringList itkAnalyzeDataImageWriter::s_handled(void)
{
  return QStringList()  << "itkDataImageChar3" << "itkDataImageChar4"
						<< "itkDataImageUChar3" << "itkDataImageUChar4"
						<< "itkDataImageShort3" << "itkDataImageShort4"
						<< "itkDataImageUShort3" << "itkDataImageUShort4"
						<< "itkDataImageInt3" << "itkDataImageInt4"
						<< "itkDataImageUInt3" << "itkDataImageUInt4"
						<< "itkDataImageLong3" << "itkDataImageLong4"
						<< "itkDataImageULong3" << "itkDataImageULong4"
						<< "itkDataImageFloat3" << "itkDataImageFloat4"
						<< "itkDataImageDouble3" << "itkDataImageDouble4"
						<< "itkDataImageRGB3";
}

bool itkAnalyzeDataImageWriter::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataWriterType("itkAnalyzeDataImageWriter", s_handled(),
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

