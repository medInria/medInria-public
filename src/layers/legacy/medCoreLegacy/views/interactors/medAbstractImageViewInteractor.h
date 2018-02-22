/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractLayeredViewInteractor.h>
#include <medImageViewEnum.h>

#include <medCoreLegacyExport.h>

class medAbstractImageView;
class medDoubleParameterL;
class medCompositeParameterL;

class medAbstractImageViewInteractorPrivate;
class MEDCORELEGACY_EXPORT medAbstractImageViewInteractor : public medAbstractLayeredViewInteractor
{
    Q_OBJECT

public:
             medAbstractImageViewInteractor(medAbstractView* parent);
    virtual ~medAbstractImageViewInteractor();

    virtual medDoubleParameterL* opacityParameter();
    virtual medCompositeParameterL* windowLevelParameter();

public slots:
    virtual void setOpacity(double opacity) = 0;
    virtual void setWindowLevel(QHash<QString,QVariant>) = 0;
    virtual void setCurrentTime(double time);

private:
    medAbstractImageViewInteractorPrivate *d;
};
