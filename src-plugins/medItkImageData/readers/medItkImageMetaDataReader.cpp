/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkImageMetaDataReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkMetaImageIO.h>

const char medItkImageMetaDataReader::ID[] = "medItkImageMetaDataReader";

medItkImageMetaDataReader::medItkImageMetaDataReader(): medItkImageDataReaderBase()
{
    this->io = itk::MetaImageIO::New();
}

medItkImageMetaDataReader::~medItkImageMetaDataReader() { }

QStringList medItkImageMetaDataReader::handled() const {
    return s_handled();
}

QStringList medItkImageMetaDataReader::s_handled()
{
    return QStringList()
            << "medItkChar3ImageData" << "medItkChar4ImageData"
            << "medItkUChar3ImageData" << "medItkUChar4ImageData"
            << "medItkShort3ImageData" << "medItkShort4ImageData"
            << "medItkUShort3ImageData" << "medItkUShort4ImageData"
            << "medItkInt3ImageData" << "medItkInt4ImageData"
            << "medItkUInt3ImageData" << "medItkUInt4ImageData"
            << "medItkLong3ImageData" << "medItkLong4ImageData"
            << "medItkULong3ImageData" << "medItkULong4ImageData"
            << "medItkFloat3ImageData" << "medItkFloat4ImageData"
            << "medItkDouble3ImageData" << "medItkDouble4ImageData"
            << "medItkRgb3ImageData" << "medItkRgba3ImageData";
}

bool medItkImageMetaDataReader::registered() {
    return medAbstractDataFactory::instance()->registerDataReaderType(ID,s_handled(),createmedItkImageMetaDataReader);
}

QString medItkImageMetaDataReader::identifier() const {
    return ID;
}

QString medItkImageMetaDataReader::description() const {
    return "Reader for itk meta data";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createmedItkImageMetaDataReader() {
    return new medItkImageMetaDataReader;
}
