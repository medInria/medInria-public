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

    QWidget* toolBoxWidget();
    QWidget* toolBarWidget();

    virtual QList<medAbstractParameter*> linkableParameters() = 0;

protected:
    /**
     * @brief buildToolBoxWidget reimplement it to construct and return the widget displayed in the
     * view settings toolBox when the container of the parent view is single selected.
     * @return
     */
    virtual QWidget* buildToolBoxWidget() = 0;
    /**
     * @brief buildToolBoxWidget reimplement it to construct and return the widget displayed in the
     * toolbar of the container where the parent view is dsiplayed.
     * @return
     */
    virtual QWidget* buildToolBarWidget() = 0;

private slots:
    void removeInternToolBoxWidget();
    void removeInternToolBarWidget();

private:
    medAbstractNavigatorPrivate *d;

/*=========================================================================
                        NOT IMPLEMENTED HERE ON PURPOSE
*=========================================================================*/
public slots:
    /**
     * @brief updateWidgets reimplemts it to reupdate your
     * widgets when data in view or orientation change.
     */
    virtual void updateWidgets(){}

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
