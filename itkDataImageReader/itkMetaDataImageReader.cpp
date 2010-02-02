#include "medItk.h"

#include "itkMetaDataImageReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkMetaImageIO.h>


itkMetaDataImageReader::itkMetaDataImageReader(void) : itkDataImageReaderBase()
{
	this->io = itk::MetaImageIO::New();
}


itkMetaDataImageReader::~itkMetaDataImageReader(void)
{
}


bool itkMetaDataImageReader::registered(void)
{
    return dtkAbstractDataFactory::instance()->registerDataReaderType("itkMetaDataImageReader", QStringList() << "itkDataImageDouble3"
																	  << "itkDataImageFloat3"
																	  << "itkDataImageULong3"
																	  << "itkDataImageLong3"
																	  << "itkDataImageUInt3"
																	  << "itkDataImageInt3"
																	  << "itkDataImageUShort3"
																	  << "itkDataImageShort3"
																	  << "itkDataImageShort4"																	  
																	  << "itkDataImageUChar3"
																	  << "itkDataImageChar3"
																	  << "itkDataImageRGB3",
																	  createItkMetaDataImageReader);
}


QStringList itkMetaDataImageReader::handled(void) const
{
    return QStringList() << "itkDataImageDouble3"
	<< "itkDataImageFloat3"
	<< "itkDataImageULong3"
	<< "itkDataImageLong3"
	<< "itkDataImageUInt3"
	<< "itkDataImageInt3"
	<< "itkDataImageUShort3"
	<< "itkDataImageShort3"
	<< "itkDataImageShort4"	
	<< "itkDataImageUChar3"
	<< "itkDataImageChar3"
	<< "itkDataImageRGB3";
}


QString itkMetaDataImageReader::description(void) const
{
    return "itkMetaDataImageReader";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkMetaDataImageReader(void)
{
    return new itkMetaDataImageReader;
}

