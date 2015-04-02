/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkBasicFiltersProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkSmartPointer.h>
#include <medItkFiltersToolBox.h>

#include <medAbstractDataFactory.h>
#include <medToolBoxHeader.h>

#include <medTriggerParameter.h>

class medItkBasicFiltersProcessPrivate
{
public:
    QPointer<medItkFiltersToolBox> toolbox;
    dtkSmartPointer<medAbstractFilteringProcess> process;
};
//-------------------------------------------------------------------------------------------

medItkBasicFiltersProcess::medItkBasicFiltersProcess(medItkBasicFiltersProcess *parent)
    : medItkFiltersProcessBase(parent), d(new medItkBasicFiltersProcessPrivate)
{
    this->setFilter(this);

    d->toolbox = new medItkFiltersToolBox;
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

medItkBasicFiltersProcess::~medItkBasicFiltersProcess( void )
{
    d->process.releasePointer();
}

//-------------------------------------------------------------------------------------------

bool medItkBasicFiltersProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("medItkBasicFiltersProcess", createmedItkBasicFiltersProcess, "medAbstractFilteringProcess");
}

//-------------------------------------------------------------------------------------------

QList<medAbstractParameter*> medItkBasicFiltersProcess::parameters()
{
    return QList<medAbstractParameter*>();
}

medToolBox* medItkBasicFiltersProcess::toolbox()
{
    return d->toolbox;
}

//-------------------------------------------------------------------------------------------

void medItkBasicFiltersProcess::setupProcess(QString process)
{
    if(process == "Add Constant to Image")
    {
        d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "medItkAddProcess" );
    }
    else if(process == "Substract Constant from Image")
    {
        d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkSubtractProcess" );
    }
    else if(process == "Multiply image by constant")
    {
        d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "medItkMultiplyProcess" );
    }
    else if(process == "Divide image by constant")
    {
        d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "medItkDivideProcess" );
    }
    else if(process == "Gaussian blur" )
    {
        d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "medItkGaussianProcess" );
    }
    else if(process == "Normalize image filter")
    {
        d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "medItkNormalizeProcess" );
    }
    else if(process == "Median filter")
    {
        d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "medItkMedianProcess" );
    }
    else if(process == "Invert intensity filter")
    {
        d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "medItkInvertProcess" );
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

        // handleOutputs will be managed by medItkBasicFiltersProcess, no need to do it twice
        disconnect(d->process, SIGNAL(success()), d->process, SLOT(handleOutputs()));
    }

}

int medItkBasicFiltersProcess::update ( void )
{
    return d->process->start();
}

void medItkBasicFiltersProcess::handleInput()
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

dtkAbstractProcess * createmedItkBasicFiltersProcess ( void )
{
    return new medItkBasicFiltersProcess;
}


