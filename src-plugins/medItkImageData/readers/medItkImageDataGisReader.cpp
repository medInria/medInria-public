/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkImageDataGisReader.h>
#include <medItkGisImageIO.h>
#include <medAbstractData.h>
#include <medAbstractDataFactory.h>


const char medItkImageDataGisReader::ID[] = "medItkImageDataGisReader";


medItkImageDataGisReader::medItkImageDataGisReader() : medItkImageDataReaderBase()
{
     this->io = itk::GISImageIO::New();
}

medItkImageDataGisReader::~medItkImageDataGisReader()
{
}

bool medItkImageDataGisReader::registered() {
    return medAbstractDataFactory::instance()->registerDataReaderType(ID,s_handled(),createmedItkImageDataGisReader);
}

QStringList medItkImageDataGisReader::handled() const {
    return s_handled();
}

QStringList medItkImageDataGisReader::s_handled() {
    return QStringList()  << "medItkChar3ImageData" << "medItkChar4ImageData"
            << "medItkUChar3ImageData" << "medItkUChar4ImageData"
            << "medItkShort3ImageData" << "medItkShort4ImageData"
            << "medItkUShort3ImageData" << "medItkUShort4ImageData"
            << "medItkInt3ImageData" << "medItkInt4ImageData"
            << "medItkUInt3ImageData" << "medItkUInt4ImageData"
            << "medItkFloat3ImageData" << "medItkFloat4ImageData"
            << "medItkDouble3ImageData" << "medItkDouble4ImageData";
}

QString medItkImageDataGisReader::identifier() const
{
    return ID;
}

QString medItkImageDataGisReader::description() const
{
    return "medItkImageDataGisReader";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createmedItkImageDataGisReader()
{
    return new medItkImageDataGisReader;
}

