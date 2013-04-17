/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkAnalyzeDataImageReader.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkAnalyzeImageIO.h>

const char itkAnalyzeDataImageReader::ID[] = "itkAnalyzeDataImageReader";

itkAnalyzeDataImageReader::itkAnalyzeDataImageReader(): itkDataImageReaderBase() {
    this->io = itk::AnalyzeImageIO::New();
}

itkAnalyzeDataImageReader::~itkAnalyzeDataImageReader() { }

QStringList itkAnalyzeDataImageReader::handled() const {
    return s_handled();
}

QStringList itkAnalyzeDataImageReader::s_handled() {
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
            << "itkDataImageRGB3" << "itkDataImageRGB3";
}

bool itkAnalyzeDataImageReader::registered() {
    return dtkAbstractDataFactory::instance()->registerDataReaderType(ID,s_handled(),createItkAnalyzeDataImageReader);
}

QString itkAnalyzeDataImageReader::identifier() const {
    return ID;
}

QString itkAnalyzeDataImageReader::description() const {
    return "Reader for analyze images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkAnalyzeDataImageReader() {
    return new itkAnalyzeDataImageReader;
}
