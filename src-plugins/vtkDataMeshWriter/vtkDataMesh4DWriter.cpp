/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkDataMesh4DWriter.h"

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkLog/dtkLog.h>

#include <vtkDataManagerWriter.h>
#include <vtkDataManager.h>
#include <vtkMetaDataSetSequence.h>


const char vtkDataMesh4DWriter::ID[] = "vtkDataMesh4DWriter";

vtkDataMesh4DWriter::vtkDataMesh4DWriter()
{
  this->writer = vtkDataManagerWriter::New();
}

vtkDataMesh4DWriter::~vtkDataMesh4DWriter()
{
  this->writer->Delete();
}

QStringList vtkDataMesh4DWriter::handled() const
{
    return QStringList() << "vtkDataMesh4D";
}

QStringList vtkDataMesh4DWriter::s_handled()
{
    return QStringList() << "vtkDataMesh4D";
}

bool vtkDataMesh4DWriter::canWrite(const QString& path)
{
    if ( ! this->data())
        return false;

    return dynamic_cast<vtkMetaDataSetSequence*>((vtkObject*)(this->data()->data()));
}

bool vtkDataMesh4DWriter::write(const QString& path)
{
  if (!this->data())
    return false;

  qDebug() << "Can write with: " << this->identifier();

  dtkAbstractData *dtkdata = this->data();

  if(dtkdata->identifier()!="vtkDataMesh4D")
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

QString vtkDataMesh4DWriter::description() const
{
    return tr( "VTK 4D Mesh Writer" );
}

QString vtkDataMesh4DWriter::identifier() const
{
    return ID;
}

bool vtkDataMesh4DWriter::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataWriterType("vtkDataMesh4DWriter", vtkDataMesh4DWriter::s_handled(), createVtkDataMesh4DWriter);
}

QStringList vtkDataMesh4DWriter::supportedFileExtensions() const
{
    return QStringList() << ".xml";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createVtkDataMesh4DWriter()
{
  return new vtkDataMesh4DWriter;
}


