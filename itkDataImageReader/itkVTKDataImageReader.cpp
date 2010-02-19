#include "medItk/medItk.h"

#include "itkVTKDataImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkVTKImageIO.h>


itkVTKDataImageReader::itkVTKDataImageReader(void) : itkDataImageReaderBase()
{
    this->io = itk::VTKImageIO::New();
}


itkVTKDataImageReader::~itkVTKDataImageReader(void)
{
}


bool itkVTKDataImageReader::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataReaderType("itkVTKDataImageReader", itkDataImageReaderBase::s_handled(),
								    createItkVTKDataImageReader);
}


QString itkVTKDataImageReader::description(void) const
{
    return "itkVTKDataImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkVTKDataImageReader(void)
{
    return new itkVTKDataImageReader;
}

