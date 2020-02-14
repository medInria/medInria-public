/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medMultiplyFilterProcessNode.h>

#include <medCore.h>
#include <medWidgets.h>


medMultiplyFilterProcessNode::medMultiplyFilterProcessNode()
{
    this->setFactory(medCore::singleFilterOperation::multiplyFilter::pluginFactory());
}

QWidget* medMultiplyFilterProcessNode::editor()
{
    medAbstractProcess* process = this->object();
    if (!process)
    {
        return nullptr;
    }
    medAbstractProcessPresenter* presenter = medWidgets::singleFilterOperation::multiplyFilter::presenterFactory().create(process);
    return presenter->buildToolBoxWidget();
}
