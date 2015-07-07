/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractViewInteractor.h>

#include <medCoreExport.h>

class medAbstractLayeredView;
class medAbstractBoolParameter;

class medAbstractLayeredViewInteractorPrivate;
class MEDCORE_EXPORT medAbstractLayeredViewInteractor : public medAbstractViewInteractor
{
    Q_OBJECT

public:
             medAbstractLayeredViewInteractor(medAbstractView *parent);
    virtual ~medAbstractLayeredViewInteractor();

public:
    virtual medAbstractBoolParameter *visibilityParameter();

private slots:
    virtual void setVisibility(bool) = 0;


private:
    medAbstractLayeredViewInteractorPrivate *d;
};

