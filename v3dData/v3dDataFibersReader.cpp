#include "v3dDataFibersReader.h"

#include "dtkCore/dtkAbstractDataFactory.h"

class v3dDataFibersReaderPrivate
{
public:
};


v3dDataFibersReader::v3dDataFibersReader(): d (new v3dDataFibersReaderPrivate)
{
}

v3dDataFibersReader::~v3dDataFibersReader()
{
}

QStringList v3dDataFibersReader::handled(void) const
{
  return QStringList() << "v3dDataFibers";
}

bool v3dDataFibersReader::canRead (const QString& path)
{
  return false;
}

bool v3dDataFibersReader::canRead (const QStringList& paths)
{
  return false;
}

void v3dDataFibersReader::readInformation (const QString& path)
{}

void v3dDataFibersReader::readInformation (const QStringList& path)
{}

void readInformation (const QStringList& paths)
{}
  
bool v3dDataFibersReader::read (const QString& path)
{
  return false;
}

bool v3dDataFibersReader::read (const QStringList& paths)
{
  return false;
}
  
void v3dDataFibersReader::setProgress (int value)
{}

QString v3dDataFibersReader::description(void) const
{
  return "v3dDataFibersReader";
}

bool v3dDataFibersReader::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataReaderType("v3dDataFibersReader",
								    QStringList() << "v3dDataFibers",
								    createV3dDataFibersReader);
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createV3dDataFibersReader(void)
{
  return new v3dDataFibersReader;
}

