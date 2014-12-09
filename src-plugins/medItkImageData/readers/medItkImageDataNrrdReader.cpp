/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkImageDataNrrdReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkNrrdImageIO.h>

const char medItkImageDataNrrdReader::ID[] = "medItkImageDataNrrdReader";

medItkImageDataNrrdReader::medItkImageDataNrrdReader(): medItkImageDataReaderBase() {
    this->io = itk::NrrdImageIO::New();
}

medItkImageDataNrrdReader::~medItkImageDataNrrdReader() { }

QStringList medItkImageDataNrrdReader::handled() const {
    return s_handled();
}

QStringList medItkImageDataNrrdReader::s_handled() {
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

bool medItkImageDataNrrdReader::registered() {
    return medAbstractDataFactory::instance()->registerDataReaderType(ID,s_handled(),createmedItkImageDataNrrdReader);
}

QString medItkImageDataNrrdReader::identifier() const {
    return ID;
}

QString medItkImageDataNrrdReader::description() const {
    return "Reader for itk Nrrd images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createmedItkImageDataNrrdReader() {
    return new medItkImageDataNrrdReader;
}

