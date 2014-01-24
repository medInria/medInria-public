/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkGISDataImageReader.h"
#include "itkGISImageIO.h"
#include <medAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>


const char itkGISDataImageReader::ID[] = "itkGISDataImageReader";


itkGISDataImageReader::itkGISDataImageReader() : itkDataImageReaderBase()
{
     this->io = itk::GISImageIO::New();
}

itkGISDataImageReader::~itkGISDataImageReader()
{
}

bool itkGISDataImageReader::registered() {
    return dtkAbstractDataFactory::instance()->registerDataReaderType(ID,s_handled(),createItkGISDataImageReader);
}

QStringList itkGISDataImageReader::handled() const {
    return s_handled();
}

QStringList itkGISDataImageReader::s_handled() {
    return QStringList()  << "itkDataImageChar3" << "itkDataImageChar4"
            << "itkDataImageUChar3" << "itkDataImageUChar4"
            << "itkDataImageShort3" << "itkDataImageShort4"
            << "itkDataImageUShort3" << "itkDataImageUShort4"
            << "itkDataImageInt3" << "itkDataImageInt4"
            << "itkDataImageUInt3" << "itkDataImageUInt4"
            << "itkDataImageFloat3" << "itkDataImageFloat4"
            << "itkDataImageDouble3" << "itkDataImageDouble4";
}

QString itkGISDataImageReader::identifier() const
{
    return ID;
}

QString itkGISDataImageReader::description() const 
{
    return "itkGISDataImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkGISDataImageReader()
{
    return new itkGISDataImageReader;
}

