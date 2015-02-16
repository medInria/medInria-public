/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkImageDataGiplReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkGiplImageIO.h>

const char medItkImageDataGiplReader::ID[] = "medItkImageDataGiplReader";

medItkImageDataGiplReader::medItkImageDataGiplReader(): medItkImageDataReaderBase() {
    this->io = itk::GiplImageIO::New();
}

medItkImageDataGiplReader::~medItkImageDataGiplReader() { }

QStringList medItkImageDataGiplReader::handled() const {
    return s_handled();
}

QStringList medItkImageDataGiplReader::s_handled() {
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

bool medItkImageDataGiplReader::registered() {
    return medAbstractDataFactory::instance()->registerDataReaderType(ID,s_handled(),createmedItkImageDataGiplReader);
}

QString medItkImageDataGiplReader::identifier() const {
    return ID;
}

QString medItkImageDataGiplReader::description() const {
    return "Reader for Gipl images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createmedItkImageDataGiplReader() {
    return new medItkImageDataGiplReader;
}

