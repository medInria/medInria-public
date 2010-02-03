#include "medItk.h"

#include "itkMetaDataImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkMetaImageIO.h>


itkMetaDataImageReader::itkMetaDataImageReader(void) : itkDataImageReaderBase()
{
    this->io = itk::MetaImageIO::New();
}


itkMetaDataImageReader::~itkMetaDataImageReader(void)
{
}


bool itkMetaDataImageReader::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataReaderType("itkMetaDataImageReader", itkDataImageReaderBase::p_handled(),
								    createItkMetaDataImageReader);
}


QString itkMetaDataImageReader::description(void) const
{
    return "itkMetaDataImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkMetaDataImageReader(void)
{
    return new itkMetaDataImageReader;
}

