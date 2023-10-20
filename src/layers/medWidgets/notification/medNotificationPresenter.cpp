/*
 * medInria
 * Copyright (c) INRIA 2013. All rights reserved.
 * 
 * medInria is under BSD-2-Clause license. See LICENSE.txt for details in the root of the sources or:
 * https://github.com/medInria/medInria-public/blob/master/LICENSE.txt
 * 
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "medNotificationPresenter.h"

#include <medNotifWindow.h>
#include <medNotifStatusbarWidget.h>


class medNotifSysPresenterPrivate
{
public:
    medNotifSys * pNotifSys;
};

medNotifSysPresenter::medNotifSysPresenter(medNotifSys * parent) : d(new medNotifSysPresenterPrivate())
{
    d->pNotifSys = parent;
}

medNotifSysPresenter::~medNotifSysPresenter()
{
    delete d;
    d = nullptr;
}

QWidget * medNotifSysPresenter::buildWidget()
{
    return buildNotificationWindow();
}

QWidget * medNotifSysPresenter::buildNotificationWindow()
{

    return new medNotificationPaneWidget(d->pNotifSys);
}

QWidget * medNotifSysPresenter::buildWidgetForStatusBar()
{
    return new medNotifStatusbarWidget(d->pNotifSys);
}

