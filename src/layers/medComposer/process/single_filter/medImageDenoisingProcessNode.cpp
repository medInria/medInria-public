/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medImageDenoisingProcessNode.h>

#include <medCore.h>
#include <medWidgets.h>

medImageDenoisingProcessNode::medImageDenoisingProcessNode()
{
    this->setFactory(medCore::singleFilterOperation::imageDenoising::pluginFactory());
}

QWidget* medImageDenoisingProcessNode::editor()
{
    medAbstractProcess* process = this->object();
    if (!process)
    {
        return nullptr;
    }
    medAbstractProcessPresenter* presenter = medWidgets::singleFilterOperation::imageDenoising::presenterFactory().create(process);
    return presenter->buildToolBoxWidget();
}
