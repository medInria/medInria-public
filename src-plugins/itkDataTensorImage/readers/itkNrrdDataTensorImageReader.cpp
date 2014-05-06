/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkNrrdDataTensorImageReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkNrrdImageIO.h>

const char itkNrrdDataTensorImageReader::ID[] = "itkNrrdDataTensorImageReader";

itkNrrdDataTensorImageReader::itkNrrdDataTensorImageReader(): itkDataTensorImageReaderBase() {
    this->io = itk::NrrdImageIO::New();
}

itkNrrdDataTensorImageReader::~itkNrrdDataTensorImageReader() { }

bool itkNrrdDataTensorImageReader::registered() {
  return medAbstractDataFactory::instance()->registerDataReaderType(ID, itkDataTensorImageReaderBase::s_handled(),
								    createItkNrrdDataTensorImageReader);
}

QString itkNrrdDataTensorImageReader::identifier() const {
    return ID;
}

QString itkNrrdDataTensorImageReader::description() const {
    return "Reader for Nrrd tensor images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkNrrdDataTensorImageReader() {
    return new itkNrrdDataTensorImageReader;
}

