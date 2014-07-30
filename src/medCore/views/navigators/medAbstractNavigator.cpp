/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractNavigator.h>

#include <medAbstractView.h>


/**
 * @fn QWidget* medAbstractNavigator::buildToolBoxWidget()
 * @brief buildToolBoxWidget reimplement it to construct and return the widget displayed in the
 * view settings toolBox when the container of the parent view is single selected.
 * @return
 */

/**
 * @fn QWidget* medAbstractNavigator::buildToolBarWidget()
 * @brief buildToolBarWidget reimplement it to construct and return the widget displayed in the
 * toolbar of the container where the parent view is dsiplayed.
 */



class medAbstractNavigatorPrivate
{
public:
    QPointer<QWidget> toolBoxWidget;
    QPointer<QWidget> toolBarWidget;
};

medAbstractNavigator::medAbstractNavigator(medAbstractView *parent):
    d(new medAbstractNavigatorPrivate)
{
    setParent(parent);
    d->toolBarWidget = NULL;
    d->toolBoxWidget = NULL;
}

medAbstractNavigator::~medAbstractNavigator()
{
    delete d;
}

QWidget* medAbstractNavigator::toolBoxWidget()
{
    if(d->toolBoxWidget.isNull())
    {
        d->toolBoxWidget = this->buildToolBoxWidget();
    }

    return d->toolBoxWidget;
}

QWidget* medAbstractNavigator::toolBarWidget()
{
    if(d->toolBarWidget.isNull())
    {
        d->toolBarWidget = this->buildToolBarWidget();
    }

    return d->toolBarWidget;
}

/**
 * @brief updateWidgets reimplemts it to reupdate your
 * widgets when data in view or orientation change.
 */
void medAbstractNavigator::updateWidgets()
{

}
