/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkMetaDataSHImageReader.h>

#include <medAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkMetaImageIO.h>

itkMetaDataSHImageReader::itkMetaDataSHImageReader(): itkDataSHImageReaderBase()
{
    this->io = itk::MetaImageIO::New();
}

itkMetaDataSHImageReader::~itkMetaDataSHImageReader()
{
}

bool itkMetaDataSHImageReader::registered()
{
  return dtkAbstractDataFactory::instance()->registerDataReaderType("itkMetaDataSHImageReader", itkDataSHImageReaderBase::s_handled(),
                                                                    createItkMetaDataSHImageReader);
}

QString itkMetaDataSHImageReader::description() const
{
    return tr("itk SH 3d image reader");
}

QString itkMetaDataSHImageReader::identifier() const
{
    return "itkMetaDataSHImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkMetaDataSHImageReader()
{
    return new itkMetaDataSHImageReader;
}

