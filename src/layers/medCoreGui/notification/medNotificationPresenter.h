#pragma once
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

#include <QObject>

#include <medNotifSys.h>
#include <medCoreGuiExport.h>

class QWidget;
class medNotifSysPresenterPrivate;
class MEDCOREGUI_EXPORT medNotifSysPresenter : public QObject
{
    Q_OBJECT

public:

    medNotifSysPresenter(medNotifSys * parent);
    ~medNotifSysPresenter();

    QWidget * buildWidget();
    QWidget * buildNotificationWindow();
    QWidget * buildWidgetForStatusBar();

private:
    medNotifSysPresenterPrivate *d;
};
