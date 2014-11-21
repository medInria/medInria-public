/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTtkTensorImageMetaDataWriter.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkMetaImageIO.h>

static QString s_identifier() {
    return "medTtkTensorImageMetaDataWriter";
}

medTtkTensorImageMetaDataWriter::medTtkTensorImageMetaDataWriter() : medTtkTensorImageDataWriterBase()
{
    this->io = itk::MetaImageIO::New();
}


medTtkTensorImageMetaDataWriter::~medTtkTensorImageMetaDataWriter()
{
}


bool medTtkTensorImageMetaDataWriter::registered()
{
  return medAbstractDataFactory::instance()->registerDataWriterType(
              "medTtkTensorImageMetaDataWriter",
              medTtkTensorImageDataWriterBase::s_handled(),
              create);
}

QString medTtkTensorImageMetaDataWriter::description() const
{
    return tr("Tensor image MetaData exporter" );
}

QString medTtkTensorImageMetaDataWriter::identifier() const
{
    return s_identifier();
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter * medTtkTensorImageMetaDataWriter::create()
{
    return new medTtkTensorImageMetaDataWriter;
}

