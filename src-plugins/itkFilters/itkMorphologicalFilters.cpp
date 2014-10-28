/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkMorphologicalFilters.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkSmartPointer.h>
#include <itkMorphologicalFiltersToolBox.h>

#include <medToolBoxHeader.h>

class itkMorphologicalFiltersPrivate
{
public:
    QPointer<itkMorphologicalFiltersToolBox> toolbox;
    dtkSmartPointer<medAbstractFilteringProcess> process;
};
//-------------------------------------------------------------------------------------------

itkMorphologicalFilters::itkMorphologicalFilters(itkMorphologicalFilters *parent)
    : itkFiltersProcessBase(parent), d(new itkMorphologicalFiltersPrivate)
{
    this->setFilter(this);
    d->process = NULL;

    d->toolbox = new itkMorphologicalFiltersToolBox;
    d->toolbox->header()->hide();

    QStringList filtersList;
    filtersList << "Dilate"
                << "Erode"
                << "Close"
                << "Open";

    d->toolbox->setAvailableProcesses(filtersList);

    connect(d->toolbox, SIGNAL(processSelected(QString)), this, SLOT(setupProcess(QString)));
}

//-------------------------------------------------------------------------------------------

itkMorphologicalFilters::~itkMorphologicalFilters( void )
{
    d->process.releasePointer();
}

//-------------------------------------------------------------------------------------------

bool itkMorphologicalFilters::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkMorphologicalFilters", createitkMorphologicalFilters, "medAbstractFilteringProcess");
}

//-------------------------------------------------------------------------------------------

void itkMorphologicalFilters::setInputImage ( medAbstractData *data )
{
    itkFiltersProcessBase::setInputImage(data);

    if( d->process )
        d->process->setInputImage(data);

    if(!d->toolbox.isNull())
        d->toolbox->update(data);
}

medAbstractData * itkMorphologicalFilters::output (  )
{
    if( d->process )
      return ( d->process->output() );

    return NULL;
}

//-------------------------------------------------------------------------------------------

QList<medAbstractParameter*> itkMorphologicalFilters::parameters()
{
    return QList<medAbstractParameter*>();
}

medToolBox* itkMorphologicalFilters::toolbox()
{
    return d->toolbox;
}

//-------------------------------------------------------------------------------------------

void itkMorphologicalFilters::setupProcess(QString process)
{
    if(process == "Dilate")
    {
        d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkDilateProcess" );
    }
    else if(process == "Erode")
    {
        d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkErodeProcess" );
    }
    else if(process == "Close")
    {
        d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkCloseProcess" );
    }
    else if(process == "Open")
    {
        d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkOpenProcess" );
    }

    if(d->process)
    {
        d->process->setInputImage(this->inputImage());
        d->toolbox->setProcessToolbox(d->process->toolbox());
    }
}

int itkMorphologicalFilters::update ( void )
{
    return d->process->update();
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkMorphologicalFilters ( void )
{
    return new itkMorphologicalFilters;
}
