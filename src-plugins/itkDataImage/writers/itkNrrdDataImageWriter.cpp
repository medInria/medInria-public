/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkNrrdDataImageWriter.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkNrrdImageIO.h>

static QString s_identifier() {
    return "itkNrrdDataImageWriter";
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
                          << "itkDataImageRGB3" << "itkDataImageRGBA3" << "itkDataImageRGB4";
}

itkNrrdDataImageWriter::itkNrrdDataImageWriter(): itkDataImageWriterBase() {
    this->io = itk::NrrdImageIO::New();
}

itkNrrdDataImageWriter::~itkNrrdDataImageWriter() {
}

QStringList itkNrrdDataImageWriter::handled() const {
    return s_handled();
}

QStringList itkNrrdDataImageWriter::supportedFileExtensions() const
{
    return QStringList() << ".nrrd" << ".nhdr";
}

bool itkNrrdDataImageWriter::registered() {
    return medAbstractDataFactory::instance()->registerDataWriterType(s_identifier(), s_handled(), create);
}

QString itkNrrdDataImageWriter::identifier() const {
    return s_identifier();
}

QString itkNrrdDataImageWriter::description() const {
    return "Nrrd image exporter";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter * itkNrrdDataImageWriter::create() {
    return new itkNrrdDataImageWriter;
}

