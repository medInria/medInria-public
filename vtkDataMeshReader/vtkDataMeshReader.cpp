#include "vtkDataMeshReader.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

vtkDataMeshReader::vtkDataMeshReader(void) : dtkAbstractDataReader()
{
  this->io = 0;
}


vtkDataMeshReader::~vtkDataMeshReader(void)
{
}


QStringList vtkDataMeshReader::handled(void) const
{
  return QStringList() << "vtkDataMesh";
}

QStringList vtkDataMeshReader::s_handled(void)
{
  return QStringList() << "vtkDataMesh";
}

bool vtkDataMeshReader::canRead (QString path)
{
  this->reader->SetFileName (path.toAscii().constData());
  
  if (this->reader->IsFilePolyData() ||
      this->reader->IsFileUnstructuredGrid() ||
      this->reader->IsFileStructuredGrid()   ||
      this->reader->IsFileRectilinearGrid()   )
  {
    return true;
  }
  return false;
}

bool vtkDataMeshReader::canRead (QStringList paths)
{
  if (!paths.count())
    return false;
  return this->canRead ( paths[0].toAscii().constData() );
}

void vtkDataMeshReader::readInformation (QString path)
{
	
  dtkAbstractData* dtkdata = this->data();
  this->reader->SetFileName (path.toAscii().constData());
  
  if (!dtkdata)
  {
  }
  else
  {
    dtkdata->addMetaData ("FilePath", QStringList() << path);
    dtkdata->description() = "vtkDataMesh";
  }
}
  

void vtkDataMeshReader::readInformation (QStringList paths)
{
  if (!paths.count())
    return;
  this->readInformation ( paths[0].toAscii().constData() );
}

bool vtkDataMeshReader::read (QString path)
{
  this->setProgress (0);
	
  this->readInformation ( path );

  this->setProgress (50);
  
  qDebug() << "Can read with: " << this->description();
  
  if (dtkAbstractData *dtkdata = this->data() )
  {
    
    if (!dtkdata->description()=="vtkDataMesh")
      return false;

    reader->SetFileName (path.toAscii().constData());
    reader->Update();
    dtkdata->setData (reader->GetOutput() );
  }

  this->setProgress (100);
  return true;
  
}

bool vtkDataMeshReader::read (QStringList paths)
{
  if (!paths.count())
    return false;
  return this->read ( paths[0].toAscii().constData() );
}

void vtkDataMeshReader::setProgress (int value)
{
  emit progressed (value);
}
