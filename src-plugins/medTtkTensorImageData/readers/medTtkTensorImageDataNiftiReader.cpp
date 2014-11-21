/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTtkTensorImageDataNiftiReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkNiftiImageIO.h>

static QString s_identifier() {
    return "medTtkTensorImageDataNiftiReader";
}

medTtkTensorImageDataNiftiReader::medTtkTensorImageDataNiftiReader(): medTtkTensorImageDataReaderBase() {
    this->io = itk::NiftiImageIO::New();
}

medTtkTensorImageDataNiftiReader::~medTtkTensorImageDataNiftiReader() { }

bool medTtkTensorImageDataNiftiReader::registered() {
  return medAbstractDataFactory::instance()->registerDataReaderType(s_identifier(),medTtkTensorImageDataReaderBase::s_handled(), create);
}

QString medTtkTensorImageDataNiftiReader::identifier() const {
    return s_identifier();
}

QString medTtkTensorImageDataNiftiReader::description() const {
    return "Reader for Nifti tensor images";
}

dtkAbstractDataReader * medTtkTensorImageDataNiftiReader::create() {
    return new medTtkTensorImageDataNiftiReader;
}

