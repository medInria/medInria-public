#include "itkNrrdDataSHImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkNrrdImageIO.h>

itkNrrdDataSHImageReader::itkNrrdDataSHImageReader(void) : itkDataSHImageReaderBase()
{
    this->io = itk::NrrdImageIO::New();
}

itkNrrdDataSHImageReader::~itkNrrdDataSHImageReader(void)
{
}


bool itkNrrdDataSHImageReader::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataReaderType("itkNrrdDataSHImageReader", itkDataSHImageReaderBase::s_handled(),
                                                                    createItkNrrdDataSHImageReader);
}

QString itkNrrdDataSHImageReader::description(void) const
{
    return tr("itk Nrrd SH reader");
}

QString itkNrrdDataSHImageReader::identifier() const
{
    return "itkNrrdDataSHImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkNrrdDataSHImageReader(void)
{
    return new itkNrrdDataSHImageReader;
}

