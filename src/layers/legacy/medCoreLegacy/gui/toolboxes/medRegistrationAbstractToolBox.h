/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medToolBox.h>

#include <medCoreLegacyExport.h>

class medRegistrationSelectorToolBox;
class medRegistrationAbstractToolBoxPrivate;

class MEDCORELEGACY_EXPORT medRegistrationAbstractToolBox : public medToolBox
{
    Q_OBJECT

public:
    medRegistrationAbstractToolBox(QWidget *parentToolBox = nullptr);

    virtual ~medRegistrationAbstractToolBox();

    virtual void setRegistrationToolBox(medRegistrationSelectorToolBox *toolbox);

    virtual medAbstractData* getOutput(){return nullptr;}

protected:
    medRegistrationSelectorToolBox *parentToolBox();

private:
    medRegistrationAbstractToolBoxPrivate *d;
};


