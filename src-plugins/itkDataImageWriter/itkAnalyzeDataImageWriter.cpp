/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkAnalyzeDataImageWriter.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkAnalyzeImageIO.h>

const char itkAnalyzeDataImageWriter::ID[] = "itkAnalyzeDataImageWriter";

itkAnalyzeDataImageWriter::itkAnalyzeDataImageWriter(): itkDataImageWriterBase() {
    this->io = itk::AnalyzeImageIO::New();
}

itkAnalyzeDataImageWriter::~itkAnalyzeDataImageWriter() { }

QStringList itkAnalyzeDataImageWriter::handled() const {
  return s_handled();
}

QStringList itkAnalyzeDataImageWriter::s_handled() {
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

bool itkAnalyzeDataImageWriter::registered() {
  return dtkAbstractDataFactory::instance()->registerDataWriterType(ID,s_handled(),createItkAnalyzeDataImageWriter);
}

QString itkAnalyzeDataImageWriter::identifier() const {
    return ID;
}

QString itkAnalyzeDataImageWriter::description() const {
    return "Writer for analyze images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkAnalyzeDataImageWriter() {
    return new itkAnalyzeDataImageWriter;
}

