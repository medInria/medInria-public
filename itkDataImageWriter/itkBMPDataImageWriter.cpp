#include "itkBMPDataImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkBMPImageIO.h>


itkBMPDataImageWriter::itkBMPDataImageWriter(void) : itkDataImageWriterBase()
{
    this->io = itk::BMPImageIO::New();
}


itkBMPDataImageWriter::~itkBMPDataImageWriter(void)
{
}

QStringList itkBMPDataImageWriter::handled(void) const
{
  return s_handled();
}

QStringList itkBMPDataImageWriter::s_handled(void)
{
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
						<< "itkDataImageRGB3";
}

bool itkBMPDataImageWriter::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataWriterType("itkBMPDataImageWriter", s_handled(),
								    createItkBMPDataImageWriter);
}


QString itkBMPDataImageWriter::description(void) const
{
    return "itkBMPDataImageWriter";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkBMPDataImageWriter(void)
{
    return new itkBMPDataImageWriter;
}

