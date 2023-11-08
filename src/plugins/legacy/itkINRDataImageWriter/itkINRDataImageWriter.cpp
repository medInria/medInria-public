/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 All rights reserved.
 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkINRDataImageWriter.h"

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <medInrimageImageIO.h>

// /////////////////////////////////////////////////////////////////
// itkINRDataImageWriter
// /////////////////////////////////////////////////////////////////

static QString s_identifier() {
    return "itkINRDataImageWriter";
}

static QStringList s_handled() {
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

itkINRDataImageWriter::itkINRDataImageWriter(void) :  itkDataImageWriterBase()
{
    this->io = InrimageImageIO::New();
}

itkINRDataImageWriter::~itkINRDataImageWriter(void)
{
}

bool itkINRDataImageWriter::registered(void)
{
    return medAbstractDataFactory::instance()->registerDataWriterType(s_identifier(), s_handled(), create);
}

QString itkINRDataImageWriter::description(void) const
{
    return "Inrimage image exporter";
}

QStringList itkINRDataImageWriter::handled() const {
    return s_handled();
}

QStringList itkINRDataImageWriter::supportedFileExtensions() const
{
    return QStringList() << ".inr.gz";
}

QString itkINRDataImageWriter::identifier() const {
    return s_identifier();
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter * itkINRDataImageWriter::create(void)
{
    return new itkINRDataImageWriter;
}
