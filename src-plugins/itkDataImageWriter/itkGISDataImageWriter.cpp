/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkGISDataImageWriter.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkGISImageIO.h>

const char itkGISDataImageWriter::ID[] = "itkGISDataImageWriter";

itkGISDataImageWriter::itkGISDataImageWriter(): itkDataImageWriterBase() {
    this->io = itk::GISImageIO::New();
}

itkGISDataImageWriter::~itkGISDataImageWriter() { }

QStringList itkGISDataImageWriter::handled() const {
    return s_handled();
}

QStringList itkGISDataImageWriter::s_handled() {
    return QStringList()  << "itkDataImageChar3" << "itkDataImageChar4"
            << "itkDataImageUChar3" << "itkDataImageUChar4"
            << "itkDataImageShort3" << "itkDataImageShort4"
            << "itkDataImageUShort3" << "itkDataImageUShort4"
            << "itkDataImageInt3" << "itkDataImageInt4"
            << "itkDataImageUInt3" << "itkDataImageUInt4"
            << "itkDataImageFloat3" << "itkDataImageFloat4"
            << "itkDataImageDouble3" << "itkDataImageDouble4";
}

bool itkGISDataImageWriter::registered() {
    return dtkAbstractDataFactory::instance()->registerDataWriterType(ID,s_handled(),createItkGISDataImageWriter);
}

QString itkGISDataImageWriter::identifier() const {
    return ID;
}

QString itkGISDataImageWriter::description() const {
    return "Writer for GIS images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkGISDataImageWriter() {
    return new itkGISDataImageWriter;
}

