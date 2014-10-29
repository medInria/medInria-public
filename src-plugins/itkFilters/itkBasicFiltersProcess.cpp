/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkBasicFiltersProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkSmartPointer.h>
#include <itkFiltersToolBox.h>

#include <medToolBoxHeader.h>

#include <medTriggerParameter.h>

class itkBasicFiltersProcessPrivate
{
public:
    QPointer<itkFiltersToolBox> toolbox;
    dtkSmartPointer<medAbstractFilteringProcess> process;
};
//-------------------------------------------------------------------------------------------

itkBasicFiltersProcess::itkBasicFiltersProcess(itkBasicFiltersProcess *parent)
    : itkFiltersProcessBase(parent), d(new itkBasicFiltersProcessPrivate)
{
    this->setFilter(this);

    d->toolbox = new itkFiltersToolBox;
    d->toolbox->header()->hide();

    QStringList filtersList;
    filtersList << "Add Constant to Image"
                << "Substract Constant from Image"
                << "Multiply image by constant"
                << "Divide image by constant"
                << "Gaussian blur"
                << "Normalize image filter"
                << "Median filter"
                << "Invert intensity filter"
                << "Shrink image filter"
                << "Intensity windowing filter";

    d->toolbox->setAvailableProcesses(filtersList);

    connect(d->toolbox, SIGNAL(processSelected(QString)), this, SLOT(setupProcess(QString)));
}

//-------------------------------------------------------------------------------------------

itkBasicFiltersProcess::~itkBasicFiltersProcess( void )
{
    d->process.releasePointer();
}

//-------------------------------------------------------------------------------------------

bool itkBasicFiltersProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkBasicFiltersProcess", createitkBasicFiltersProcess, "medAbstractFilteringProcess");
}

//-------------------------------------------------------------------------------------------

void itkBasicFiltersProcess::setInputImage ( medAbstractData *data )
{
    itkFiltersProcessBase::setInputImage(data);

    if( d->process )
        d->process->setInputImage(data);

    if(!d->toolbox.isNull())
        d->toolbox->update(data);
}

medAbstractData * itkBasicFiltersProcess::output (  )
{
    if( d->process )
      return ( d->process->output() );

    return NULL;
}

//-------------------------------------------------------------------------------------------

QList<medAbstractParameter*> itkBasicFiltersProcess::parameters()
{
    return QList<medAbstractParameter*>();
}

medToolBox* itkBasicFiltersProcess::toolbox()
{
    return d->toolbox;
}

//-------------------------------------------------------------------------------------------

void itkBasicFiltersProcess::setupProcess(QString process)
{
    if(process == "Add Constant to Image")
    {
        d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkAddProcess" );
    }
    else if(process == "Substract Constant from Image")
    {
        d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkSubtractProcess" );
    }
    else if(process == "Multiply image by constant")
    {
        d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkMultiplyProcess" );
    }
    else if(process == "Divide image by constant")
    {
        d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkDivideProcess" );
    }
    else if(process == "Gaussian blur" )
    {
        d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkGaussianProcess" );
    }
    else if(process == "Normalize image filter")
    {
        d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkNormalizeProcess" );
    }
    else if(process == "Median filter")
    {
        d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkMedianProcess" );
    }
    else if(process == "Invert intensity filter")
    {
        d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkInvertProcess" );
    }
    else if(process == "Shrink image filter")
    {
        d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkShrinkProcess" );
    }
    else if(process ==  "Intensity windowing filter")
    {
        d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkWindowingProcess" );
    }

    if(d->process)
    {
        d->process->setInputImage(this->inputImage());
        d->toolbox->setProcessToolbox(d->process->toolbox());

        connect(d->process->runParameter(), SIGNAL(triggered()),
                this->runParameter(), SIGNAL(triggered()));
    }

}

int itkBasicFiltersProcess::update ( void )
{
    return d->process->start();
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkBasicFiltersProcess ( void )
{
    return new itkBasicFiltersProcess;
}
