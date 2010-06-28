/* medMessageController.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Jun 28 09:57:25 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Mon Jun 28 13:06:51 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 22
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDMESSAGECONTROLLER_H
#define MEDMESSAGECONTROLLER_H

#include <QtCore/QObject>

class medBrowserArea;
class medViewerArea;
class medMessageControllerPrivate;

class medMessageController : public QObject
{
    Q_OBJECT

public:
    static medMessageController *instance(void);

    enum Area {
        Browser,
        Viewer
    };

    void attach(medBrowserArea *browser);
    void attach(medViewerArea *viewer);

public slots:
    int     showInfo(Area area, const QString& text, int timeout = 0);
    int showProgress(Area area, const QString& text);

protected:
     medMessageController(void);
    ~medMessageController(void);

protected:
    static medMessageController *s_instance;

private:
    medMessageControllerPrivate *d;
};

#endif
