#include "vtkDataMesh4DReader.h"
#include "vtkDataManagerReader.h"
#include "vtkDataManager.h"
#include "vtkMetaDataSetSequence.h"
#include "vtkSmartPointer.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkSmartPointer.h>

vtkDataMesh4DReader::vtkDataMesh4DReader(void) : dtkAbstractDataReader()
{
  this->reader = vtkDataManagerReader::New();
}


vtkDataMesh4DReader::~vtkDataMesh4DReader(void)
{
  this->reader->Delete();
}


QStringList vtkDataMesh4DReader::handled(void) const
{
  return QStringList() << "vtkDataMesh4D";
}

QStringList vtkDataMesh4DReader::s_handled(void)
{
  return QStringList() << "vtkDataMesh4D";
}

bool vtkDataMesh4DReader::canRead (const QString& path)
{
  if (this->reader->CanReadFile (path.toAscii().constData()))
  {
    return true;
  }
  return false;
}

bool vtkDataMesh4DReader::canRead (const QStringList& paths)
{
  if (!paths.count())
    return false;
  return this->canRead ( paths[0].toAscii().constData() );
}

void vtkDataMesh4DReader::readInformation (const QString& path)
{
  
  dtkSmartPointer<dtkAbstractData> dtkdata = this->data();
  this->reader->SetFileName (path.toAscii().constData());
  
  if (!dtkdata)
  {
    dtkdata = dtkAbstractDataFactory::instance()->createSmartPointer ("vtkDataMesh4D");
    if (dtkdata)
      this->setData ( dtkdata );
  }


  dtkdata->addMetaData ("FilePath", QStringList() << path);
  dtkdata->identifier() = "vtkDataMesh4D";
}

void vtkDataMesh4DReader::readInformation (const QStringList& paths)
{
  if (!paths.count())
    return;
  this->readInformation ( paths[0].toAscii().constData() );
}

bool vtkDataMesh4DReader::read (const QString& path)
{
  this->setProgress (0);
  
  this->readInformation ( path );

  this->setProgress (50);
  
  qDebug() << "Can read with: " << this->description();
  
  if (dtkAbstractData *dtkdata = this->data() )
  {
    
    if (!(dtkdata->identifier()=="vtkDataMesh4D"))
      return false;

    this->reader->SetFileName (path.toAscii().constData());
    this->reader->Update();
    vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::SafeDownCast (this->reader->GetOutput()->GetMetaDataSet ((unsigned int)0));
    if (sequence)
      dtkdata->setData (sequence );
  }

  this->setProgress (100);
  return true;
  
}

bool vtkDataMesh4DReader::read (const QStringList& paths)
{
  if (!paths.count())
    return false;
  return this->read ( paths[0].toAscii().constData() );
}

void vtkDataMesh4DReader::setProgress (int value)
{
  emit progressed (value);
}

bool vtkDataMesh4DReader::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataReaderType(
          "vtkDataMesh4DReader",
          vtkDataMesh4DReader::s_handled(),
          createVtkDataMesh4DReader);
}

QString vtkDataMesh4DReader::description(void) const
{
    return "vtkDataMesh4DReader";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createVtkDataMesh4DReader(void)
{
  return new vtkDataMesh4DReader;
}

