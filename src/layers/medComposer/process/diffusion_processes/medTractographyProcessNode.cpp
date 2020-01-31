/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <medTractographyProcessNode.h>

#include <dtkComposer>
#include <dtkLog>

#include <medAbstractDiffusionModelImageData.h>
#include <medAbstractFibersData.h>
#include <medCore.h>
#include <medWidgets.h>

class medTractographyProcessNodePrivate
{
public:
    dtkComposerTransmitterReceiver<medAbstractDiffusionModelImageData *> input;
    dtkComposerTransmitterEmitter<medAbstractFibersData *> output;
};

medTractographyProcessNode::medTractographyProcessNode()
    : dtkComposerNodeObject<medAbstractTractographyProcess>(), d(new medTractographyProcessNodePrivate())
{
    this->appendReceiver(&d->input);
    this->appendEmitter(&d->output);

    this->setFactory(medCore::tractography::pluginFactory());
}

medTractographyProcessNode::~medTractographyProcessNode()
{
}

void medTractographyProcessNode::run()
{
    if (d->input.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "Missing inputs. Aborting.";
        return;
    }

    medAbstractTractographyProcess *filter = this->object();
    if (this->object())
    {
        filter->setInput(d->input.data());
        filter->run();
        d->output.setData(filter->output());
    }
}

QWidget* medTractographyProcessNode::editor()
{
    medAbstractProcess* process = this->object();
    if (!process)
        return NULL;
    medAbstractProcessPresenter* presenter = medWidgets::tractography::presenterFactory().create(process);
    return presenter->buildToolBoxWidget();
}
