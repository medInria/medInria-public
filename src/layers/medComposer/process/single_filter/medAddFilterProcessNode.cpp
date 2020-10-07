/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
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


medAddFilterProcessNode::medAddFilterProcessNode()
{
    this->setFactory(medCore::singleFilterOperation::addFilter::pluginFactory());
}

QWidget* medAddFilterProcessNode::editor()
{
    medAbstractProcess* process = this->object();
    if (!process)
    {
        return nullptr;
    }
    medAbstractProcessPresenter* presenter = medWidgets::singleFilterOperation::addFilter::presenterFactory().create(process);
    return presenter->buildToolBoxWidget();
}
