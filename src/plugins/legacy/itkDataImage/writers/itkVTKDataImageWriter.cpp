/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkVTKDataImageWriter.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkVTKImageIO.h>

static QString s_identifier() {
    return "itkVTKDataImageWriter";
}

static QStringList s_handled() {
    return QStringList()  << "itkDataImageChar3"
                          << "itkDataImageUChar3"
                          << "itkDataImageShort3"
                          << "itkDataImageUShort3"
                          << "itkDataImageInt3"
                          << "itkDataImageUInt3"
                          << "itkDataImageLong3"
                          << "itkDataImageULong3"
                          << "itkDataImageFloat3"
                          << "itkDataImageDouble3"
                          << "itkDataImageRGB3"
                          << "itkDataImageRGBA3";
}


itkVTKDataImageWriter::itkVTKDataImageWriter(): itkDataImageWriterBase() {
    this->io = itk::VTKImageIO::New();
}

itkVTKDataImageWriter::~itkVTKDataImageWriter() { }

QStringList itkVTKDataImageWriter::handled() const {
    return s_handled();
}

QStringList itkVTKDataImageWriter::supportedFileExtensions() const
{
    return QStringList() << ".vtk";
}

bool itkVTKDataImageWriter::registered() {
    return medAbstractDataFactory::instance()->registerDataWriterType(s_identifier(), s_handled(), create);
}

QString itkVTKDataImageWriter::identifier() const {
    return s_identifier();
}

QString itkVTKDataImageWriter::description() const {
    return "VTK image exporter";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter * itkVTKDataImageWriter::create() {
    return new itkVTKDataImageWriter;
}

