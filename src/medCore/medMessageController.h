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
// medMessage
// /////////////////////////////////////////////////////////////////

class MEDCORE_EXPORT medMessage : public QWidget
{
    Q_OBJECT

public:
     medMessage(QWidget *parent = 0, const QString& text=NULL, unsigned int timeout=0, bool progress=false);
    ~medMessage(void);

	    
protected:
	QProgressBar *progress;
	QLabel *icon;
	QTimer *timer;
	QLabel *info;


protected slots:
    void remove(void);
};



// /////////////////////////////////////////////////////////////////
// medMessageInfo
// /////////////////////////////////////////////////////////////////

class medMessageInfo : public medMessage
{
    Q_OBJECT

public:
     medMessageInfo(const QString& text, QWidget *parent = 0,
                                     unsigned int timeout=0);
    ~medMessageInfo(void);
};

// /////////////////////////////////////////////////////////////////
// medMessageError
// /////////////////////////////////////////////////////////////////

class medMessageError : public medMessage
{
    Q_OBJECT
public:
     medMessageError(const QString& text, QWidget *parent = 0,
                                      unsigned int timeout=0);
    ~medMessageError(void);
};

// /////////////////////////////////////////////////////////////////
// medMessageProgress
// /////////////////////////////////////////////////////////////////


class MEDCORE_EXPORT medMessageProgress : public medMessage
{
    Q_OBJECT

public:
     medMessageProgress(const QString& text, QWidget *parent = 0);
    ~medMessageProgress(void);


public slots:
    void setProgress(int value);
	void success(void);
	void failure(void);

};

// /////////////////////////////////////////////////////////////////
// medMessageQuestion
// /////////////////////////////////////////////////////////////////

class medMessageQuestionPrivate;

class MEDCORE_EXPORT medMessageQuestion : public medMessage
{
    Q_OBJECT

public:
     medMessageQuestion(const QString& text, QWidget *parent = 0);
    ~medMessageQuestion(void);

signals:
    void accepted(void);
    void rejected(void);

private:
    medMessageQuestionPrivate *d;
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
    medMessageProgress * showProgress(const QString& text);

	void remove(medMessage *message);

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
