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

#include <medAbstractDataFactory.h>
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
        medAbstractData *input = this->input<medAbstractData>(0);
        medAbstractData *output = this->output<medAbstractData>(0);

        d->process->setInput<medAbstractData>(input, 0);
        d->process->setOutput<medAbstractData>(output, 0);

        d->toolbox->setProcessToolbox(d->process->toolbox());

        connect(d->process->runParameter(), SIGNAL(triggered()),
                this->runParameter(), SIGNAL(triggered()));

        // handleOutputs will be managed by itkBasicFiltersProcess, no need to do it twice
        disconnect(d->process, SIGNAL(success()), d->process, SLOT(handleOutputs()));
    }

}

int itkBasicFiltersProcess::update ( void )
{
    return d->process->start();
}

void itkBasicFiltersProcess::handleInput()
{
    medAbstractFilteringProcess::handleInput();

    medAbstractData *input = this->input<medAbstractData>(0);

    if(!input)
        return;

    medAbstractData *output = medAbstractDataFactory::instance()->create(input->identifier());

    this->setOutput<medAbstractData>(output, 0);

    if(d->process)
    {
        d->process->setInput<medAbstractData>(input, 0);
        d->process->setOutput<medAbstractData>(output, 0);
    }

    if(!d->toolbox.isNull())
        d->toolbox->update(input);
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkBasicFiltersProcess ( void )
{
    return new itkBasicFiltersProcess;
}


