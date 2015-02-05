/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkNrrdDataImageReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkNrrdImageIO.h>

const char itkNrrdDataImageReader::ID[] = "itkNrrdDataImageReader";

itkNrrdDataImageReader::itkNrrdDataImageReader(): itkDataImageReaderBase() {
    this->io = itk::NrrdImageIO::New();
}

itkNrrdDataImageReader::~itkNrrdDataImageReader() { }

QStringList itkNrrdDataImageReader::handled() const {
    return s_handled();
}

QStringList itkNrrdDataImageReader::s_handled() {
    return QStringList()  << "itkDataImageChar3" << "itkDataImageChar4"
            << "itkDataImageUChar3" << "itkDataImageUChar4"
            << "itkDataImageShort3" << "itkDataImageShort4"
            << "itkDataImageUShort3" << "itkDataImageUShort4"
            << "itkDataImageInt3" << "itkDataImageInt4"
            << "itkDataImageUInt3" << "itkDataImageUInt4"
            << "itkDataImageLong3" << "itkDataImageLong4"
            << "itkDataImageULong3" << "itkDataImageULong4"
            << "itkDataImageFloat3" << "itkDataImageFloat4"
            << "itkDataImageDouble3" << "itkDataImageDouble4"
            << "itkDataImageRGB3" << "itkDataImageRGBA3"<< "itkDataImageRGB4";
}

bool itkNrrdDataImageReader::registered() {
    return medAbstractDataFactory::instance()->registerDataReaderType(ID,s_handled(),createItkNrrdDataImageReader);
}

QString itkNrrdDataImageReader::identifier() const {
    return ID;
}

QString itkNrrdDataImageReader::description() const {
    return "Reader for itk Nrrd images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkNrrdDataImageReader() {
    return new itkNrrdDataImageReader;
}

