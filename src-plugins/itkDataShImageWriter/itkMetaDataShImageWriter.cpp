#include "itkMetaDataShImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkMetaImageIO.h>


itkMetaDataShImageWriter::itkMetaDataShImageWriter(void) : itkDataShImageWriterBase()
{
    this->io = itk::MetaImageIO::New();
}


itkMetaDataShImageWriter::~itkMetaDataShImageWriter(void)
{
}


bool itkMetaDataShImageWriter::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataWriterType("itkMetaDataShImageWriter", itkDataShImageWriterBase::s_handled(),
                                                                    createItkMetaDataShImageWriter);
}


QString itkMetaDataShImageWriter::description(void) const
{
    return tr("itk MetaData Sh Image Writer");
}

QString itkMetaDataShImageWriter::identifier(void) const
{
    return "itkMetaDataShImageWriter";
}
\
// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkMetaDataShImageWriter(void)
{
    return new itkMetaDataShImageWriter;
}

