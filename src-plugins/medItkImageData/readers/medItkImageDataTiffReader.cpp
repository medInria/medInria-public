/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkImageDataTiffReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkTIFFImageIO.h>

const char medItkImageDataTiffReader::ID[] = "medItkImageDataTiffReader";

medItkImageDataTiffReader::medItkImageDataTiffReader(): medItkImageDataReaderBase() {
    this->io = itk::TIFFImageIO::New();
}

medItkImageDataTiffReader::~medItkImageDataTiffReader() { }

QStringList medItkImageDataTiffReader::handled() const {
    return s_handled();
}

QStringList medItkImageDataTiffReader::s_handled() {
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

bool medItkImageDataTiffReader::registered() {
    return medAbstractDataFactory::instance()->registerDataReaderType(ID,s_handled(),createmedItkImageDataTiffReader);
}

QString medItkImageDataTiffReader::identifier() const {
    return ID;
}

QString medItkImageDataTiffReader::description() const {
    return "Reader for TIFF images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createmedItkImageDataTiffReader() {
    return new medItkImageDataTiffReader;
}

