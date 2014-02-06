/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractViewInteractor.h>

#include <medCoreExport.h>

#include <QWidget>

#include <medAbstractView.h>
#include <medAbstractData.h>
#include <medImageViewEnum.h>
#include <medDataEnum.h>

class medAbstractParameter;

class medAbstractInteractorPrivate;
class MEDCORE_EXPORT medAbstractInteractor : public dtkAbstractViewInteractor
{
    Q_OBJECT

public:
             medAbstractInteractor(medAbstractView* parent);
    virtual ~medAbstractInteractor();

public:
    virtual QString description() const = 0;

    virtual void setView(medAbstractView*view);
    virtual medAbstractView *view() const;

    virtual void setData(medAbstractData *data);
    virtual medAbstractData *data() const;

    virtual QWidget* toolBoxWidget() = 0;
    virtual QWidget* toolBarWidget() = 0;
    virtual QWidget* layerWidget() = 0;

    virtual QList<medAbstractParameter*> parameters() = 0;

public slots:
    virtual void updateWidgets(){}

private:
    medAbstractInteractorPrivate *d;

/*=========================================================================
                         NOT IMPLEMENTED ON PURPOSE
*=========================================================================*/
private:
    virtual dtkAbstractData *output (void);
    virtual dtkAbstractData *output (int channel);
    virtual dtkAbstractData *output (int channel, int frame);
    virtual void predraw(void);
    virtual void draw(void);
    virtual void postdraw(void);
    bool enabled(void) const;
    virtual void enable(void);
    virtual void disable(void);
    virtual void setData(dtkAbstractData* data, int channel);
    virtual void setData(dtkAbstractData* data, int channel, int frame);

};
