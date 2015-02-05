/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkGiplDataImageReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkGiplImageIO.h>

const char itkGiplDataImageReader::ID[] = "itkGiplDataImageReader";

itkGiplDataImageReader::itkGiplDataImageReader(): itkDataImageReaderBase() {
    this->io = itk::GiplImageIO::New();
}

itkGiplDataImageReader::~itkGiplDataImageReader() { }

QStringList itkGiplDataImageReader::handled() const {
    return s_handled();
}

QStringList itkGiplDataImageReader::s_handled() {
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

bool itkGiplDataImageReader::registered() {
    return medAbstractDataFactory::instance()->registerDataReaderType(ID,s_handled(),createItkGiplDataImageReader);
}

QString itkGiplDataImageReader::identifier() const {
    return ID;
}

QString itkGiplDataImageReader::description() const {
    return "Reader for Gipl images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkGiplDataImageReader() {
    return new itkGiplDataImageReader;
}

