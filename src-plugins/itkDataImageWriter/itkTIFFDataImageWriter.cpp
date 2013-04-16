/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkTIFFDataImageWriter.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkTIFFImageIO.h>

const char itkTIFFDataImageWriter::ID[] = "itkTIFFDataImageWriter";

itkTIFFDataImageWriter::itkTIFFDataImageWriter(): itkDataImageWriterBase() {
    this->io = itk::TIFFImageIO::New();
}

itkTIFFDataImageWriter::~itkTIFFDataImageWriter() { }

QStringList itkTIFFDataImageWriter::handled() const {
    return s_handled();
}

QStringList itkTIFFDataImageWriter::s_handled() {
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

bool itkTIFFDataImageWriter::registered() {
    return dtkAbstractDataFactory::instance()->registerDataWriterType(ID,s_handled(),createItkTIFFDataImageWriter);
}

QString itkTIFFDataImageWriter::identifier() const {
    return ID;
}

QString itkTIFFDataImageWriter::description() const {
    return "Writer for TIFF images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkTIFFDataImageWriter() {
    return new itkTIFFDataImageWriter;
}

