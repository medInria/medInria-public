/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <medDiffusionModelEstimationProcessNode.h>

#include <dtkComposer>
#include <dtkLog>

#include <medAbstractDiffusionModelImageData.h>
#include <medBoolParameter.h>

class medDiffusionModelEstimationProcessNodePrivate
{
public:
    dtkComposerTransmitterReceiver<medAbstractImageData *> input;
    dtkComposerTransmitterReceiver<QString> gradients;
    dtkComposerTransmitterReceiver<bool> gradientsInImageCoordinates;
    dtkComposerTransmitterReceiver<QString> bvalues;

    dtkComposerTransmitterEmitter<medAbstractDiffusionModelImageData *> output;
};

medDiffusionModelEstimationProcessNode::medDiffusionModelEstimationProcessNode()
    : dtkComposerNodeObject<medAbstractDiffusionModelEstimationProcess>(), d(new medDiffusionModelEstimationProcessNodePrivate())
{
    this->appendReceiver(&d->input);
    this->appendReceiver(&d->gradients);
    this->appendReceiver(&d->gradientsInImageCoordinates);
    this->appendReceiver(&d->bvalues);

    this->appendEmitter(&d->output);
}

medDiffusionModelEstimationProcessNode::~medDiffusionModelEstimationProcessNode()
{
}

void medDiffusionModelEstimationProcessNode::run()
{
    if (d->input.isEmpty() || d->gradients.isEmpty())
    {
        dtkDebug() << Q_FUNC_INFO << "Missing inputs. Aborting.";
        return;
    }

    medAbstractDiffusionModelEstimationProcess *filter = this->object();
    if (this->object())
    {
        filter->setInput(d->input.data());
        filter->setGradients(d->gradients.data());
        filter->gradientsInImageCoordinates()->setValue(d->gradientsInImageCoordinates.data());
        filter->setBValues(d->bvalues.data());

        filter->run();
        d->output.setData(filter->output());
    }
}

