/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkImageDataBmpReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkBMPImageIO.h>

const char medItkImageDataBmpReader::ID[] = "medItkImageDataBmpReader";

medItkImageDataBmpReader::medItkImageDataBmpReader(): medItkImageDataReaderBase() {
    this->io = itk::BMPImageIO::New();
}

medItkImageDataBmpReader::~medItkImageDataBmpReader() { }

QStringList medItkImageDataBmpReader::handled() const {
    return s_handled();
}

QStringList medItkImageDataBmpReader::s_handled() {
    return QStringList()  << "medItkChar3ImageData" << "medItkChar4ImageData"
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

bool medItkImageDataBmpReader::registered() {
    return medAbstractDataFactory::instance()->registerDataReaderType(ID,s_handled(),createmedItkImageDataBmpReader);
}

QString medItkImageDataBmpReader::identifier() const {
    return ID;
}

QString medItkImageDataBmpReader::description() const {
    return "Reader for BMP images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createmedItkImageDataBmpReader() {
    return new medItkImageDataBmpReader;
}
