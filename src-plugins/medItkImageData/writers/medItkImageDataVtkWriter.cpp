/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkImageDataVtkWriter.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkVTKImageIO.h>

static QString s_identifier() {
    return "medItkImageDataVtkWriter";
}

static QStringList s_handled() {
    return QStringList()  << "medItkChar3ImageData"
                          << "medItkUChar3ImageData"
                          << "medItkShort3ImageData"
                          << "medItkUShort3ImageData"
                          << "medItkInt3ImageData"
                          << "medItkUInt3ImageData"
                          << "medItkLong3ImageData"
                          << "medItkULong3ImageData"
                          << "medItkFloat3ImageData"
                          << "medItkDouble3ImageData"
                          << "medItkRgb3ImageData"
                          << "medItkRgba3ImageData";
}


medItkImageDataVtkWriter::medItkImageDataVtkWriter(): medItkImageDataWriterBase() {
    this->io = itk::VTKImageIO::New();
}

medItkImageDataVtkWriter::~medItkImageDataVtkWriter() { }

QStringList medItkImageDataVtkWriter::handled() const {
    return s_handled();
}

QStringList medItkImageDataVtkWriter::supportedFileExtensions() const
{
    return QStringList() << ".vtk";
}

bool medItkImageDataVtkWriter::registered() {
    return medAbstractDataFactory::instance()->registerDataWriterType(s_identifier(), s_handled(), create);
}

QString medItkImageDataVtkWriter::identifier() const {
    return s_identifier();
}

QString medItkImageDataVtkWriter::description() const {
    return "VTK image exporter";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter * medItkImageDataVtkWriter::create() {
    return new medItkImageDataVtkWriter;
}

