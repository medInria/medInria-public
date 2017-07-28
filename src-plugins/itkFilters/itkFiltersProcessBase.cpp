/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersProcessBase.h>
#include <itkMacro.h>
#include <itkProcessObject.h>

#include <medAbstractDataFactory.h>

itkFiltersProcessBase::itkFiltersProcessBase(itkFiltersProcessBase *parent) 
    : medAbstractProcess()
{  
    inputData = NULL;
    outputData = NULL;
    
    descriptionText = "";
}

itkFiltersProcessBase& itkFiltersProcessBase::operator = (const itkFiltersProcessBase& other)
{
    return *this;
}

void itkFiltersProcessBase::emitProgress(int progress)
{
    emit progressed(progress);
}

QString itkFiltersProcessBase::description()
{ 
    return descriptionText;
}

void itkFiltersProcessBase::setInput(medAbstractData *data, int channel)
{
    if (!data)
        return;
   
    QString identifier = data->identifier();
    
    outputData = dynamic_cast<medAbstractImageData*> (medAbstractDataFactory::instance()->create(identifier));
    inputData = dynamic_cast<medAbstractImageData*> (data);
}

medAbstractData * itkFiltersProcessBase::output ( void )
{   
    return outputData;
}

int itkFiltersProcessBase::update()
{
    int res = DTK_FAILURE;

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

    if ( !source ) { dtkWarn() << "Source is null"; }

    source->filter->emitProgress((int) (processObject->GetProgress() * 100));
}
