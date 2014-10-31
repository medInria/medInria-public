/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersProcessBase.h>

#include <medAbstractDataFactory.h>
#include <medAbstractImageData.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkProcessObject.h>
#include <itkCommand.h>

class itkFiltersProcessBasePrivate
{
public:
    itkFiltersProcessBasePrivate(itkFiltersProcessBase *q = 0){}

    virtual ~itkFiltersProcessBasePrivate(void) {}

    QString description;
    QString filterType;

    itk::CStyleCommand::Pointer callback;
    itkFiltersProcessBase *filter;

    template <class PixelType> void setupFilter() {}
    virtual void setFilterDescription() {}

    static void eventCallback ( itk::Object *caller, const itk::EventObject& event, void *clientData) {
        itkFiltersProcessBase * source = reinterpret_cast<itkFiltersProcessBase *> ( clientData );
        itk::ProcessObject * processObject = ( itk::ProcessObject* ) caller;

        if ( !source ) { dtkWarn() << "Source is null"; }

        source->emitProgress((int) (processObject->GetProgress() * 100));
    }
};

itkFiltersProcessBase::itkFiltersProcessBase(itkFiltersProcessBase *parent) 
    : medAbstractFilteringProcess(parent), d(new itkFiltersProcessBasePrivate)
{    
    d->description = "";
}


itkFiltersProcessBase::itkFiltersProcessBase(const itkFiltersProcessBase& other) 
    : medAbstractFilteringProcess(other)
{
    
}


itkFiltersProcessBase& itkFiltersProcessBase::operator = (const itkFiltersProcessBase& other)
{
    return *this;
}

itkFiltersProcessBase::~itkFiltersProcessBase()
{
    
}

void itkFiltersProcessBase::emitProgress(int progress)
{
    emit progressed(progress);
}

QString itkFiltersProcessBase::description() const
{    
    return d->description;
}

void itkFiltersProcessBase::setDescription(QString description)
{
    d->description = description;
}

QString itkFiltersProcessBase::filterType() const
{
    return d->filterType;
}

void itkFiltersProcessBase::setFilterType(QString filterType)
{
    d->filterType = filterType;
}

itk::CStyleCommand::Pointer itkFiltersProcessBase::callback() const
{
    return d->callback;
}

void itkFiltersProcessBase::setCallBack(itk::CStyleCommand::Pointer callback)
{
    d->callback = callback;
    callback->SetCallback ( itkFiltersProcessBasePrivate::eventCallback );
}

itkFiltersProcessBase *itkFiltersProcessBase::filter() const
{
    return d->filter;
}

void itkFiltersProcessBase::setFilter(itkFiltersProcessBase *filter)
{
    d->filter = filter;
}

//void itkFiltersProcessBase::setInputImage(medAbstractData *data)
//{
//    if (!data)
//        return;
     
//    QString identifier = data->identifier();
    
//    d->output = dynamic_cast<medAbstractImageData*> (medAbstractDataFactory::instance()->create(identifier));
//    d->input = dynamic_cast<medAbstractImageData*> (data);
//}

//medAbstractData * itkFiltersProcessBase::output (  )
//{
//    return ( d->output );
//}

QList<medAbstractParameter*> itkFiltersProcessBase::parameters()
{
    return QList<medAbstractParameter*>();
}

bool itkFiltersProcessBase::isInteractive()
{
    return false;
}
