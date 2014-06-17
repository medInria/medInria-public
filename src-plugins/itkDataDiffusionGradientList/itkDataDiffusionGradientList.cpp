/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDataDiffusionGradientList.h>

#include <medAbstractDataFactory.h>

#include <itkGradientFileReader.h>

#include <QFile>
#include <QVector>
#include <QString>

// /////////////////////////////////////////////////////////////////
// itkDataDiffusionGradientListPrivate
// /////////////////////////////////////////////////////////////////

class itkDataDiffusionGradientListPrivate
{
public:

    typedef QVector<double>       GradientType;
    typedef QVector<GradientType> GradientListType;

    GradientListType gradients;
};

// /////////////////////////////////////////////////////////////////
// itkDataDiffusionGradientList
// /////////////////////////////////////////////////////////////////

itkDataDiffusionGradientList::itkDataDiffusionGradientList() : medAbstractData(), d(new itkDataDiffusionGradientListPrivate)
{

}

itkDataDiffusionGradientList::~itkDataDiffusionGradientList()
{

}

bool itkDataDiffusionGradientList::registered()
{
    return medAbstractDataFactory::instance()->registerDataType("itkDataDiffusionGradientList", createItkDataDiffusionGradientList);
}

QString itkDataDiffusionGradientList::description() const
{
    return "itkDataDiffusionGradientList";
}

void *itkDataDiffusionGradientList::data()
{
    return d->gradients.data();
}

void *itkDataDiffusionGradientList::data (int channel)
{
    if (channel>=d->gradients.count())
        return 0;
    return d->gradients[channel].data();
}

void itkDataDiffusionGradientList::setData (void* data)
{
    d->gradients = *(dynamic_cast<itkDataDiffusionGradientListPrivate::GradientListType*>( static_cast<QObject*>(data) ));
}

bool itkDataDiffusionGradientList::read (const QString& filename)
{
    typedef itkDataDiffusionGradientListPrivate::GradientType     GradientType;
    typedef itkDataDiffusionGradientListPrivate::GradientListType GradientListType;
        
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

bool itkDataDiffusionGradientList::write (const QString& file)
{
    qDebug() << "itkDataDiffusionGradientList::write() not implemented yet.";
    return false;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

medAbstractData *createItkDataDiffusionGradientList()
{
    return new itkDataDiffusionGradientList;
}
