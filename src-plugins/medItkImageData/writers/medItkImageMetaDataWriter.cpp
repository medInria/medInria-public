/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkImageMetaDataWriter.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkMetaImageIO.h>

QString s_identifier() {
    return "medItkImageMetaDataWriter";
}

QStringList s_handled() {
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
                          << "medItkVectorUChar3ImageData" << "medItkVectorFloat3ImageData"
                          << "medItkVectorDouble3ImageData"
                          << "medItkRgb3ImageData" << "medItkRgba3ImageData";
}

medItkImageMetaDataWriter::medItkImageMetaDataWriter(): medItkImageDataWriterBase() {
    this->io = itk::MetaImageIO::New();
}

medItkImageMetaDataWriter::~medItkImageMetaDataWriter() { }

QStringList medItkImageMetaDataWriter::handled() const {
    return s_handled();
}

bool medItkImageMetaDataWriter::registered() {
    return medAbstractDataFactory::instance()->registerDataWriterType(s_identifier(), s_handled(), create);
}

QString medItkImageMetaDataWriter::identifier() const {
    return s_identifier();
}

QString medItkImageMetaDataWriter::description() const {
    return "ITK MetaData image exporter";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter * medItkImageMetaDataWriter::create() {
    return new medItkImageMetaDataWriter;
}

