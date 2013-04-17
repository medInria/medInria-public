/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkJPEGDataImageWriter.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkJPEGImageIO.h>

const char itkJPEGDataImageWriter::ID[] = "itkJPEGDataImageWriter";

itkJPEGDataImageWriter::itkJPEGDataImageWriter(): itkDataImageWriterBase() {
    this->io = itk::JPEGImageIO::New();
}

itkJPEGDataImageWriter::~itkJPEGDataImageWriter() { }

QStringList itkJPEGDataImageWriter::handled() const {
    return s_handled();
}

QStringList itkJPEGDataImageWriter::s_handled() {
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

bool itkJPEGDataImageWriter::registered() {
    return dtkAbstractDataFactory::instance()->registerDataWriterType(ID,s_handled(),createItkJPEGDataImageWriter);
}

QString itkJPEGDataImageWriter::identifier() const {
    return ID;
}

QString itkJPEGDataImageWriter::description() const {
    return "Writer for JPEG images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkJPEGDataImageWriter() {
    return new itkJPEGDataImageWriter;
}

