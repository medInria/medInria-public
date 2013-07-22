/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkMetaDataSHImageWriter.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkMetaImageIO.h>

itkMetaDataSHImageWriter::itkMetaDataSHImageWriter(): itkDataSHImageWriterBase() {
    this->io = itk::MetaImageIO::New();
}

itkMetaDataSHImageWriter::~itkMetaDataSHImageWriter() { }

bool itkMetaDataSHImageWriter::registered() {
  return dtkAbstractDataFactory::instance()->
        registerDataWriterType("itkMetaDataSHImageWriter",itkDataSHImageWriterBase::s_handled(),
                                                          createItkMetaDataSHImageWriter);
}


QString itkMetaDataSHImageWriter::description() const {
    return tr("itk MetaData SH Image Writer");
}

QString itkMetaDataSHImageWriter::identifier() const {
    return "itkMetaDataSHImageWriter";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkMetaDataSHImageWriter() {
    return new itkMetaDataSHImageWriter();
}
