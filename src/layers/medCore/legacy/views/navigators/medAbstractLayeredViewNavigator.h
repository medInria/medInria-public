#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractViewNavigator.h>
#include <medCoreLegacyExport.h>

class MEDCORELEGACY_EXPORT medAbstractLayeredViewNavigator : public medAbstractViewNavigator
{
    Q_OBJECT

public:
    medAbstractLayeredViewNavigator(medAbstractView* parent);
    virtual ~medAbstractLayeredViewNavigator();
};
