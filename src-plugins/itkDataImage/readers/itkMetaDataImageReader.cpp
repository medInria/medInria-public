/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkMetaDataImageReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkMetaImageIO.h>

const char itkMetaDataImageReader::ID[] = "itkMetaDataImageReader";

itkMetaDataImageReader::itkMetaDataImageReader(): itkDataImageReaderBase() {
    this->io = itk::MetaImageIO::New();
}

itkMetaDataImageReader::~itkMetaDataImageReader() { }

QStringList itkMetaDataImageReader::handled() const {
    return s_handled();
}

QStringList itkMetaDataImageReader::s_handled() {
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

bool itkMetaDataImageReader::registered() {
    return medAbstractDataFactory::instance()->registerDataReaderType(ID,s_handled(),createItkMetaDataImageReader);
}

QString itkMetaDataImageReader::identifier() const {
    return ID;
}

QString itkMetaDataImageReader::description() const {
    return "Reader for itk meta data";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkMetaDataImageReader() {
    return new itkMetaDataImageReader;
}
