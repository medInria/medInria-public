/* medMessageController.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Jun 28 09:57:25 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Oct  7 12:33:38 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 56
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

#include "medCoreExport.h"

class medMessageControllerPrivate;

// /////////////////////////////////////////////////////////////////
// medMessageControllerMessage
// /////////////////////////////////////////////////////////////////

class MEDCORE_EXPORT medMessageControllerMessage : public QWidget
{
    Q_OBJECT

public:
     medMessageControllerMessage(QObject* sender, QWidget *parent = 0, const QString& text=NULL, unsigned int timeout=0, bool progress=false);
    ~medMessageControllerMessage(void);

    QWidget * getWidget();

	    
protected:
    QObject* sender;
	QProgressBar *progress;
	QLabel *icon;
	QTimer *timer;
	QLabel *info;

    //The widget containing the message
    QWidget* widget;

protected slots:
    void remove(void);
};



// /////////////////////////////////////////////////////////////////
// medMessageControllerMessageInfo
// /////////////////////////////////////////////////////////////////

class medMessageControllerMessageInfo : public medMessageControllerMessage
{
    Q_OBJECT

public:
     medMessageControllerMessageInfo(const QString& text, QWidget *parent = 0,
                                     unsigned int timeout=0);
    ~medMessageControllerMessageInfo(void);
};

// /////////////////////////////////////////////////////////////////
// medMessageControllerMessageError
// /////////////////////////////////////////////////////////////////

class medMessageControllerMessageError : public medMessageControllerMessage
{
    Q_OBJECT
public:
     medMessageControllerMessageError(const QString& text, QWidget *parent = 0,
                                      unsigned int timeout=0);
    ~medMessageControllerMessageError(void);
};

// /////////////////////////////////////////////////////////////////
// medMessageControllerMessageProgress
// /////////////////////////////////////////////////////////////////


class MEDCORE_EXPORT medMessageControllerMessageProgress : public medMessageControllerMessage
{
    Q_OBJECT

public:
     medMessageControllerMessageProgress(QObject* sender,const QString& text, QWidget *parent = 0);
    ~medMessageControllerMessageProgress(void);

	void successMessage();
	void abortMessage();

public slots:
    void setProgress(int value);

};

// /////////////////////////////////////////////////////////////////
// medMessageControllerMessageQuestion
// /////////////////////////////////////////////////////////////////

class medMessageControllerMessageQuestionPrivate;

class MEDCORE_EXPORT medMessageControllerMessageQuestion : public medMessageControllerMessage
{
    Q_OBJECT

public:
     medMessageControllerMessageQuestion(QObject* sender, const QString& text, QWidget *parent = 0);
    ~medMessageControllerMessageQuestion(void);

signals:
    void accepted(void);
    void rejected(void);

private:
    medMessageControllerMessageQuestionPrivate *d;
};

// /////////////////////////////////////////////////////////////////
// medMessageController
// /////////////////////////////////////////////////////////////////

class MEDCORE_EXPORT medMessageController : public QObject
{
    Q_OBJECT

public:
    static medMessageController *instance(void);

public slots:
    void     showInfo(const QString& text,unsigned int timeout=0);
    void     showError(const QString& text,unsigned int timeout=0);
    void     showProgress(QObject *sender, const QString& text);

    void setProgress(int value);
	void success(QObject *sender);
	void failure (QObject *sender);

    void remove(QObject *sender);
	void remove(medMessageControllerMessage *message);

signals:
  void addMessage(QWidget * message);
  void removeMessage(QWidget * message);

protected:
     medMessageController(void);
    ~medMessageController(void);

protected:
    static medMessageController *s_instance;

private:
    medMessageControllerPrivate *d;
};

#endif
