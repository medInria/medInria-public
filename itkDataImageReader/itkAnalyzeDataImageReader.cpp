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

QStringList itkAnalyzeDataImageReader::handled(void) const
{
    return s_handled();
}

QStringList itkAnalyzeDataImageReader::s_handled(void)
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

bool itkAnalyzeDataImageReader::registered(void)
{
    return dtkAbstractDataFactory::instance()->registerDataReaderType("itkAnalyzeDataImageReader", s_handled(),
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

