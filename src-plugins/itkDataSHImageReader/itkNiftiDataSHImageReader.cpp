/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkNiftiDataSHImageReader.h>
#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <itkNiftiImageIO.h>

itkNiftiDataSHImageReader::itkNiftiDataSHImageReader() : itkDataSHImageReaderBase()
{
    this->io = itk::NiftiImageIO::New();
}

itkNiftiDataSHImageReader::~itkNiftiDataSHImageReader()
{
}

bool itkNiftiDataSHImageReader::registered()
{
  return medAbstractDataFactory::instance()->registerDataReaderType("itkNiftiDataSHImageReader", itkDataSHImageReaderBase::s_handled(),
                                                                    createItkNiftiDataSHImageReader);
}

QString itkNiftiDataSHImageReader::description() const
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

dtkAbstractDataReader *createItkNiftiDataSHImageReader()
{
    return new itkNiftiDataSHImageReader;
}

