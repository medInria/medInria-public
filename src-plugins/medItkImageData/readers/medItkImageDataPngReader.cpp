/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkImageDataPngReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkPNGImageIO.h>

const char medItkImageDataPngReader::ID[] = "medItkImageDataPngReader";

medItkImageDataPngReader::medItkImageDataPngReader(): medItkImageDataReaderBase() {
    this->io = itk::PNGImageIO::New();
}

medItkImageDataPngReader::~medItkImageDataPngReader() { }

QStringList medItkImageDataPngReader::handled() const {
    return s_handled();
}

QStringList medItkImageDataPngReader::s_handled() {
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
            << "medItkRgb3ImageData" << "medItkRgb3ImageData";
}

bool medItkImageDataPngReader::registered() {
    return medAbstractDataFactory::instance()->registerDataReaderType(ID,s_handled(),createmedItkImageDataPngReader);
}

QString medItkImageDataPngReader::identifier() const {
    return ID;
}

QString medItkImageDataPngReader::description() const {
    return "Reader for PNG images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createmedItkImageDataPngReader() {
    return new medItkImageDataPngReader;
}
