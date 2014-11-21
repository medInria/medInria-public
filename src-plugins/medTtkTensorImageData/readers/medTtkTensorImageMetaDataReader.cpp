/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTtkTensorImageMetaDataReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkMetaImageIO.h>

const char medTtkTensorImageMetaDataReader::ID[] = "medTtkTensorImageMetaDataReader";

medTtkTensorImageMetaDataReader::medTtkTensorImageMetaDataReader(): medTtkTensorImageDataReaderBase() {
    this->io = itk::MetaImageIO::New();
}

medTtkTensorImageMetaDataReader::~medTtkTensorImageMetaDataReader() { }

bool medTtkTensorImageMetaDataReader::registered() {
  return medAbstractDataFactory::instance()->registerDataReaderType(ID,medTtkTensorImageDataReaderBase::s_handled(),
                                    createmedTtkTensorImageMetaDataReader);
}

QString medTtkTensorImageMetaDataReader::identifier() const {
    return ID;
}

QString medTtkTensorImageMetaDataReader::description() const {
    return "Reader for meta data tensor images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createmedTtkTensorImageMetaDataReader() {
    return new medTtkTensorImageMetaDataReader;
}

