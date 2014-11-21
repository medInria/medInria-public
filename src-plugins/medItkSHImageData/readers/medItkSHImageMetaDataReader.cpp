/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkSHImageMetaDataReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkMetaImageIO.h>

medItkSHImageMetaDataReader::medItkSHImageMetaDataReader(): medItkSHImageDataReaderBase()
{
    this->io = itk::MetaImageIO::New();
}

medItkSHImageMetaDataReader::~medItkSHImageMetaDataReader()
{
}

bool medItkSHImageMetaDataReader::registered()
{
  return medAbstractDataFactory::instance()->registerDataReaderType("medItkSHImageMetaDataReader", medItkSHImageDataReaderBase::s_handled(),
                                                                    createItkMetaDataSHImageReader);
}

QString medItkSHImageMetaDataReader::description() const
{
    return tr("itk SH 3d image reader");
}

QString medItkSHImageMetaDataReader::identifier() const
{
    return "medItkSHImageMetaDataReader";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkMetaDataSHImageReader()
{
    return new medItkSHImageMetaDataReader;
}

