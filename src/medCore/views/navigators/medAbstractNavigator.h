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

    QWidget* toolBoxWidget();
    QWidget* toolBarWidget();


    virtual QList<medAbstractParameter*> parameters() = 0;


protected:
    virtual QWidget* buildToolBoxWidget() = 0;
    virtual QWidget* buildToolBarWidget() = 0;

private slots:
    void removeInternToolBoxWidget();
    void removeInternToolBarWidget();

private:
    medAbstractNavigatorPrivate *d;

/*=========================================================================
                         NOT IMPLEMENTED ON PURPOSE
*=========================================================================*/
private:
//    using dtkAbstractViewNavigator::setView;
//    using dtkAbstractViewNavigator::view;

    using dtkAbstractViewNavigator::setData;
    using dtkAbstractViewNavigator::data;
    using dtkAbstractViewNavigator::output;
    using dtkAbstractViewNavigator::predraw;
    using dtkAbstractViewNavigator::draw;
    using dtkAbstractViewNavigator::postdraw;
    using dtkAbstractViewNavigator::enabled;
    using dtkAbstractViewNavigator::enable;
    using dtkAbstractViewNavigator::disable;

};
