/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkImageDataJpegReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkJPEGImageIO.h>

const char medItkImageDataJpegReader::ID[] = "medItkImageDataJpegReader";

medItkImageDataJpegReader::medItkImageDataJpegReader(): medItkImageDataReaderBase() {
    this->io = itk::JPEGImageIO::New();
}

medItkImageDataJpegReader::~medItkImageDataJpegReader() { }

QStringList medItkImageDataJpegReader::handled() const {
    return s_handled();
}

QStringList medItkImageDataJpegReader::s_handled() {
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

bool medItkImageDataJpegReader::registered() {
    return medAbstractDataFactory::instance()->registerDataReaderType(ID,s_handled(),createmedItkImageDataJpegReader);
}

QString medItkImageDataJpegReader::identifier() const {
    return ID;
}

QString medItkImageDataJpegReader::description() const {
    return "Reader for JPEG images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createmedItkImageDataJpegReader() {
    return new medItkImageDataJpegReader;
}
