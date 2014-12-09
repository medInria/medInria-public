/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTtkTensorImageDataNiftiWriter.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkNiftiImageIO.h>


static QString s_identifier() {
    return "medTtkTensorImageDataNiftiWriter";
}

medTtkTensorImageDataNiftiWriter::medTtkTensorImageDataNiftiWriter() : medTtkTensorImageDataWriterBase()
{
    this->io = itk::NiftiImageIO::New();
}


medTtkTensorImageDataNiftiWriter::~medTtkTensorImageDataNiftiWriter()
{
}


bool medTtkTensorImageDataNiftiWriter::registered()
{
  return medAbstractDataFactory::instance()->registerDataWriterType(s_identifier(), s_handled(), create);
}

QString medTtkTensorImageDataNiftiWriter::description() const
{
    return tr( "Tensor image Nifti exporter" );
}

QString medTtkTensorImageDataNiftiWriter::identifier() const
{
    return s_identifier();
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter * medTtkTensorImageDataNiftiWriter::create()
{
    return new medTtkTensorImageDataNiftiWriter;
}

