/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkMetaDataTensorImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkMetaImageIO.h>


const char itkMetaDataTensorImageWriter::ID[] = "itkMetaDataTensorImageWriter";

itkMetaDataTensorImageWriter::itkMetaDataTensorImageWriter() : itkDataTensorImageWriterBase()
{
    this->io = itk::MetaImageIO::New();
}


itkMetaDataTensorImageWriter::~itkMetaDataTensorImageWriter()
{
}


bool itkMetaDataTensorImageWriter::registered()
{
  return dtkAbstractDataFactory::instance()->registerDataWriterType(
              "itkMetaDataTensorImageWriter",
              itkDataTensorImageWriterBase::s_handled(),
              createItkMetaDataTensorImageWriter);
}

QString itkMetaDataTensorImageWriter::description() const
{
    return tr( "MetaData writer for Tensor images" );
}

QString itkMetaDataTensorImageWriter::identifier() const
{
    return ID;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkMetaDataTensorImageWriter()
{
    return new itkMetaDataTensorImageWriter;
}

