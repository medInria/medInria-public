/* medMessageController.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Jun 28 09:59:08 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Mon Jun 28 14:44:18 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 39
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medBrowserArea.h"
#include "medMessageController.h"
#include "medViewerArea.h"

#include <QtGui>

// /////////////////////////////////////////////////////////////////
// medMessageControllerMessage
// /////////////////////////////////////////////////////////////////

class medMessageControllerMessage : public QWidget
{
    Q_OBJECT

public:
     medMessageControllerMessage(void) {}
    ~medMessageControllerMessage(void) {}
};

class medMessageControllerMessageInfo : public medMessageControllerMessage
{
    Q_OBJECT

public:
     medMessageControllerMessageInfo(void) {}
    ~medMessageControllerMessageInfo(void) {}
};

class medMessageControllerMessageProgress : public medMessageControllerMessage
{
    Q_OBJECT

public:
     medMessageControllerMessageProgress(void) {}
    ~medMessageControllerMessageProgress(void) {}
};

// /////////////////////////////////////////////////////////////////
// medMessageController
// /////////////////////////////////////////////////////////////////

class medMessageControllerPrivate
{
public:
    medBrowserArea *browser;
    medViewerArea *viewer;
};

medMessageController *medMessageController::instance(void)
{
    if(!s_instance)
        s_instance = new medMessageController;

    return s_instance;
}

void medMessageController::attach(medBrowserArea *browser)
{
    d->browser = browser;
}

void medMessageController::attach(medViewerArea *viewer)
{
    d->viewer = viewer;
}

int medMessageController::showInfo(Area area, const QString& text, int timeout)
{
    Q_UNUSED(text);
    Q_UNUSED(timeout);

    switch(area) {
    case Browser:
        break;
    case Viewer:
        break;
    default:
        break;
    }
}

int medMessageController::showProgress(Area area, const QString& text)
{
    Q_UNUSED(text);

    switch(area) {
    case Browser:
        break;
    case Viewer:
        break;
    default:
        break;
    }
}

medMessageController::medMessageController(void) : QObject(), d(new medMessageControllerPrivate)
{
    d->browser = NULL;
    d->viewer = NULL;
}

medMessageController::~medMessageController(void)
{
    delete d;

    d = NULL;
}

medMessageController *medMessageController::s_instance = NULL;
