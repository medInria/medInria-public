/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTtkTensorImageDataNrrdWriter.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkNrrdImageIO.h>


static QString s_identifier() {
    return "medTtkTensorImageDataNrrdWriter";
}

medTtkTensorImageDataNrrdWriter::medTtkTensorImageDataNrrdWriter() : medTtkTensorImageDataWriterBase()
{
    this->io = itk::NrrdImageIO::New();
}


medTtkTensorImageDataNrrdWriter::~medTtkTensorImageDataNrrdWriter()
{
}


bool medTtkTensorImageDataNrrdWriter::registered()
{
  return medAbstractDataFactory::instance()->registerDataWriterType(
              s_identifier(),
              medTtkTensorImageDataWriterBase::s_handled(),
              create);
}

QString medTtkTensorImageDataNrrdWriter::description() const
{
    return tr( "Tensor images Nrrd exporter" );
}

QString medTtkTensorImageDataNrrdWriter::identifier() const
{
    return s_identifier();
}

QStringList medTtkTensorImageDataNrrdWriter::supportedFileExtensions() const
{
    return QStringList() << ".nrrd" << ".nhdr";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter * medTtkTensorImageDataNrrdWriter::create()
{
    return new medTtkTensorImageDataNrrdWriter;
}

