/*=========================================================================

 MedInria

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


const char itkNrrdDataTensorImageWriter::ID[] = "itkNrrdDataTensorImageWriter";

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
              "itkNrrdDataTensorImageWriter",
              itkDataTensorImageWriterBase::s_handled(),
              createitkNrrdDataTensorImageWriter);
}

QString itkNrrdDataTensorImageWriter::description() const
{
    return tr( "Nrrd writer for Tensor images" );
}

QString itkNrrdDataTensorImageWriter::identifier() const
{
    return ID;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createitkNrrdDataTensorImageWriter()
{
    return new itkNrrdDataTensorImageWriter;
}

