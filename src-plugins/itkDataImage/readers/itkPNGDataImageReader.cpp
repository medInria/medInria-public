/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkPNGDataImageReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkPNGImageIO.h>

const char itkPNGDataImageReader::ID[] = "itkPNGDataImageReader";

itkPNGDataImageReader::itkPNGDataImageReader(): itkDataImageReaderBase() {
    this->io = itk::PNGImageIO::New();
}

itkPNGDataImageReader::~itkPNGDataImageReader() { }

QStringList itkPNGDataImageReader::handled() const {
    return s_handled();
}

QStringList itkPNGDataImageReader::s_handled() {
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
            << "itkDataImageRGB3" << "itkDataImageRGBA3" << "itkDataImageRGB4";
}

bool itkPNGDataImageReader::registered() {
    return medAbstractDataFactory::instance()->registerDataReaderType(ID,s_handled(),createItkPNGDataImageReader);
}

QString itkPNGDataImageReader::identifier() const {
    return ID;
}

QString itkPNGDataImageReader::description() const {
    return "Reader for PNG images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkPNGDataImageReader() {
    return new itkPNGDataImageReader;
}
