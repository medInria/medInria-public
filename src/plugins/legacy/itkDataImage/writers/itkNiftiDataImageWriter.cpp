/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkNiftiDataImageWriter.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkNiftiImageIO.h>

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
                          << "itkDataImageDouble3" << "itkDataImageDouble4";
}

itkNiftiDataImageWriter::itkNiftiDataImageWriter(): itkDataImageWriterBase() {
    this->io = itk::NiftiImageIO::New();
}

itkNiftiDataImageWriter::~itkNiftiDataImageWriter() { }

QStringList itkNiftiDataImageWriter::handled() const {
    return s_handled();
}

QStringList itkNiftiDataImageWriter::supportedFileExtensions() const
{
    return QStringList() << ".nii.gz" << ".nii" << ".hdr"
                         << ".nia" << ".img" << ".img.gz";
}

bool itkNiftiDataImageWriter::registered()
{
    return medAbstractDataFactory::instance()->registerDataWriterType(QString("itkNiftiDataImageWriter"), 
                                                                      s_handled(), 
                                                                      create);
}

QString itkNiftiDataImageWriter::identifier() const
{
    return "itkNiftiDataImageWriter";
}

QString itkNiftiDataImageWriter::description() const {
    return "Nifti image exporter";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter * itkNiftiDataImageWriter::create() {
    return new itkNiftiDataImageWriter;
}

