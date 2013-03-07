#include "itkNrrdDataShImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkNrrdImageIO.h>

itkNrrdDataShImageReader::itkNrrdDataShImageReader(void) : itkDataShImageReaderBase()
{
    this->io = itk::NrrdImageIO::New();
}

itkNrrdDataShImageReader::~itkNrrdDataShImageReader(void)
{
}


bool itkNrrdDataShImageReader::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataReaderType("itkNrrdDataShImageReader", itkDataShImageReaderBase::s_handled(),
                                                                    createItkNrrdDataShImageReader);
}

QString itkNrrdDataShImageReader::description(void) const
{
    return tr("itk Nrrd SH reader");
}

QString itkNrrdDataShImageReader::identifier() const
{
    return "itkNrrdDataShImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkNrrdDataShImageReader(void)
{
    return new itkNrrdDataShImageReader;
}

