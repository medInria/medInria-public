/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkMetaDataTensorImageReader.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkMetaImageIO.h>

const char itkMetaDataTensorImageReader::ID[] = "itkMetaDataTensorImageReader";

itkMetaDataTensorImageReader::itkMetaDataTensorImageReader(): itkDataTensorImageReaderBase() {
    this->io = itk::MetaImageIO::New();
}

itkMetaDataTensorImageReader::~itkMetaDataTensorImageReader() { }

bool itkMetaDataTensorImageReader::registered() {
  return dtkAbstractDataFactory::instance()->registerDataReaderType(ID,itkDataTensorImageReaderBase::s_handled(),
								    createItkMetaDataTensorImageReader);
}

QString itkMetaDataTensorImageReader::identifier() const {
    return ID;
}

QString itkMetaDataTensorImageReader::description() const {
    return "Reader for meta data tensor images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkMetaDataTensorImageReader() {
    return new itkMetaDataTensorImageReader;
}

