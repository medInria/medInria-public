/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersProcessBase.h>
#include <itkProcessObject.h>

#include <medAbstractDataFactory.h>

class itkFiltersProcessBasePrivate
{
public:
    dtkSmartPointer<medAbstractImageData> inputData;
    dtkSmartPointer<medAbstractImageData> outputData;
};

itkFiltersProcessBase::itkFiltersProcessBase(itkFiltersProcessBase *parent) 
    : medAbstractProcessLegacy(parent), d(new itkFiltersProcessBasePrivate)
{  
    d->inputData = nullptr;
    d->outputData = nullptr;
}

itkFiltersProcessBase::itkFiltersProcessBase(const itkFiltersProcessBase& other)
     : medAbstractProcessLegacy(other), d(other.d)
{
}

itkFiltersProcessBase& itkFiltersProcessBase::operator = (const itkFiltersProcessBase& other)
{
    return *this;
}

void itkFiltersProcessBase::emitProgress(int progress)
{
    emit progressed(progress);
}

void itkFiltersProcessBase::setInput(medAbstractData *data, int channel)
{
    Q_UNUSED(channel)

    if (data)
    {
        QString identifier = data->identifier();

        d->outputData = dynamic_cast<medAbstractImageData*> (medAbstractDataFactory::instance()->create(identifier));
        d->inputData = dynamic_cast<medAbstractImageData*> (data);
    }
}

medAbstractData * itkFiltersProcessBase::output()
{   
    return d->outputData;
}

int itkFiltersProcessBase::update()
{
    int res = medAbstractProcessLegacy::FAILURE;

    try
    {
        res = tryUpdate();
    }
    catch( itk::ExceptionObject & error )
    {
        qDebug() << "ExceptionObject caught in" << metaObject()->className() << "!";
        qDebug() << error.GetDescription();
    }

    return res;
}

void itkFiltersProcessBase::eventCallback ( itk::Object *caller, const itk::EventObject& event, void *clientData)
{
    itkFiltersProcessBase * source = reinterpret_cast<itkFiltersProcessBase *> ( clientData );
    itk::ProcessObject * processObject = ( itk::ProcessObject* ) caller;

    if (!source)
    {
        qWarning() << "Source is null";
    }

    source->emitProgress(static_cast<int>((processObject->GetProgress() * 100)));
}

dtkSmartPointer<medAbstractImageData> itkFiltersProcessBase::getInputData()
{
    return d->inputData;
}

void itkFiltersProcessBase::setInputData(dtkSmartPointer<medAbstractImageData> inputData)
{
    d->inputData = inputData;
}

dtkSmartPointer<medAbstractImageData> itkFiltersProcessBase::getOutputData()
{
    return d->outputData;
}

void itkFiltersProcessBase::setOutputData(dtkSmartPointer<medAbstractImageData> outputData)
{
    d->outputData = outputData;
}
