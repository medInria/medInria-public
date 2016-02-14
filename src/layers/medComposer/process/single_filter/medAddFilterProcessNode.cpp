/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAddFilterProcessNode.h>

#include <medAbstractAddFilterProcessPresenter.h>

#include <medCore.h>
#include <medWidgets.h>

#include <QCheckBox>

class medAddFilterProcessNodePrivate
{
public:
    dtkComposerTransmitterReceiver<double> sigma;
};

medAddFilterProcessNode::medAddFilterProcessNode() : d(new medAddFilterProcessNodePrivate())
{
    this->setFactory(medCore::singleFilterOperation::addFilter::pluginFactory());

    this->appendReceiver(&d->sigma);
}

bool medAddFilterProcessNode::prepareInput(void)
{
    bool status = medSingleFilterOperationProcessNode<medAbstractAddFilterProcess>::prepareInput();

    if (status)
    {
        if(!d->sigma.isEmpty())
        {

            if(this->object())
            {
                medAbstractAddFilterProcess* const filter = this->object();
                filter->sigma()->setValue(d->sigma.data());
            }
        }
    }

    return status;
}


QWidget *medAddFilterProcessNode::editor(void)
{
    medAbstractProcess* process = this->object();
    if (!process)
        return NULL;
    medAbstractAddFilterProcessPresenter* presenter = medWidgets::singleFilterOperation::addFilter::presenterFactory().create(process);
    return presenter->buildToolBoxWidget();
}
