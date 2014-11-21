/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkDiffusionGradientListData.h>

#include <medAbstractDataFactory.h>

#include <itkGradientFileReader.h>

#include <QFile>
#include <QVector>
#include <QString>

// /////////////////////////////////////////////////////////////////
// medItkDiffusionGradientListDataPrivate
// /////////////////////////////////////////////////////////////////

class medItkDiffusionGradientListDataPrivate
{
public:

    typedef QVector<double>       GradientType;
    typedef QVector<GradientType> GradientListType;

    GradientListType gradients;
};

// /////////////////////////////////////////////////////////////////
// medItkDiffusionGradientListData
// /////////////////////////////////////////////////////////////////

medItkDiffusionGradientListData::medItkDiffusionGradientListData() : medAbstractData(), d(new medItkDiffusionGradientListDataPrivate)
{

}

medItkDiffusionGradientListData::~medItkDiffusionGradientListData()
{

}

bool medItkDiffusionGradientListData::registered()
{
    return medAbstractDataFactory::instance()->registerDataType<medItkDiffusionGradientListData>();
}

void *medItkDiffusionGradientListData::data()
{
    return d->gradients.data();
}

void *medItkDiffusionGradientListData::data (int channel)
{
    if (channel>=d->gradients.count())
        return 0;
    return d->gradients[channel].data();
}

void medItkDiffusionGradientListData::setData (void* data)
{
    d->gradients = *(dynamic_cast<medItkDiffusionGradientListDataPrivate::GradientListType*>( static_cast<QObject*>(data) ));
}

bool medItkDiffusionGradientListData::read (const QString& filename)
{
    typedef medItkDiffusionGradientListDataPrivate::GradientType     GradientType;
    typedef medItkDiffusionGradientListDataPrivate::GradientListType GradientListType;

    GradientListType output;
    //Read Gradients
    typedef itk::GradientFileReader ReaderType;
    typedef ReaderType::VectorListType VectorListType;

    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName (filename.toStdString());
    reader->Update();
    VectorListType gradients = reader->GetGradientList();

    for(unsigned i=0; i<(gradients.size()); i++)
    {
        GradientType grad (3);
        grad[0] = gradients[i][0];
        grad[1] = gradients[i][1];
        grad[2] = gradients[i][2];
        output.push_back (grad);
    }

    this->setNumberOfChannels (output.size());

    d->gradients = output;

    return true;
}

bool medItkDiffusionGradientListData::write (const QString& file)
{
    qDebug() << "medItkDiffusionGradientListData::write() not implemented yet.";
    return false;
}
