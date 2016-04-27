/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <medDWIMaskingProcessNode.h>

#include <dtkComposer>
#include <dtkLog>

#include <medAbstractImageData.h>
#include <medCore.h>
#include <medWidgets.h>

class medDWIMaskingProcessNodePrivate
{
public:
    dtkComposerTransmitterReceiver<medAbstractImageData*> input;
    dtkComposerTransmitterEmitter<medAbstractImageData*> output;
};

medDWIMaskingProcessNode::medDWIMaskingProcessNode()
    : dtkComposerNodeObject<medAbstractDWIMaskingProcess>(), d(new medDWIMaskingProcessNodePrivate())
{
    this->appendReceiver(&d->input);
    this->appendEmitter (&d->output);
    this->setFactory(medCore::dwiMasking::pluginFactory());
}

medDWIMaskingProcessNode::~medDWIMaskingProcessNode()
{
}

void medDWIMaskingProcessNode::run()
{
    if (d->input.isEmpty())
    {
        dtkDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }

    medAbstractDWIMaskingProcess *filter = this->object();
    if (this->object())
    {
        filter->setInput(d->input.data());
        filter->run();
        d->output.setData(filter->output());
    }
}

QWidget* medDWIMaskingProcessNode::editor()
{
    medAbstractProcess* process = this->object();
    if (!process)
        return NULL;
    medAbstractProcessPresenter* presenter = medWidgets::dwiMasking::presenterFactory().create(process);
    return presenter->buildToolBoxWidget();
}
