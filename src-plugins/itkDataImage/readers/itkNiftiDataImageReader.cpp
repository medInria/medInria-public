/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkNiftiDataImageReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkNiftiImageIO.h>

const char itkNiftiDataImageReader::ID[] = "itkNiftiDataImageReader";

itkNiftiDataImageReader::itkNiftiDataImageReader(): itkDataImageReaderBase() {
    this->io = itk::NiftiImageIO::New();
}

itkNiftiDataImageReader::~itkNiftiDataImageReader() { }

QStringList itkNiftiDataImageReader::handled() const {
    return s_handled();
}

QStringList itkNiftiDataImageReader::s_handled() {
    return QStringList()  << "itkDataImageChar3" << "itkDataImageChar4"
            << "itkDataImageUChar3" << "itkDataImageUChar4"
            << "itkDataImageShort3" << "itkDataImageShort4"
            << "itkDataImageUShort3" << "itkDataImageUShort4"
            << "itkDataImageInt3" << "itkDataImageInt4"
            << "itkDataImageUInt3" << "itkDataImageUInt4"
            << "itkDataImageLong3" << "itkDataImageLong4"
            << "itkDataImageULong3" << "itkDataImageULong4"
            << "itkDataImageFloat3" << "itkDataImageFloat4"
            << "itkDataImageDouble3" << "itkDataImageDouble4";
}

bool itkNiftiDataImageReader::registered() {
    return medAbstractDataFactory::instance()->registerDataReaderType(ID,s_handled(),createItkNiftiDataImageReader);
}

QString itkNiftiDataImageReader::identifier() const {
    return ID;
}

QString itkNiftiDataImageReader::description() const {
    return "Reader for nifti images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkNiftiDataImageReader() {
    return new itkNiftiDataImageReader;
}
