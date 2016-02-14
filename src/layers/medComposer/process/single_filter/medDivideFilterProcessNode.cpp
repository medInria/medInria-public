/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDivideFilterProcessNode.h>

#include <medCore.h>
#include <medWidgets.h>

medDivideFilterProcessNode::medDivideFilterProcessNode()
{
    this->setFactory(medCore::singleFilterOperation::divideFilter::pluginFactory());
}

QWidget* medDivideFilterProcessNode::editor()
{
    medAbstractProcess* process = this->object();
    if (!process)
        return NULL;
    medAbstractProcessPresenter* presenter = medWidgets::singleFilterOperation::divideFilter::presenterFactory().create(process);
    return presenter->buildToolBoxWidget();
}
