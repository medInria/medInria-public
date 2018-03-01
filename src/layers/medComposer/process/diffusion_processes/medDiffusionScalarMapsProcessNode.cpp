/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <medDiffusionScalarMapsProcessNode.h>

#include <dtkComposer>
#include <dtkLog>

#include <medAbstractDiffusionModelImageData.h>
#include <medCore.h>
#include <medWidgets.h>

class medDiffusionScalarMapsProcessNodePrivate
{
public:
    dtkComposerTransmitterReceiver<medAbstractDiffusionModelImageData *> input;
    dtkComposerTransmitterEmitter<medAbstractImageData *> output;
};

medDiffusionScalarMapsProcessNode::medDiffusionScalarMapsProcessNode()
    : dtkComposerNodeObject<medAbstractDiffusionScalarMapsProcess>(), d(new medDiffusionScalarMapsProcessNodePrivate())
{
    this->appendReceiver(&d->input);
    this->appendEmitter(&d->output);
    this->setFactory(medCore::diffusionScalarMaps::pluginFactory());
}

medDiffusionScalarMapsProcessNode::~medDiffusionScalarMapsProcessNode()
{
}

void medDiffusionScalarMapsProcessNode::run()
{
    if (d->input.isEmpty())
    {
        dtkDebug() << Q_FUNC_INFO << "Missing input. Aborting.";
        return;
    }

    medAbstractDiffusionScalarMapsProcess *filter = this->object();
    if (this->object())
    {
        filter->setInput(d->input.data());
        filter->run();
        d->output.setData(filter->output());
    }
}

QWidget* medDiffusionScalarMapsProcessNode::editor()
{
    medAbstractProcess* process = this->object();
    if (!process)
        return NULL;
    medAbstractProcessPresenter* presenter = medWidgets::diffusionScalarMaps::presenterFactory().create(process);
    return presenter->buildToolBoxWidget();
}
