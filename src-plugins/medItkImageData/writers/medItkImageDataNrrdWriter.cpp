/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkImageDataNrrdWriter.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkNrrdImageIO.h>

static QString s_identifier() {
    return "medItkImageDataNrrdWriter";
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

medItkImageDataNrrdWriter::medItkImageDataNrrdWriter(): medItkImageDataWriterBase() {
    this->io = itk::NrrdImageIO::New();
}

medItkImageDataNrrdWriter::~medItkImageDataNrrdWriter() {
}

QStringList medItkImageDataNrrdWriter::handled() const {
    return s_handled();
}

QStringList medItkImageDataNrrdWriter::supportedFileExtensions() const
{
    return QStringList() << ".nrrd" << ".nhdr";
}

bool medItkImageDataNrrdWriter::registered() {
    return medAbstractDataFactory::instance()->registerDataWriterType(s_identifier(), s_handled(), create);
}

QString medItkImageDataNrrdWriter::identifier() const {
    return s_identifier();
}

QString medItkImageDataNrrdWriter::description() const {
    return "Nrrd image exporter";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter * medItkImageDataNrrdWriter::create() {
    return new medItkImageDataNrrdWriter;
}

