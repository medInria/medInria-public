#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractViewInteractor.h>
#include <medCoreLegacyExport.h>

class medAbstractLayeredView;
class medAbstractBoolParameterL;

class medAbstractLayeredViewInteractorPrivate;
class MEDCORELEGACY_EXPORT medAbstractLayeredViewInteractor : public medAbstractViewInteractor
{
    Q_OBJECT

public:
    medAbstractLayeredViewInteractor(medAbstractView *parent);
    virtual ~medAbstractLayeredViewInteractor();

public:
    virtual medAbstractBoolParameterL *visibilityParameter();

private slots:
    virtual void setVisibility(bool) = 0;

private:
    medAbstractLayeredViewInteractorPrivate *d;
};
