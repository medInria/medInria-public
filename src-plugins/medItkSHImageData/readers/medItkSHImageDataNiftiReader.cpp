/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkSHImageDataNiftiReader.h>
#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <itkNiftiImageIO.h>

medItkSHImageDataNiftiReader::medItkSHImageDataNiftiReader() : medItkSHImageDataReaderBase()
{
    this->io = itk::NiftiImageIO::New();
}

medItkSHImageDataNiftiReader::~medItkSHImageDataNiftiReader()
{
}

bool medItkSHImageDataNiftiReader::registered()
{
  return medAbstractDataFactory::instance()->registerDataReaderType("medItkSHImageDataNiftiReader", medItkSHImageDataReaderBase::s_handled(),
                                                                    createItkNiftiDataSHImageReader);
}

QString medItkSHImageDataNiftiReader::description() const
{
    return tr("itk Nifti SH image reader");
}

QString medItkSHImageDataNiftiReader::identifier() const
{
    return "medItkSHImageDataNiftiReader";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkNiftiDataSHImageReader()
{
    return new medItkSHImageDataNiftiReader;
}

