/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkBMPDataImageReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkBMPImageIO.h>

const char itkBMPDataImageReader::ID[] = "itkBMPDataImageReader";

itkBMPDataImageReader::itkBMPDataImageReader(): itkDataImageReaderBase() {
    this->io = itk::BMPImageIO::New();
}

itkBMPDataImageReader::~itkBMPDataImageReader() { }

QStringList itkBMPDataImageReader::handled() const {
    return s_handled();
}

QStringList itkBMPDataImageReader::s_handled() {
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
            << "itkDataImageRGB3" << "itkDataImageRGBA3";
}

bool itkBMPDataImageReader::registered() {
    return medAbstractDataFactory::instance()->registerDataReaderType(ID,s_handled(),createItkBMPDataImageReader);
}

QString itkBMPDataImageReader::identifier() const {
    return ID;
}

QString itkBMPDataImageReader::description() const {
    return "Reader for BMP images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkBMPDataImageReader() {
    return new itkBMPDataImageReader;
}
