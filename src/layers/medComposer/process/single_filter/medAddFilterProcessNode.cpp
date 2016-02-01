/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAddFilterProcessNode.h>

#include <medCore.h>

medAddFilterProcessNode::medAddFilterProcessNode()
{
    this->setFactory(medCore::singleFilterOperation::addFilter::pluginFactory());
}


