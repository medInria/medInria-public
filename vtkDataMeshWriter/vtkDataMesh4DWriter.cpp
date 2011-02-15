#include "vtkDataMesh4DWriter.h"

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkLog.h>

#include "vtkDataManagerWriter.h"
#include "vtkDataManager.h"
#include "vtkMetaDataSetSequence.h"


vtkDataMesh4DWriter::vtkDataMesh4DWriter()
{
  this->writer = vtkDataManagerWriter::New();
}

vtkDataMesh4DWriter::~vtkDataMesh4DWriter()
{
  this->writer->Delete();
}

QStringList vtkDataMesh4DWriter::handled(void) const
{
    return QStringList() << "vtkDataMesh4D";
}

QStringList vtkDataMesh4DWriter::s_handled(void)
{
    return QStringList() << "vtkDataMesh4D";
}

bool vtkDataMesh4DWriter::canWrite(const QString& path)
{
  return path.endsWith (QString (".vtk"));
}

bool vtkDataMesh4DWriter::write(const QString& path)
{
  if (!this->data())
    return false;
  
  qDebug() << "Can write with: " << this->description();
  
  dtkAbstractData *dtkdata = this->data();

  if(dtkdata->description()!="vtkDataMesh4D")
  {
    return false;
  }
  
  vtkMetaDataSetSequence* sequence = dynamic_cast< vtkMetaDataSetSequence* >( (vtkObject*)(this->data()->output()));
  if (!sequence)
    return false;

  vtkDataManager* manager = vtkDataManager::New();
  manager->AddMetaDataSet (sequence);
  
  this->writer->SetFileName(path.toAscii().constData());
  this->writer->SetInput (manager);
  // this->writer->SetFileTypeToBinary();
  this->writer->Update();

  manager->Delete();
  
  return true;
}

QString vtkDataMesh4DWriter::description(void) const
{
    return "vtkDataMesh4DWriter";
}


bool vtkDataMesh4DWriter::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataWriterType("vtkDataMesh4DWriter", vtkDataMesh4DWriter::s_handled(), createVtkDataMesh4DWriter);
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createVtkDataMesh4DWriter(void)
{
  return new vtkDataMesh4DWriter;
}


