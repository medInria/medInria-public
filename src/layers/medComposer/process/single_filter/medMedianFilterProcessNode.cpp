/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medMedianFilterProcessNode.h>

#include <medCore.h>
#include <medWidgets.h>

medMedianFilterProcessNode::medMedianFilterProcessNode()
{
    this->setFactory(medCore::singleFilterOperation::medianFilter::pluginFactory());
}

QWidget* medMedianFilterProcessNode::editor()
{
    medAbstractProcess* process = this->object();
    if (!process)
        return NULL;
    medAbstractProcessPresenter* presenter = medWidgets::singleFilterOperation::medianFilter::presenterFactory().create(process);
    return presenter->buildToolBoxWidget();
}

