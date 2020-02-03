#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractLayeredViewInteractor.h>
#include <medCoreLegacyExport.h>
#include <medImageViewEnum.h>

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
