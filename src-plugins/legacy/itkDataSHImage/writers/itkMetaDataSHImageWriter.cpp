/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkMetaDataSHImageWriter.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkMetaImageIO.h>

static QString s_identifier() {
    return "itkMetaDataSHImageWriter";
}

itkMetaDataSHImageWriter::itkMetaDataSHImageWriter(): itkDataSHImageWriterBase() {
    this->io = itk::MetaImageIO::New();
}

itkMetaDataSHImageWriter::~itkMetaDataSHImageWriter() { }

bool itkMetaDataSHImageWriter::registered() {
  return medAbstractDataFactory::instance()->registerDataWriterType(s_identifier(), itkDataSHImageWriterBase::s_handled(), create);
}


QString itkMetaDataSHImageWriter::description() const {
    return tr("itk MetaData SH Image exporter");
}

QString itkMetaDataSHImageWriter::identifier() const {
    return s_identifier();
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter * itkMetaDataSHImageWriter::create() {
    return new itkMetaDataSHImageWriter();
}
