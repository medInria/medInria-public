/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medNormalizeFilterProcessNode.h>

#include <medCore.h>
#include <medWidgets.h>

medNormalizeFilterProcessNode::medNormalizeFilterProcessNode()
{
    this->setFactory(medCore::singleFilterOperation::normalizeFilter::pluginFactory());
}

QWidget* medNormalizeFilterProcessNode::editor()
{
    medAbstractProcess* process = this->object();
    if (!process)
    {
        return nullptr;
    }
    medAbstractProcessPresenter* presenter = medWidgets::singleFilterOperation::normalizeFilter::presenterFactory().create(process);
    return presenter->buildToolBoxWidget();
}
