/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractInteractor.h>

#include <medCoreExport.h>

#include <medAbstractView.h>

class MEDCORE_EXPORT medAbstractViewInteractor : public medAbstractInteractor
{
    Q_OBJECT
public:
            medAbstractViewInteractor(medAbstractView* parent);
    virtual ~medAbstractViewInteractor();

    /**
     * This method is called from medAbstractView::generateThumbnail
     * Implement it to set up your view correctly before generating thumbnail
     */
    virtual void setUpViewForThumbnail() = 0;

    virtual void removeData() = 0;

};
