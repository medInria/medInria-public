/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtk4DMeshDataWriter.h>

#include <medAbstractDataFactory.h>
#include <medAbstractData.h>
#include <dtkLog/dtkLog.h>

#include <vtkDataManagerWriter.h>
#include <vtkDataManager.h>
#include <vtkMetaDataSetSequence.h>


const char medVtk4DMeshDataWriter::ID[] = "medVtk4DMeshDataWriter";

medVtk4DMeshDataWriter::medVtk4DMeshDataWriter()
{
  this->writer = vtkDataManagerWriter::New();
}

medVtk4DMeshDataWriter::~medVtk4DMeshDataWriter()
{
  this->writer->Delete();
}

QStringList medVtk4DMeshDataWriter::handled() const
{
    return QStringList() << "medVtk4DMeshData";
}

QStringList medVtk4DMeshDataWriter::s_handled()
{
    return QStringList() << "medVtk4DMeshData";
}

bool medVtk4DMeshDataWriter::canWrite(const QString& path)
{
    if ( ! this->data())
        return false;

    return dynamic_cast<vtkMetaDataSetSequence*>((vtkObject*)(this->data()->data()));
}

bool medVtk4DMeshDataWriter::write(const QString& path)
{
  if (!this->data())
    return false;

  qDebug() << "Can write with: " << this->identifier();

  medAbstractData *medData = dynamic_cast<medAbstractData*>(this->data());

  if(medData->identifier()!="medVtk4DMeshData")
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

QString medVtk4DMeshDataWriter::description() const
{
    return tr( "VTK 4D Mesh Writer" );
}

QString medVtk4DMeshDataWriter::identifier() const
{
    return ID;
}

bool medVtk4DMeshDataWriter::registered()
{
    return medAbstractDataFactory::instance()->registerDataWriterType("medVtk4DMeshDataWriter", medVtk4DMeshDataWriter::s_handled(), createmedVtk4DMeshDataWriter);
}

QStringList medVtk4DMeshDataWriter::supportedFileExtensions() const
{
    return QStringList() << ".xml";
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createmedVtk4DMeshDataWriter()
{
  return new medVtk4DMeshDataWriter;
}


