/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkVTKDataImageWriter.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkVTKImageIO.h>

static QString s_identifier() {
    return "itkVTKDataImageWriter";
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
    return dtkAbstractDataFactory::instance()->registerDataWriterType(s_identifier(), s_handled(), create);
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

