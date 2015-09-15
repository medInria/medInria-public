/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkNiftiDataTensorImageReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkNiftiImageIO.h>

static QString s_identifier() {
    return "itkNiftiDataTensorImageReader";
}

itkNiftiDataTensorImageReader::itkNiftiDataTensorImageReader(): itkDataTensorImageReaderBase() {
    this->io = itk::NiftiImageIO::New();
}

itkNiftiDataTensorImageReader::~itkNiftiDataTensorImageReader() { }

bool itkNiftiDataTensorImageReader::registered() {
  return medAbstractDataFactory::instance()->registerDataReaderType(s_identifier(),itkDataTensorImageReaderBase::s_handled(), create);
}

QString itkNiftiDataTensorImageReader::identifier() const {
    return s_identifier();
}

QString itkNiftiDataTensorImageReader::description() const {
    return "Reader for Nifti tensor images";
}

dtkAbstractDataReader * itkNiftiDataTensorImageReader::create() {
    return new itkNiftiDataTensorImageReader;
}

