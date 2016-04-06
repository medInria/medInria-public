/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkDataMesh4DReader.h>
#include <vtkDataManagerReader.h>
#include <vtkDataManager.h>
#include <vtkMetaDataSetSequence.h>
#include <vtkSmartPointer.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

const char vtkDataMesh4DReader::ID[] = "vtkDataMesh4DReader";

vtkDataMesh4DReader::vtkDataMesh4DReader(): dtkAbstractDataReader() {
    this->reader = vtkDataManagerReader::New();
}

vtkDataMesh4DReader::~vtkDataMesh4DReader() {
    this->reader->Delete();
}

QStringList vtkDataMesh4DReader::handled() const {
    return QStringList() << "vtkDataMesh4D";
}

QStringList vtkDataMesh4DReader::s_handled() {
    return QStringList() << "vtkDataMesh4D";
}

bool vtkDataMesh4DReader::canRead (const QString& path) {
    return this->reader->CanReadFile (path.toLatin1().constData());
}

bool vtkDataMesh4DReader::canRead (const QStringList& paths) {
    if (!paths.count())
        return false;
    return this->canRead ( paths[0].toLatin1().constData() );
}

bool vtkDataMesh4DReader::readInformation (const QString& path) {
  
    medAbstractData *medData = dynamic_cast<medAbstractData*>(this->data());
    this->reader->SetFileName (path.toLatin1().constData());
  
    if (!medData) {
        medData = medAbstractDataFactory::instance()->create("vtkDataMesh4D");
        if (medData)
            this->setData ( medData );
    }

    medData->addMetaData ("FilePath", QStringList() << path);
    medData->identifier() = "vtkDataMesh4D";
    
    return true;
}

bool vtkDataMesh4DReader::readInformation (const QStringList& paths) {
    if (!paths.count())
        return false;
    return this->readInformation ( paths[0].toLatin1().constData() );
}

bool vtkDataMesh4DReader::read (const QString& path) {
    this->setProgress (0);

    this->readInformation ( path );

    this->setProgress (50);

    dtkDebug() << "Can read with: " << this->identifier();

    if (medAbstractData *medData = dynamic_cast<medAbstractData*>(this->data()) ) {

        if (!(medData->identifier()=="vtkDataMesh4D"))
            return false;

        this->reader->SetFileName (path.toLatin1().constData());
        this->reader->Update();
        vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::SafeDownCast (this->reader->GetOutput()->GetMetaDataSet ((unsigned int)0));
        if (sequence)
            medData->setData (sequence );
    }

    this->setProgress (100);
    return true;
}

bool vtkDataMesh4DReader::read (const QStringList& paths) {
    if (!paths.count())
        return false;
    return this->read ( paths[0].toLatin1().constData() );
}

void vtkDataMesh4DReader::setProgress (int value) {
    emit progressed (value);
}

bool vtkDataMesh4DReader::registered() {
    return medAbstractDataFactory::instance()->registerDataReaderType(
          "vtkDataMesh4DReader",
          vtkDataMesh4DReader::s_handled(),
          createVtkDataMesh4DReader);
}

QString vtkDataMesh4DReader::identifier() const {
    return ID;
}

QString vtkDataMesh4DReader::description() const {
    return "Reader for vtk 4D meshes";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createVtkDataMesh4DReader() {
    return new vtkDataMesh4DReader;
}
