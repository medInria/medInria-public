/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkImageDataGiplWriter.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkGiplImageIO.h>

static QString s_identifier() {
    return "medItkImageDataGiplWriter";
}

static QStringList s_handled() {
    return QStringList()  << "medItkChar3ImageData" << "medItkChar4ImageData"
                          << "medItkUChar3ImageData" << "medItkUChar4ImageData"
                          << "medItkShort3ImageData" << "medItkShort4ImageData"
                          << "medItkUShort3ImageData" << "medItkUShort4ImageData"
                          << "medItkInt3ImageData" << "medItkInt4ImageData"
                          << "medItkUInt3ImageData" << "medItkUInt4ImageData"
                          << "medItkLong3ImageData" << "medItkLong4ImageData"
                          << "medItkULong3ImageData" << "medItkULong4ImageData"
                          << "medItkFloat3ImageData" << "medItkFloat4ImageData"
                          << "medItkDouble3ImageData" << "medItkDouble4ImageData"
                          << "medItkRgb3ImageData" << "medItkRgba3ImageData";
}

medItkImageDataGiplWriter::medItkImageDataGiplWriter(): medItkImageDataWriterBase() {
    this->io = itk::GiplImageIO::New();
}

medItkImageDataGiplWriter::~medItkImageDataGiplWriter() { }

QStringList medItkImageDataGiplWriter::handled() const {
    return s_handled();
}

QStringList medItkImageDataGiplWriter::supportedFileExtensions() const
{
    return QStringList() << ".gipl" << ".gipl.gz";
}

bool medItkImageDataGiplWriter::registered() {
    return medAbstractDataFactory::instance()->registerDataWriterType(s_identifier(), s_handled(), create);
}

QString medItkImageDataGiplWriter::identifier() const {
    return s_identifier();
}

QString medItkImageDataGiplWriter::description() const {
    return "Gipl image exporter";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter * medItkImageDataGiplWriter::create() {
    return new medItkImageDataGiplWriter;
}

