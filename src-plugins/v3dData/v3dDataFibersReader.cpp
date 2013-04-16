/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <v3dDataFibersReader.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <vtkXMLFiberDataSetReader.h>
#include <vtkFiberDataSet.h>

class v3dDataFibersReaderPrivate {
public:
    vtkXMLFiberDataSetReader *reader;
};

const char v3dDataFibersReader::ID[] = "v3dDataFibersReader";

v3dDataFibersReader::v3dDataFibersReader(): d(new v3dDataFibersReaderPrivate) {
    d->reader = vtkXMLFiberDataSetReader::New();
}

v3dDataFibersReader::~v3dDataFibersReader() {
    d->reader->Delete();
    delete d;
    d = NULL;
}

QStringList v3dDataFibersReader::handled() const {
    return QStringList() << "v3dDataFibers";
}

bool v3dDataFibersReader::canRead (const QString& path) {
    return d->reader->CanReadFile (path.toAscii().constData());
}

bool v3dDataFibersReader::canRead (const QStringList& paths) {
    if (!paths.count())
        return false;
    return this->canRead (paths[0]);
}

bool v3dDataFibersReader::readInformation (const QString& path) {
    // d->reader->SetFileName (path.toAscii().constData());

    dtkSmartPointer<dtkAbstractData> dtkdata = this->data();

    if (!dtkdata) {
        dtkdata = dtkAbstractDataFactory::instance()->createSmartPointer ("v3dDataFibers");
        if (dtkdata)
            this->setData (dtkdata);
    }
    
    return true;
}

bool v3dDataFibersReader::readInformation (const QStringList& paths) {
    if (!paths.count())
        return false;
    return this->readInformation ( paths[0].toAscii().constData() );
}

bool v3dDataFibersReader::read (const QString& path) {
  this->setProgress (0);

  this->readInformation (path);

  this->setProgress (25);

  if (dtkAbstractData *dtkdata = this->data()) {
    d->reader->SetFileName (path.toAscii().constData());
    d->reader->Update();

    if (vtkFiberDataSet *dataset = d->reader->GetOutput()) {

        QStringList bundles;
        QStringList bundleColors;

        vtkFiberDataSet::vtkFiberBundleListType bundleList   = dataset->GetBundleList();
        vtkFiberDataSet::vtkFiberBundleListType::iterator it = bundleList.begin();
        while (it!=bundleList.end())
        {
          bundles << (*it).first.c_str();
          QColor color (static_cast<int>((*it).second.Red * 255.0),
                        static_cast<int>((*it).second.Green * 255.0),
                        static_cast<int>((*it).second.Blue * 255.0));
          bundleColors << color.name();
          ++it;
        }

        dtkdata->setMetaData ("BundleList",      bundles);
        dtkdata->setMetaData ("BundleColorList", bundleColors);
    }

    dtkdata->setData ( d->reader->GetOutput() );
  }

  this->setProgress (100);

  return true;
}

bool v3dDataFibersReader::read (const QStringList& paths) {
    if (!paths.count())
        return false;
    return this->read ( paths[0].toAscii().constData() );
}
  
void v3dDataFibersReader::setProgress (int value) {
    emit progressed (value);
}

QString v3dDataFibersReader::identifier() const {
    return ID;
}

QString v3dDataFibersReader::description() const {
    return "Reader for v3d fibers";
}

bool v3dDataFibersReader::registered() {
  return dtkAbstractDataFactory::instance()->registerDataReaderType(ID,
								    QStringList() << "v3dDataFibers",
								    createV3dDataFibersReader);
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createV3dDataFibersReader() {
  return new v3dDataFibersReader;
}

