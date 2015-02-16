/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkSHImageMetaDataWriter.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkMetaImageIO.h>

static QString s_identifier() {
    return "medItkSHImageMetaDataWriter";
}

medItkSHImageMetaDataWriter::medItkSHImageMetaDataWriter(): medItkSHImageDataWriterBase() {
    this->io = itk::MetaImageIO::New();
}

medItkSHImageMetaDataWriter::~medItkSHImageMetaDataWriter() { }

bool medItkSHImageMetaDataWriter::registered() {
  return medAbstractDataFactory::instance()->registerDataWriterType(s_identifier(), medItkSHImageDataWriterBase::s_handled(), create);
}


QString medItkSHImageMetaDataWriter::description() const {
    return tr("itk MetaData SH Image exporter");
}

QString medItkSHImageMetaDataWriter::identifier() const {
    return s_identifier();
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter * medItkSHImageMetaDataWriter::create() {
    return new medItkSHImageMetaDataWriter();
}
