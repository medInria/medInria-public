/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkNiftiDataTensorImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkNiftiImageIO.h>


const char itkNiftiDataTensorImageWriter::ID[] = "itkNiftiDataTensorImageWriter";

itkNiftiDataTensorImageWriter::itkNiftiDataTensorImageWriter() : itkDataTensorImageWriterBase()
{
    this->io = itk::NiftiImageIO::New();
}


itkNiftiDataTensorImageWriter::~itkNiftiDataTensorImageWriter()
{
}


bool itkNiftiDataTensorImageWriter::registered()
{
  return dtkAbstractDataFactory::instance()->registerDataWriterType(
              "itkNiftiDataTensorImageWriter",
              itkDataTensorImageWriterBase::s_handled(),
              createitkNiftiDataTensorImageWriter);
}

QString itkNiftiDataTensorImageWriter::description() const
{
    return tr( "Nifti writer for Tensor images" );
}

QString itkNiftiDataTensorImageWriter::identifier() const
{
    return ID;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createitkNiftiDataTensorImageWriter()
{
    return new itkNiftiDataTensorImageWriter;
}

