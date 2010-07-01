/* medMessageController.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Jun 28 09:57:25 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Jun 29 15:02:01 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 45
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
// medMessageControllerMessageSimple
// /////////////////////////////////////////////////////////////////

class medMessageControllerMessageSimple : public medMessageControllerMessage
{
    Q_OBJECT

public:
     medMessageControllerMessageSimple(const QString& text, QWidget *parent = 0);
    ~medMessageControllerMessageSimple(void);
protected:
    QLabel * icon;
};

// /////////////////////////////////////////////////////////////////
// medMessageControllerMessageInfo
// /////////////////////////////////////////////////////////////////

class medMessageControllerMessageInfo : public medMessageControllerMessageSimple
{
    Q_OBJECT

public:
     medMessageControllerMessageInfo(const QString& text, QWidget *parent = 0);
    ~medMessageControllerMessageInfo(void);
};

// /////////////////////////////////////////////////////////////////
// medMessageControllerMessageError
// /////////////////////////////////////////////////////////////////

class medMessageControllerMessageError : public medMessageControllerMessageSimple
{
    Q_OBJECT
public:
     medMessageControllerMessageError(const QString& text, QWidget *parent = 0);
    ~medMessageControllerMessageError(void);
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

    void attach(QStatusBar *status);

public slots:
    void     showInfo(QObject *sender, const QString& text);
    void     showError(QObject *sender, const QString& text);
    void showProgress(QObject *sender, const QString& text);

    void setProgress(int value);

    void remove(QObject *sender);

protected:
     medMessageController(void);
    ~medMessageController(void);

protected:
    static medMessageController *s_instance;

private:
    medMessageControllerPrivate *d;
};

#endif
