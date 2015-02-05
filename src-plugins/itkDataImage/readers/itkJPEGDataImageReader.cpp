/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkJPEGDataImageReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkJPEGImageIO.h>

const char itkJPEGDataImageReader::ID[] = "itkJPEGDataImageReader";

itkJPEGDataImageReader::itkJPEGDataImageReader(): itkDataImageReaderBase() {
    this->io = itk::JPEGImageIO::New();
}

itkJPEGDataImageReader::~itkJPEGDataImageReader() { }

QStringList itkJPEGDataImageReader::handled() const {
    return s_handled();
}

QStringList itkJPEGDataImageReader::s_handled() {
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
            << "itkDataImageRGB3" << "itkDataImageRGB3" << "itkDataImageRGB4";
}

bool itkJPEGDataImageReader::registered() {
    return medAbstractDataFactory::instance()->registerDataReaderType(ID,s_handled(),createItkJPEGDataImageReader);
}

QString itkJPEGDataImageReader::identifier() const {
    return ID;
}

QString itkJPEGDataImageReader::description() const {
    return "Reader for JPEG images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkJPEGDataImageReader() {
    return new itkJPEGDataImageReader;
}
