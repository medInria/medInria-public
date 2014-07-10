/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractLayeredViewInteractor.h>
#include <medImageViewEnum.h>

#include <medCoreExport.h>

class medAbstractImageView;
class medDoubleParameter;
class medCompositeParameter;

class medAbstractImageViewInteractorPrivate;
class MEDCORE_EXPORT medAbstractImageViewInteractor : public medAbstractLayeredViewInteractor
{
    Q_OBJECT

public:
             medAbstractImageViewInteractor(medAbstractView* parent);
    virtual ~medAbstractImageViewInteractor();

    virtual medDoubleParameter* opacityParameter();

    virtual medCompositeParameter* windowLevelParameter();

public slots:

    virtual void setOpacity (double opacity) = 0;

    virtual void setWindowLevel(QHash<QString,QVariant>) = 0;

    /**
    * @brief setCurrentTime Reimplement this methode if the data has a temporal component,
    * to update what to display in the view according to the parameter time.
    * Do not update the view, it has to be done by the primary navigator.
    * @param time
    */
    virtual void setCurrentTime(const double &time)
    {
        qDebug()<<"No implementation of setCurrentTime(const double &time) for" << this->identifier();
    }

private:
    medAbstractImageViewInteractorPrivate *d;
};
