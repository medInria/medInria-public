/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkMetaDataImageWriter.h>

#include <medAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkMetaImageIO.h>

QString s_identifier() {
    return "itkMetaDataImageWriter";
}

QStringList s_handled() {
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
                          << "itkDataImageVectorUChar3" << "itkDataImageVectorFloat3"
                          << "itkDataImageRGB3" << "itkDataImageRGBA3";
}

itkMetaDataImageWriter::itkMetaDataImageWriter(): itkDataImageWriterBase() {
    this->io = itk::MetaImageIO::New();
}

itkMetaDataImageWriter::~itkMetaDataImageWriter() { }

QStringList itkMetaDataImageWriter::handled() const {
    return s_handled();
}

bool itkMetaDataImageWriter::registered() {
    return dtkAbstractDataFactory::instance()->registerDataWriterType(s_identifier(), s_handled(), create);
}

QString itkMetaDataImageWriter::identifier() const {
    return s_identifier();
}

QString itkMetaDataImageWriter::description() const {
    return "ITK MetaData image exporter";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter * itkMetaDataImageWriter::create() {
    return new itkMetaDataImageWriter;
}

