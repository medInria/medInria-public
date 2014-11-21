/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkSHImageDataNrrdReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkNrrdImageIO.h>

medItkSHImageDataNrrdReader::medItkSHImageDataNrrdReader() : medItkSHImageDataReaderBase()
{
    this->io = itk::NrrdImageIO::New();
}

medItkSHImageDataNrrdReader::~medItkSHImageDataNrrdReader()
{
}

bool medItkSHImageDataNrrdReader::registered()
{
  return medAbstractDataFactory::instance()->registerDataReaderType("medItkSHImageDataNrrdReader", medItkSHImageDataReaderBase::s_handled(),
                                                                    createmedItkSHImageDataNrrdReader);
}

QString medItkSHImageDataNrrdReader::description() const
{
    return tr("itk Nrrd SH reader");
}

QString medItkSHImageDataNrrdReader::identifier() const
{
    return "medItkSHImageDataNrrdReader";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createmedItkSHImageDataNrrdReader()
{
    return new medItkSHImageDataNrrdReader;
}

