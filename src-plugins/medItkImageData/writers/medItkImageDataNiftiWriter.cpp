/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkImageDataNiftiWriter.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkNiftiImageIO.h>

static QString s_identifier() {
    return "medItkImageDataNiftiWriter";
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

medItkImageDataNiftiWriter::medItkImageDataNiftiWriter(): medItkImageDataWriterBase() {
    this->io = itk::NiftiImageIO::New();
}

medItkImageDataNiftiWriter::~medItkImageDataNiftiWriter() { }

QStringList medItkImageDataNiftiWriter::handled() const {
    return s_handled();
}

bool medItkImageDataNiftiWriter::registered() {
    return medAbstractDataFactory::instance()->registerDataWriterType(s_identifier(), s_handled(), create);
}

QString medItkImageDataNiftiWriter::identifier() const {
    return s_identifier();
}

QString medItkImageDataNiftiWriter::description() const {
    return "Nifti image exporter";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter * medItkImageDataNiftiWriter::create() {
    return new medItkImageDataNiftiWriter;
}

