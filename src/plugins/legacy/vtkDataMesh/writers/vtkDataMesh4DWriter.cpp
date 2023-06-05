/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkDataMesh4DWriter.h"

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <vtkDataManager.h>
#include <vtkDataManagerWriter.h>
#include <vtkMetaDataSetSequence.h>

const char vtkDataMesh4DWriter::ID[] = "vtkDataMesh4DWriter";

vtkDataMesh4DWriter::vtkDataMesh4DWriter() : vtkDataMeshWriterBase()
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

bool vtkDataMesh4DWriter::write(const QString& path)
{ 
    if (!this->data())
    {
        return false;
    }

    qDebug().noquote() << "Can write with: " << this->identifier();

    medAbstractData *medData = dynamic_cast<medAbstractData*>(this->data());

    if(medData->identifier()!="vtkDataMesh4D")
    {
        return false;
    }

    vtkMetaDataSetSequence* sequence = dynamic_cast< vtkMetaDataSetSequence* >( (vtkObject*)(this->data()->output()));
    if (!sequence)
    {
        return false;
    }

    for(vtkMetaDataSet* dataSet : sequence->GetMetaDataSetList())
    {
        addMetaDataAsFieldData(dataSet);
    }

    vtkDataManager* manager = vtkDataManager::New();
    manager->AddMetaDataSet (sequence);

    this->writer->SetFileName(path.toUtf8().constData());
    this->writer->SetInput (manager);
    this->writer->Update();

    for(vtkMetaDataSet* dataSet : sequence->GetMetaDataSetList())
    {
        clearMetaDataFieldData(dataSet);
    }

    manager->Delete();

    return true;
}

QString vtkDataMesh4DWriter::description() const
{
    return tr( "VTK with metadata 4D Mesh Writer" );
}

QString vtkDataMesh4DWriter::identifier() const
{
    return ID;
}

bool vtkDataMesh4DWriter::registered()
{
    return medAbstractDataFactory::instance()->registerDataWriterType(QString("vtkDataMesh4DWriter"), 
                                                                      vtkDataMesh4DWriter::s_handled(), 
                                                                      createVtkDataMesh4DWriter);
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
