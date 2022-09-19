/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkFibersDataReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <vtkXMLFiberDataSetReader.h>
#include <vtkFiberDataSet.h>

#include <QColor>

class medVtkFibersDataReaderPrivate
{
public:
    vtkXMLFiberDataSetReader *reader;
};

const char medVtkFibersDataReader::ID[] = "medVtkFibersDataReader";

medVtkFibersDataReader::medVtkFibersDataReader(): d(new medVtkFibersDataReaderPrivate)
{
    d->reader = vtkXMLFiberDataSetReader::New();
}

medVtkFibersDataReader::~medVtkFibersDataReader()
{
    d->reader->Delete();
    delete d;
    d = nullptr;
}

QStringList medVtkFibersDataReader::handled() const
{
    return QStringList() << "medVtkFibersData";
}

bool medVtkFibersDataReader::canRead (const QString& path)
{
    return d->reader->CanReadFile (path.toUtf8().constData());
}

bool medVtkFibersDataReader::canRead (const QStringList& paths)
{
    if (!paths.count())
        return false;
    return this->canRead (paths[0]);
}

bool medVtkFibersDataReader::readInformation (const QString& path)
{
    medAbstractData *medData = dynamic_cast<medAbstractData*>(this->data());

    if (!medData)
    {
        medData = medAbstractDataFactory::instance()->create("medVtkFibersData");
        if (medData)
            this->setData (medData);
    }
    
    return true;
}

bool medVtkFibersDataReader::readInformation (const QStringList& paths)
{
    if (!paths.count())
        return false;
    return this->readInformation ( paths[0].toUtf8().constData() );
}

bool medVtkFibersDataReader::read (const QString& path)
{
    this->setProgress (0);

    this->readInformation (path);

    this->setProgress (25);

    if (medAbstractData *medData = dynamic_cast<medAbstractData*>(this->data()))
    {
        d->reader->SetFileName (path.toUtf8().constData());
        d->reader->Update();

        if (vtkFiberDataSet *dataset = d->reader->GetOutput())
        {
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

            medData->setMetaData ("BundleList", bundles);
            medData->setMetaData ("BundleColorList", bundleColors);
        }

        medData->setData (d->reader->GetOutput());
    }

    this->setProgress (100);

    return true;
}

bool medVtkFibersDataReader::read (const QStringList& paths)
{
    if (!paths.count())
        return false;
    return this->read ( paths[0].toUtf8().constData() );
}

void medVtkFibersDataReader::setProgress (int value)
{
    emit progressed (value);
}

QString medVtkFibersDataReader::identifier() const
{
    return ID;
}

QString medVtkFibersDataReader::description() const
{
    return "Reader for medVtkFibersData";
}

bool medVtkFibersDataReader::registered()
{
    return medAbstractDataFactory::instance()->registerDataReaderType(ID,
                                                                      QStringList() << "medVtkFibersData",
                                                                      create_medVtkFibersDataReader);
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *create_medVtkFibersDataReader()
{
    return new medVtkFibersDataReader;
}
