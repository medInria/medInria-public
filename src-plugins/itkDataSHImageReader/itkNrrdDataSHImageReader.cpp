/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkNrrdDataSHImageReader.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkNrrdImageIO.h>

itkNrrdDataSHImageReader::itkNrrdDataSHImageReader() : itkDataSHImageReaderBase()
{
    this->io = itk::NrrdImageIO::New();
}

itkNrrdDataSHImageReader::~itkNrrdDataSHImageReader()
{
}

bool itkNrrdDataSHImageReader::registered()
{
  return dtkAbstractDataFactory::instance()->registerDataReaderType("itkNrrdDataSHImageReader", itkDataSHImageReaderBase::s_handled(),
                                                                    createItkNrrdDataSHImageReader);
}

QString itkNrrdDataSHImageReader::description() const
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

dtkAbstractDataReader *createItkNrrdDataSHImageReader()
{
    return new itkNrrdDataSHImageReader;
}

