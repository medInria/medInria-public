/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medToolBox.h>
#include <medCoreExport.h>

class medRegistrationSelectorToolBox;
class medRegistrationAbstractToolBoxPrivate;

class MEDCORE_EXPORT medRegistrationAbstractToolBox : public medToolBox
{
    Q_OBJECT

public:
             medRegistrationAbstractToolBox(QWidget *parentToolBox = 0);
    virtual ~medRegistrationAbstractToolBox();

    virtual void setRegistrationToolBox(medRegistrationSelectorToolBox *toolbox);

protected:
    medRegistrationSelectorToolBox *parentToolBox();

private:
    medRegistrationAbstractToolBoxPrivate *d;
};


