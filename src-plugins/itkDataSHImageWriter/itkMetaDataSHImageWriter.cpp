#include "itkMetaDataSHImageWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkMetaImageIO.h>


itkMetaDataSHImageWriter::itkMetaDataSHImageWriter(void) : itkDataSHImageWriterBase()
{
    this->io = itk::MetaImageIO::New();
}


itkMetaDataSHImageWriter::~itkMetaDataSHImageWriter(void)
{
}


bool itkMetaDataSHImageWriter::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataWriterType("itkMetaDataSHImageWriter", itkDataSHImageWriterBase::s_handled(),
                                                                    createItkMetaDataSHImageWriter);
}


QString itkMetaDataSHImageWriter::description(void) const
{
    return tr("itk MetaData SH Image Writer");
}

QString itkMetaDataSHImageWriter::identifier(void) const
{
    return "itkMetaDataSHImageWriter";
}
\
// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkMetaDataSHImageWriter(void)
{
    return new itkMetaDataSHImageWriter;
}

