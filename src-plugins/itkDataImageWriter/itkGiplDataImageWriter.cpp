/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkGiplDataImageWriter.h>

#include <medAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkGiplImageIO.h>

static QString s_identifier() {
    return "itkGiplDataImageWriter";
}

static QStringList s_handled() {
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

itkGiplDataImageWriter::itkGiplDataImageWriter(): itkDataImageWriterBase() {
    this->io = itk::GiplImageIO::New();
}

itkGiplDataImageWriter::~itkGiplDataImageWriter() { }

QStringList itkGiplDataImageWriter::handled() const {
    return s_handled();
}

QStringList itkGiplDataImageWriter::supportedFileExtensions() const
{
    return QStringList() << ".gipl" << ".gipl.gz";
}

bool itkGiplDataImageWriter::registered() {
    return dtkAbstractDataFactory::instance()->registerDataWriterType(s_identifier(), s_handled(), create);
}

QString itkGiplDataImageWriter::identifier() const {
    return s_identifier();
}

QString itkGiplDataImageWriter::description() const {
    return "Gipl image exporter";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter * itkGiplDataImageWriter::create() {
    return new itkGiplDataImageWriter;
}

