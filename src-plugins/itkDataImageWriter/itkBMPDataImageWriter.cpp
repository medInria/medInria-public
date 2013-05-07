/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkBMPDataImageWriter.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkBMPImageIO.h>

const char itkBMPDataImageWriter::ID[] = "itkBMPDataImageWriter";

itkBMPDataImageWriter::itkBMPDataImageWriter(): itkDataImageWriterBase() {
    this->io = itk::BMPImageIO::New();
}

itkBMPDataImageWriter::~itkBMPDataImageWriter() { }

QStringList itkBMPDataImageWriter::handled() const {
  return s_handled();
}

QStringList itkBMPDataImageWriter::s_handled() {
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

bool itkBMPDataImageWriter::registered() {
    return dtkAbstractDataFactory::instance()->registerDataWriterType(ID,s_handled(),createItkBMPDataImageWriter);
}

QString itkBMPDataImageWriter::identifier() const {
    return ID;
}

QString itkBMPDataImageWriter::description() const {
    return "Writer for BMP images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkBMPDataImageWriter() {
    return new itkBMPDataImageWriter;
}

