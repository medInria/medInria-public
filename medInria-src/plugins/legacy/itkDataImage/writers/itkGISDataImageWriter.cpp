/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkGISDataImageWriter.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkGISImageIO.h>

static QString s_identifier() {
    return "itkGISDataImageWriter";
}

static QStringList s_handled() {
    return QStringList()  << "itkDataImageChar3" << "itkDataImageChar4"
                          << "itkDataImageUChar3" << "itkDataImageUChar4"
                          << "itkDataImageShort3" << "itkDataImageShort4"
                          << "itkDataImageUShort3" << "itkDataImageUShort4"
                          << "itkDataImageInt3" << "itkDataImageInt4"
                          << "itkDataImageUInt3" << "itkDataImageUInt4"
                          << "itkDataImageFloat3" << "itkDataImageFloat4"
                          << "itkDataImageDouble3" << "itkDataImageDouble4";
}

itkGISDataImageWriter::itkGISDataImageWriter(): itkDataImageWriterBase() {
    this->io = itk::GISImageIO::New();
}

itkGISDataImageWriter::~itkGISDataImageWriter() { }

QStringList itkGISDataImageWriter::handled() const {
    return s_handled();
}

QStringList itkGISDataImageWriter::supportedFileExtensions() const
{
    return QStringList() << ".dim" << ".ima";
}

bool itkGISDataImageWriter::registered() {
    return medAbstractDataFactory::instance()->registerDataWriterType(s_identifier(), s_handled(), create);
}

QString itkGISDataImageWriter::identifier() const {
    return s_identifier();
}

QString itkGISDataImageWriter::description() const {
    return "GIS image exporter";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter * itkGISDataImageWriter::create() {
    return new itkGISDataImageWriter;
}

