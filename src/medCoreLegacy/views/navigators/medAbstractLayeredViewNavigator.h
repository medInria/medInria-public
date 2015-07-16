/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractViewNavigator.h>

#include <medCoreExport.h>

class MEDCORE_EXPORT medAbstractLayeredViewNavigator : public medAbstractViewNavigator
{
    Q_OBJECT

public:
             medAbstractLayeredViewNavigator(medAbstractView* parent);
    virtual ~medAbstractLayeredViewNavigator();
};
