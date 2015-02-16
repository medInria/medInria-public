/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkImageDataVtkReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkVTKImageIO.h>

const char medItkImageDataVtkReader::ID[] = "medItkImageDataVtkReader";

medItkImageDataVtkReader::medItkImageDataVtkReader() : medItkImageDataReaderBase()
{
    this->io = itk::VTKImageIO::New();
}


medItkImageDataVtkReader::~medItkImageDataVtkReader()
{
}

QStringList medItkImageDataVtkReader::handled() const
{
    return s_handled();
}

QStringList medItkImageDataVtkReader::s_handled()
{
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

bool medItkImageDataVtkReader::registered()
{
    return medAbstractDataFactory::instance()->registerDataReaderType(ID, s_handled(),
                                                                      createmedItkImageDataVtkReader);
}

QString medItkImageDataVtkReader::identifier() const
{
    return ID;
}

QString medItkImageDataVtkReader::description() const
{
    return "Reader for VTK images";
}



// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createmedItkImageDataVtkReader()
{
    return new medItkImageDataVtkReader;
}

