/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkImageDataGisWriter.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <medItkGisImageIO.h>

static QString s_identifier() {
    return "medItkImageDataGisWriter";
}

static QStringList s_handled() {
    return QStringList()  << "medItkChar3ImageData" << "medItkChar4ImageData"
                          << "medItkUChar3ImageData" << "medItkUChar4ImageData"
                          << "medItkShort3ImageData" << "medItkShort4ImageData"
                          << "medItkUShort3ImageData" << "medItkUShort4ImageData"
                          << "medItkInt3ImageData" << "medItkInt4ImageData"
                          << "medItkUInt3ImageData" << "medItkUInt4ImageData"
                          << "medItkFloat3ImageData" << "medItkFloat4ImageData"
                          << "medItkDouble3ImageData" << "medItkDouble4ImageData";
}

medItkImageDataGisWriter::medItkImageDataGisWriter(): medItkImageDataWriterBase() {
    this->io = itk::GISImageIO::New();
}

medItkImageDataGisWriter::~medItkImageDataGisWriter() { }

QStringList medItkImageDataGisWriter::handled() const {
    return s_handled();
}

QStringList medItkImageDataGisWriter::supportedFileExtensions() const
{
    return QStringList() << ".dim" << ".ima";
}

bool medItkImageDataGisWriter::registered() {
    return medAbstractDataFactory::instance()->registerDataWriterType(s_identifier(), s_handled(), create);
}

QString medItkImageDataGisWriter::identifier() const {
    return s_identifier();
}

QString medItkImageDataGisWriter::description() const {
    return "GIS image exporter";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter * medItkImageDataGisWriter::create() {
    return new medItkImageDataGisWriter;
}

