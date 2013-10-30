/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkNrrdDataTensorImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkNrrdImageIO.h>


static QString s_identifier() {
    return "itkNrrdDataTensorImageWriter";
}

itkNrrdDataTensorImageWriter::itkNrrdDataTensorImageWriter() : itkDataTensorImageWriterBase()
{
    this->io = itk::NrrdImageIO::New();
}


itkNrrdDataTensorImageWriter::~itkNrrdDataTensorImageWriter()
{
}


bool itkNrrdDataTensorImageWriter::registered()
{
  return dtkAbstractDataFactory::instance()->registerDataWriterType(
              s_identifier(),
              itkDataTensorImageWriterBase::s_handled(),
              create);
}

QString itkNrrdDataTensorImageWriter::description() const
{
    return tr( "Tensor images Nrrd exporter" );
}

QString itkNrrdDataTensorImageWriter::identifier() const
{
    return s_identifier();
}

QStringList itkNrrdDataTensorImageWriter::supportedFileExtensions() const
{
    return QStringList() << ".nrrd" << ".nhdr";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter * itkNrrdDataTensorImageWriter::create()
{
    return new itkNrrdDataTensorImageWriter;
}

