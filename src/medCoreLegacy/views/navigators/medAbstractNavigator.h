/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCoreSupport/dtkAbstractViewNavigator.h>

#include <medCoreExport.h>
#include <medImageViewEnum.h>

class medAbstractView;
class medAbstractParameter;
class medBoolParameter;


class medAbstractNavigatorPrivate;
class MEDCORE_EXPORT medAbstractNavigator : public dtkAbstractViewNavigator
{
    Q_OBJECT

public:
             medAbstractNavigator(medAbstractView* parent);
    virtual ~medAbstractNavigator();

public:
    virtual QString description() const = 0;

    QWidget* toolBoxWidget();
    QWidget* toolBarWidget();

    virtual QList<medAbstractParameter*> linkableParameters() = 0;
    virtual QList<medBoolParameter*> mouseInteractionParameters() = 0;

protected:
    virtual QWidget* buildToolBoxWidget() = 0;
    virtual QWidget* buildToolBarWidget() = 0;

private slots:


private:
    medAbstractNavigatorPrivate *d;

/*=========================================================================
                        NOT IMPLEMENTED HERE ON PURPOSE
*=========================================================================*/
public slots:
    virtual void updateWidgets();

private:
    using dtkAbstractViewNavigator::setView;
    using dtkAbstractViewNavigator::view;

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
