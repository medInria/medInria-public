#include "itkNiftiDataSHImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkNiftiImageIO.h>


itkNiftiDataSHImageReader::itkNiftiDataSHImageReader(void) : itkDataSHImageReaderBase()
{
    this->io = itk::NiftiImageIO::New();
}

itkNiftiDataSHImageReader::~itkNiftiDataSHImageReader(void)
{
}

bool itkNiftiDataSHImageReader::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataReaderType("itkNiftiDataSHImageReader", itkDataSHImageReaderBase::s_handled(),
                                                                    createItkNiftiDataSHImageReader);
}

QString itkNiftiDataSHImageReader::description(void) const
{
    return tr("itk Nifti SH image reader");
}

QString itkNiftiDataSHImageReader::identifier() const
{
    return "itkNiftiDataSHImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkNiftiDataSHImageReader(void)
{
    return new itkNiftiDataSHImageReader;
}

