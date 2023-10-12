/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 All rights reserved.
 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkINRDataImageReader.h"

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <medInrimageImageIO.h>

itkINRDataImageReader::itkINRDataImageReader() :  itkDataImageReaderBase()
{
    this->io = InrimageImageIO::New();
}

bool itkINRDataImageReader::registered()
{
    return medAbstractDataFactory::instance()->registerDataReaderType("itkINRDataImageReader",s_handled(),createItkINRDataImageReader);
}

QString itkINRDataImageReader::description() const
{
    return "itkINRDataImageReader";
}

QStringList itkINRDataImageReader::handled() const {
    return s_handled();
}

QStringList itkINRDataImageReader::s_handled() {
    return QStringList()  << "itkDataImageChar3" << "itkDataImageChar4"
            << "itkDataImageUChar3" << "itkDataImageUChar4"
            << "itkDataImageShort3" << "itkDataImageShort4"
            << "itkDataImageUShort3" << "itkDataImageUShort4"
            << "itkDataImageInt3" << "itkDataImageInt4"
            << "itkDataImageUInt3" << "itkDataImageUInt4"
            << "itkDataImageLong3" << "itkDataImageLong4"
            << "itkDataImageULong3" << "itkDataImageULong4"
            << "itkDataImageFloat3" << "itkDataImageFloat4"
            << "itkDataImageDouble3" << "itkDataImageDouble4"
            << "itkDataImageRGB3" << "itkDataImageRGBA3";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkINRDataImageReader()
{
    return new itkINRDataImageReader;
}
