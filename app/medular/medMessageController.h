/* medMessageController.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Jun 28 09:57:25 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Mon Jun 28 16:23:14 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 40
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDMESSAGECONTROLLER_H
#define MEDMESSAGECONTROLLER_H

#include <QtCore>
#include <QtGui>

class medBrowserArea;
class medViewerArea;
class medMessageControllerPrivate;

// /////////////////////////////////////////////////////////////////
// medMessageControllerMessage
// /////////////////////////////////////////////////////////////////

class medMessageControllerMessage : public QWidget
{
    Q_OBJECT

public:
     medMessageControllerMessage(QWidget *parent = 0);
    ~medMessageControllerMessage(void);
};

// /////////////////////////////////////////////////////////////////
// medMessageControllerMessageInfo
// /////////////////////////////////////////////////////////////////

class medMessageControllerMessageInfo : public medMessageControllerMessage
{
    Q_OBJECT

public:
     medMessageControllerMessageInfo(const QString& text, QWidget *parent = 0);
    ~medMessageControllerMessageInfo(void);
};

// /////////////////////////////////////////////////////////////////
// medMessageControllerMessageProgress
// /////////////////////////////////////////////////////////////////

class medMessageControllerMessageProgressPrivate;

class medMessageControllerMessageProgress : public medMessageControllerMessage
{
    Q_OBJECT

public:
     medMessageControllerMessageProgress(const QString& text, QWidget *parent = 0);
    ~medMessageControllerMessageProgress(void);

public slots:
    void setProgress(int value);

private:
    medMessageControllerMessageProgressPrivate *d;
};

// /////////////////////////////////////////////////////////////////
// medMessageController
// /////////////////////////////////////////////////////////////////

class medMessageController : public QObject
{
    Q_OBJECT

public:
    static medMessageController *instance(void);

    void attach(medBrowserArea *browser);
    void attach(medViewerArea *viewer);

public slots:
    int     showInfo(const QString& text);
    int showProgress(const QString& text);

    void setProgress(int id, int value);

    void remove(int id);

protected:
     medMessageController(void);
    ~medMessageController(void);

protected:
    static medMessageController *s_instance;

private:
    medMessageControllerPrivate *d;
};

#endif
