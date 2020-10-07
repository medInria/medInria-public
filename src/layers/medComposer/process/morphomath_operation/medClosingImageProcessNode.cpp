/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medClosingImageProcessNode.h>

#include <medCore.h>

medClosingImageProcessNode::medClosingImageProcessNode()
{
    this->setFactory(medCore::morphomathOperation::closingImage::pluginFactory());
}


