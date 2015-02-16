/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtk4DMeshDataReader.h>
#include <vtkDataManagerReader.h>
#include <vtkDataManager.h>
#include <vtkMetaDataSetSequence.h>
#include <vtkSmartPointer.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

const char medVtk4DMeshDataReader::ID[] = "medVtk4DMeshDataReader";

medVtk4DMeshDataReader::medVtk4DMeshDataReader(): dtkAbstractDataReader() {
    this->reader = vtkDataManagerReader::New();
}

medVtk4DMeshDataReader::~medVtk4DMeshDataReader() {
    this->reader->Delete();
}

QStringList medVtk4DMeshDataReader::handled() const {
    return QStringList() << "medVtk4DMeshData";
}

QStringList medVtk4DMeshDataReader::s_handled() {
    return QStringList() << "medVtk4DMeshData";
}

bool medVtk4DMeshDataReader::canRead (const QString& path) {
    return this->reader->CanReadFile (path.toAscii().constData());
}

bool medVtk4DMeshDataReader::canRead (const QStringList& paths) {
    if (!paths.count())
        return false;
    return this->canRead ( paths[0].toAscii().constData() );
}

bool medVtk4DMeshDataReader::readInformation (const QString& path) {

    medAbstractData *medData = dynamic_cast<medAbstractData*>(this->data());
    this->reader->SetFileName (path.toAscii().constData());

    if (!medData) {
        medData = medAbstractDataFactory::instance()->create("medVtk4DMeshData");
        if (medData)
            this->setData ( medData );
    }

    medData->addMetaData ("FilePath", QStringList() << path);
    medData->identifier() = "medVtk4DMeshData";

    return true;
}

bool medVtk4DMeshDataReader::readInformation (const QStringList& paths) {
    if (!paths.count())
        return false;
    return this->readInformation ( paths[0].toAscii().constData() );
}

bool medVtk4DMeshDataReader::read (const QString& path) {
    this->setProgress (0);

    this->readInformation ( path );

    this->setProgress (50);

    qDebug() << "Can read with: " << this->identifier();

    if (medAbstractData *medData = dynamic_cast<medAbstractData*>(this->data()) ) {

        if (!(medData->identifier()=="medVtk4DMeshData"))
            return false;

        this->reader->SetFileName (path.toAscii().constData());
        this->reader->Update();
        vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::SafeDownCast (this->reader->GetOutput()->GetMetaDataSet ((unsigned int)0));
        if (sequence)
            medData->setData (sequence );
    }

    this->setProgress (100);
    return true;
}

bool medVtk4DMeshDataReader::read (const QStringList& paths) {
    if (!paths.count())
        return false;
    return this->read ( paths[0].toAscii().constData() );
}

void medVtk4DMeshDataReader::setProgress (int value) {
    emit progressed (value);
}

bool medVtk4DMeshDataReader::registered() {
    return medAbstractDataFactory::instance()->registerDataReaderType(
          "medVtk4DMeshDataReader",
          medVtk4DMeshDataReader::s_handled(),
          createmedVtk4DMeshDataReader);
}

QString medVtk4DMeshDataReader::identifier() const {
    return ID;
}

QString medVtk4DMeshDataReader::description() const {
    return "Reader for vtk 4D meshes";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createmedVtk4DMeshDataReader() {
    return new medVtk4DMeshDataReader;
}
