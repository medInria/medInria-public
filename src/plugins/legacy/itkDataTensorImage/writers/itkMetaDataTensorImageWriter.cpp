/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkMetaDataTensorImageWriter.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkMetaImageIO.h>

static QString s_identifier() {
    return "itkMetaDataTensorImageWriter";
}

itkMetaDataTensorImageWriter::itkMetaDataTensorImageWriter() : itkDataTensorImageWriterBase()
{
    this->io = itk::MetaImageIO::New();
}

itkMetaDataTensorImageWriter::~itkMetaDataTensorImageWriter()
{
}

bool itkMetaDataTensorImageWriter::registered()
{
  return medAbstractDataFactory::instance()->registerDataWriterType(
              QString("itkMetaDataTensorImageWriter"),
              itkDataTensorImageWriterBase::s_handled(),
              create);
}

QString itkMetaDataTensorImageWriter::description() const
{
    return tr("Tensor image MetaData exporter" );
}

QString itkMetaDataTensorImageWriter::identifier() const
{
    return s_identifier();
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter * itkMetaDataTensorImageWriter::create()
{
    return new itkMetaDataTensorImageWriter;
}
