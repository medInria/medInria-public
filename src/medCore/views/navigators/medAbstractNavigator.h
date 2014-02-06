/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractViewNavigator.h>

#include <medCoreExport.h>
#include <medDataEnum.h>
#include <medImageViewEnum.h>

class medAbstractView;
class medAbstractParameter;


class medAbstractNavigatorPrivate;
class MEDCORE_EXPORT medAbstractNavigator : public dtkAbstractViewNavigator
{
    Q_OBJECT

public:
             medAbstractNavigator(medAbstractView* parent);
    virtual ~medAbstractNavigator();

public:
    virtual QString description() const = 0;

    virtual void setView(medAbstractView *view);
    virtual medAbstractView *view() const;

    virtual QWidget* widgetForToolBox() const = 0;
    virtual QWidget* widgetForToolBar() const = 0;

    virtual QList<medAbstractParameter*> parameters() = 0;

public slots:
    virtual void updateWidgets(){}

private:
    medAbstractNavigatorPrivate *d;

/*=========================================================================
                         NOT IMPLEMENTED ON PURPOSE
*=========================================================================*/
private:
    virtual void setData(dtkAbstractData *data);
    virtual dtkAbstractData* data();
    virtual dtkAbstractData *output (void);
    virtual dtkAbstractData *output (int channel);
    virtual dtkAbstractData *output (int channel, int frame);
    virtual void predraw(void);
    virtual void draw(void);
    virtual void postdraw(void);
    bool enabled(void) const;
    virtual void enable(void);
    virtual void disable(void);

};
