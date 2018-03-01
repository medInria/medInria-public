/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSubtractFilterProcessNode.h>

#include <medCore.h>
#include <medWidgets.h>

medSubtractFilterProcessNode::medSubtractFilterProcessNode()
{
    this->setFactory(medCore::singleFilterOperation::subtractFilter::pluginFactory());
}

QWidget* medSubtractFilterProcessNode::editor()
{
    medAbstractProcess* process = this->object();
    if (!process)
        return NULL;
    medAbstractProcessPresenter* presenter = medWidgets::singleFilterOperation::subtractFilter::presenterFactory().create(process);
    return presenter->buildToolBoxWidget();
}
