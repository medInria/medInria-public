/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkFiltersProcessBase.h>

#include <medAbstractDataFactory.h>
#include <medAbstractImageData.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkProcessObject.h>
#include <itkCommand.h>

class medItkFiltersProcessBasePrivate
{
public:
    medItkFiltersProcessBasePrivate(medItkFiltersProcessBase *q = 0){}

    virtual ~medItkFiltersProcessBasePrivate(void) {}

    QString description;
    QString filterType;

    itk::CStyleCommand::Pointer callback;
    medItkFiltersProcessBase *filter;

    template <class PixelType> void setupFilter() {}
    virtual void setFilterDescription() {}

    static void eventCallback ( itk::Object *caller, const itk::EventObject& event, void *clientData) {
        medItkFiltersProcessBase * source = reinterpret_cast<medItkFiltersProcessBase *> ( clientData );
        itk::ProcessObject * processObject = ( itk::ProcessObject* ) caller;

        if ( !source ) { dtkWarn() << "Source is null"; }

        source->emitProgress((int) (processObject->GetProgress() * 100));
    }
};

medItkFiltersProcessBase::medItkFiltersProcessBase(medItkFiltersProcessBase *parent)
    : medAbstractFilteringProcess(parent), d(new medItkFiltersProcessBasePrivate)
{
    d->description = "";
}


medItkFiltersProcessBase::medItkFiltersProcessBase(const medItkFiltersProcessBase& other)
    : medAbstractFilteringProcess(other)
{

}


medItkFiltersProcessBase& medItkFiltersProcessBase::operator = (const medItkFiltersProcessBase& other)
{
    return *this;
}

medItkFiltersProcessBase::~medItkFiltersProcessBase()
{

}

void medItkFiltersProcessBase::emitProgress(int progress)
{
    emit progressed(progress);
}

QString medItkFiltersProcessBase::description() const
{
    return d->description;
}

void medItkFiltersProcessBase::setDescription(QString description)
{
    d->description = description;
}

QString medItkFiltersProcessBase::filterType() const
{
    return d->filterType;
}

void medItkFiltersProcessBase::setFilterType(QString filterType)
{
    d->filterType = filterType;
}

itk::CStyleCommand::Pointer medItkFiltersProcessBase::callback() const
{
    return d->callback;
}

void medItkFiltersProcessBase::setCallBack(itk::CStyleCommand::Pointer callback)
{
    d->callback = callback;
    callback->SetCallback ( medItkFiltersProcessBasePrivate::eventCallback );
}

medItkFiltersProcessBase *medItkFiltersProcessBase::filter() const
{
    return d->filter;
}

void medItkFiltersProcessBase::setFilter(medItkFiltersProcessBase *filter)
{
    d->filter = filter;
}

//void medItkFiltersProcessBase::setInputImage(medAbstractData *data)
//{
//    if (!data)
//        return;

//    QString identifier = data->identifier();

//    d->output = dynamic_cast<medAbstractImageData*> (medAbstractDataFactory::instance()->create(identifier));
//    d->input = dynamic_cast<medAbstractImageData*> (data);
//}

//medAbstractData * medItkFiltersProcessBase::output (  )
//{
//    return ( d->output );
//}

QList<medAbstractParameter*> medItkFiltersProcessBase::parameters()
{
    return QList<medAbstractParameter*>();
}

bool medItkFiltersProcessBase::isInteractive()
{
    return false;
}
