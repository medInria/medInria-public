/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkImageDataNiftiReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkNiftiImageIO.h>

const char medItkImageDataNiftiReader::ID[] = "medItkImageDataNiftiReader";

medItkImageDataNiftiReader::medItkImageDataNiftiReader(): medItkImageDataReaderBase() {
    this->io = itk::NiftiImageIO::New();
}

medItkImageDataNiftiReader::~medItkImageDataNiftiReader() { }

QStringList medItkImageDataNiftiReader::handled() const {
    return s_handled();
}

QStringList medItkImageDataNiftiReader::s_handled() {
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

bool medItkImageDataNiftiReader::registered() {
    return medAbstractDataFactory::instance()->registerDataReaderType(ID,s_handled(),createmedItkImageDataNiftiReader);
}

QString medItkImageDataNiftiReader::identifier() const {
    return ID;
}

QString medItkImageDataNiftiReader::description() const {
    return "Reader for nifti images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createmedItkImageDataNiftiReader() {
    return new medItkImageDataNiftiReader;
}
