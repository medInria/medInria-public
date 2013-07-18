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

QString itkDataSHImageWriterPlugin::contact() const {
    return QString::fromUtf8("Théo Papadopoulo");
}

QStringList itkDataSHImageWriterPlugin::authors() const {
    QStringList list;
    list << QString::fromUtf8("Jaime Garcia Guevara");
    return list;
}

QStringList itkDataSHImageWriterPlugin::contributors() const {
    QStringList list;
    list << QString::fromUtf8("Théo Papadopoulo");
    return list;
}

QString itkDataSHImageWriterPlugin::version() const {
    return ITKDATASHIMAGEWRITEADERPLUGIN_VERSION;
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
