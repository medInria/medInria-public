/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkNrrdDataImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkNrrdImageIO.h>

const char itkNrrdDataImageWriter::ID[] = "itkNrrdDataImageWriter";

itkNrrdDataImageWriter::itkNrrdDataImageWriter(): itkDataImageWriterBase() {
    this->io = itk::NrrdImageIO::New();
}

itkNrrdDataImageWriter::~itkNrrdDataImageWriter() {
}

QStringList itkNrrdDataImageWriter::handled() const {
    return s_handled();
}

QStringList itkNrrdDataImageWriter::s_handled() {
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

bool itkNrrdDataImageWriter::registered() {
    return dtkAbstractDataFactory::instance()->registerDataWriterType(ID,s_handled(),createItkNrrdDataImageWriter);
}

QString itkNrrdDataImageWriter::identifier() const {
    return ID;
}

QString itkNrrdDataImageWriter::description() const {
    return "Writer for Nrrd images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkNrrdDataImageWriter() {
    return new itkNrrdDataImageWriter;
}

