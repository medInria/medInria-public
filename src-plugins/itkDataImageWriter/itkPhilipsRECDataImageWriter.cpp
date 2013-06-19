/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkPhilipsRECDataImageWriter.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkPhilipsRECImageIO.h>

const char itkPhilipsRECDataImageWriter::ID[] = "itkPhilipsRECDataImageWriter";

itkPhilipsRECDataImageWriter::itkPhilipsRECDataImageWriter(): itkDataImageWriterBase() {
    this->io = itk::PhilipsRECImageIO::New();
}

itkPhilipsRECDataImageWriter::~itkPhilipsRECDataImageWriter() { }

QStringList itkPhilipsRECDataImageWriter::handled() const {
    return s_handled();
}

QStringList itkPhilipsRECDataImageWriter::s_handled() {
    return QStringList() << "itkDataImageChar4" << "itkDataImageUChar3" << "itkDataImageUChar4" << "itkDataImageShort3" << "itkDataImageShort4" << "itkDataImageUShort3" << "itkDataImageUShort4" << "itkDataImageInt3" << "itkDataImageInt4" << "itkDataImageUInt3" << "itkDataImageUInt4" << "itkDataImageLong3" << "itkDataImageLong4" << "itkDataImageULong3" << "itkDataImageULong4" << "itkDataImageFloat3" << "itkDataImageFloat4" << "itkDataImageDouble3" << "itkDataImageDouble4";
}

bool itkPhilipsRECDataImageWriter::registered() {
    return dtkAbstractDataFactory::instance()->registerDataWriterType(ID,s_handled(),create);
}

QString itkPhilipsRECDataImageWriter::identifier() const {
    return ID;
}

QString itkPhilipsRECDataImageWriter::description() const {
    return "Writer for Philips REC images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter* itkPhilipsRECDataImageWriter::create() {
    return new itkPhilipsRECDataImageWriter;
}
