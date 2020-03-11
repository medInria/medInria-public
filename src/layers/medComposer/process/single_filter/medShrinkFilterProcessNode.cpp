/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medShrinkFilterProcessNode.h>

#include <medCore.h>
#include <medWidgets.h>

medShrinkFilterProcessNode::medShrinkFilterProcessNode()
{
    this->setFactory(medCore::singleFilterOperation::shrinkFilter::pluginFactory());
}

QWidget* medShrinkFilterProcessNode::editor()
{
    medAbstractProcess* process = this->object();
    if (!process)
    {
        return nullptr;
    }
    medAbstractProcessPresenter* presenter = medWidgets::singleFilterOperation::shrinkFilter::presenterFactory().create(process);
    return presenter->buildToolBoxWidget();
}
