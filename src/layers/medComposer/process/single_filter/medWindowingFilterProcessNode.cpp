/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medWindowingFilterProcessNode.h>

#include <medCore.h>
#include <medWidgets.h>

medWindowingFilterProcessNode::medWindowingFilterProcessNode()
{
    this->setFactory(medCore::singleFilterOperation::windowingFilter::pluginFactory());
}

QWidget* medWindowingFilterProcessNode::editor()
{
    medAbstractProcess* process = this->object();
    if (!process)
        return NULL;
    medAbstractProcessPresenter* presenter = medWidgets::singleFilterOperation::windowingFilter::presenterFactory().create(process);
    return presenter->buildToolBoxWidget();
}
