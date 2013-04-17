/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkPNGDataImageWriter.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkPNGImageIO.h>

const char itkPNGDataImageWriter::ID[] = "itkPNGDataImageWriter";

itkPNGDataImageWriter::itkPNGDataImageWriter(): itkDataImageWriterBase() {
    this->io = itk::PNGImageIO::New();
}

itkPNGDataImageWriter::~itkPNGDataImageWriter() { }

QStringList itkPNGDataImageWriter::handled() const {
    return s_handled();
}

QStringList itkPNGDataImageWriter::s_handled() {
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

bool itkPNGDataImageWriter::registered() {
    return dtkAbstractDataFactory::instance()->registerDataWriterType(ID,s_handled(),createItkPNGDataImageWriter);
}

QString itkPNGDataImageWriter::identifier() const {
    return ID;
}

QString itkPNGDataImageWriter::description() const {
    return "Writer for PNG images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkPNGDataImageWriter() {
    return new itkPNGDataImageWriter;
}

