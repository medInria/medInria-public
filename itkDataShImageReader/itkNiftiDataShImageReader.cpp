#include "itkNiftiDataShImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkNiftiImageIO.h>


itkNiftiDataShImageReader::itkNiftiDataShImageReader(void) : itkDataShImageReaderBase()
{
    this->io = itk::NiftiImageIO::New();
}

itkNiftiDataShImageReader::~itkNiftiDataShImageReader(void)
{
}

bool itkNiftiDataShImageReader::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataReaderType("itkNiftiDataShImageReader", itkDataShImageReaderBase::s_handled(),
                                                                    createItkNiftiDataShImageReader);
}

QString itkNiftiDataShImageReader::description(void) const
{
    return tr("itk Nifti SH image reader");
}

QString itkNiftiDataShImageReader::identifier() const
{
    return "itkNiftiDataShImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkNiftiDataShImageReader(void)
{
    return new itkNiftiDataShImageReader;
}

