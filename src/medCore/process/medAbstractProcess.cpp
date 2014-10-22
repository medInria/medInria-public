/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractProcess.h>

#include <medAbstractParameter.h>
#include <medToolBox.h>

class medAbstractProcessPrivate
{
  public:
    QList<medProcessIOPort*> inputs;
    QList<medProcessIOPort*> outputs;
    QPointer<medToolBox> toolbox;
};

medAbstractProcess::medAbstractProcess(medAbstractProcess * parent):dtkAbstractProcess(parent), d(new medAbstractProcessPrivate)
{
    d->toolbox = NULL;
}

medAbstractProcess::~medAbstractProcess()
{
    delete d;
}


QList<medProcessIOPort*> medAbstractProcess::inputs()
{
   return d->inputs;
}

QList<medProcessIOPort*> medAbstractProcess::outputs()
{
    return d->outputs;
}

void medAbstractProcess::appendInput(medProcessIOPort *input)
{
    d->inputs.append(input);
}

void medAbstractProcess::appendOutput(medProcessIOPort *output)
{
    d->outputs.append(output);
}

medAbstractParameter* medAbstractProcess::parameter(QString parameterName)
{
    foreach(medAbstractParameter *param, this->parameters())
    {
        if(param->name() == parameterName)
            return param;
    }

    return NULL;
}

medToolBox* medAbstractProcess::toolbox()
{
    if(d->toolbox.isNull())
    {
        d->toolbox = new medToolBox;
        QFormLayout *layout = new QFormLayout(d->toolbox);
        foreach(medAbstractParameter *param, this->parameters())
        {
            layout->addRow(param->getLabel(), param->getWidget());
        }
    }
    return d->toolbox;
}
