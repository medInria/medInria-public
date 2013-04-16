/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkNiftiDataImageWriter.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkNiftiImageIO.h>

const char itkNiftiDataImageWriter::ID[] = "itkNiftiDataImageWriter";

itkNiftiDataImageWriter::itkNiftiDataImageWriter(): itkDataImageWriterBase() {
    this->io = itk::NiftiImageIO::New();
}

itkNiftiDataImageWriter::~itkNiftiDataImageWriter() { }

QStringList itkNiftiDataImageWriter::handled() const {
    return s_handled();
}

QStringList itkNiftiDataImageWriter::s_handled() {
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

bool itkNiftiDataImageWriter::registered() {
    return dtkAbstractDataFactory::instance()->registerDataWriterType(ID,s_handled(),createItkNiftiDataImageWriter);
}

QString itkNiftiDataImageWriter::identifier() const {
    return ID;
}

QString itkNiftiDataImageWriter::description() const {
    return "Writer for Nifti images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkNiftiDataImageWriter() {
    return new itkNiftiDataImageWriter;
}

