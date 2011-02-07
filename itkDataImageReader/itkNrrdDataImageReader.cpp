#include "itkNrrdDataImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkNrrdImageIO.h>


itkNrrdDataImageReader::itkNrrdDataImageReader(void) : itkDataImageReaderBase()
{
    this->io = itk::NrrdImageIO::New();
}


itkNrrdDataImageReader::~itkNrrdDataImageReader(void)
{
}

QStringList itkNrrdDataImageReader::handled(void) const
{
    return s_handled();
}

QStringList itkNrrdDataImageReader::s_handled(void)
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

bool itkNrrdDataImageReader::registered(void)
{
    return dtkAbstractDataFactory::instance()->registerDataReaderType("itkNrrdDataImageReader", s_handled(),
                                                                      createItkNrrdDataImageReader);
}


QString itkNrrdDataImageReader::description(void) const
{
    return "itkNrrdDataImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkNrrdDataImageReader(void)
{
    return new itkNrrdDataImageReader;
}

