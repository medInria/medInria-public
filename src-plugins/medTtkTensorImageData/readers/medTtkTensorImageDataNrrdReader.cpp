/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTtkTensorImageDataNrrdReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkNrrdImageIO.h>

const char medTtkTensorImageDataNrrdReader::ID[] = "medTtkTensorImageDataNrrdReader";

medTtkTensorImageDataNrrdReader::medTtkTensorImageDataNrrdReader(): medTtkTensorImageDataReaderBase() {
    this->io = itk::NrrdImageIO::New();
}

medTtkTensorImageDataNrrdReader::~medTtkTensorImageDataNrrdReader() { }

bool medTtkTensorImageDataNrrdReader::registered() {
  return medAbstractDataFactory::instance()->registerDataReaderType(ID, medTtkTensorImageDataReaderBase::s_handled(),
                                    createmedTtkTensorImageDataNrrdReader);
}

QString medTtkTensorImageDataNrrdReader::identifier() const {
    return ID;
}

QString medTtkTensorImageDataNrrdReader::description() const {
    return "Reader for Nrrd tensor images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createmedTtkTensorImageDataNrrdReader() {
    return new medTtkTensorImageDataNrrdReader;
}

