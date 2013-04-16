/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkNiftiDataTensorImageReader.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkNiftiImageIO.h>

const char itkNiftiDataTensorImageReader::ID[] = "itkNiftiDataTensorImageReader";

itkNiftiDataTensorImageReader::itkNiftiDataTensorImageReader(): itkDataTensorImageReaderBase() {
    this->io = itk::NiftiImageIO::New();
}

itkNiftiDataTensorImageReader::~itkNiftiDataTensorImageReader() { }

bool itkNiftiDataTensorImageReader::registered() {
  return dtkAbstractDataFactory::instance()->registerDataReaderType(ID,itkDataTensorImageReaderBase::s_handled(),
								    createItkNiftiDataTensorImageReader);
}

QString itkNiftiDataTensorImageReader::identifier() const {
    return ID;
}

QString itkNiftiDataTensorImageReader::description() const {
    return "Reader for Nifti tensor images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkNiftiDataTensorImageReader()
{
    return new itkNiftiDataTensorImageReader;
}

