/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkNiftiDataTensorImageWriter.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkNiftiImageIO.h>


static QString s_identifier() {
    return "itkNiftiDataTensorImageWriter";
}

itkNiftiDataTensorImageWriter::itkNiftiDataTensorImageWriter() : itkDataTensorImageWriterBase()
{
    this->io = itk::NiftiImageIO::New();
}


itkNiftiDataTensorImageWriter::~itkNiftiDataTensorImageWriter()
{
}


bool itkNiftiDataTensorImageWriter::registered()
{
  return medAbstractDataFactory::instance()->registerDataWriterType(s_identifier(), s_handled(), create);
}

QString itkNiftiDataTensorImageWriter::description() const
{
    return tr( "Tensor image Nifti exporter" );
}

QString itkNiftiDataTensorImageWriter::identifier() const
{
    return s_identifier();
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter * itkNiftiDataTensorImageWriter::create()
{
    return new itkNiftiDataTensorImageWriter;
}

