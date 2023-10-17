#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include <medCoreExport.h>

#include <memory>

class medMessageControllerPrivate;

// /////////////////////////////////////////////////////////////////
// medMessage
// /////////////////////////////////////////////////////////////////

class MEDCORE_EXPORT medMessage : public QWidget
{
    Q_OBJECT

public:
     medMessage(QWidget *parent = nullptr, const QString& text=nullptr, unsigned int timeout=0);
    ~medMessage() = default;
    void startTimer();
    void stopTimer();


protected:
    QLabel *icon;
    QTimer *timer;
    int timeout;
    QLabel *info;


protected slots:
    void remove();
};

// /////////////////////////////////////////////////////////////////
// medMessageInfo
// /////////////////////////////////////////////////////////////////

class medMessageInfo : public medMessage
{
    Q_OBJECT

public:
     medMessageInfo(const QString& text, QWidget *parent = nullptr,
                                     unsigned int timeout=0);
    ~medMessageInfo() = default;
};

// /////////////////////////////////////////////////////////////////
// medMessageError
// /////////////////////////////////////////////////////////////////

class medMessageError : public medMessage
{
    Q_OBJECT
public:
     medMessageError(const QString& text, QWidget *parent = nullptr,
                                      unsigned int timeout=0);
    ~medMessageError() = default;
};

// /////////////////////////////////////////////////////////////////
// medMessageProgress
// /////////////////////////////////////////////////////////////////

class MEDCORE_EXPORT medMessageProgress : public medMessage
{
    Q_OBJECT

public:
     medMessageProgress(const QString& text, QWidget *parent = nullptr);
    ~medMessageProgress() = default;
    void associateTimer();
    void paintEvent ( QPaintEvent * event );

protected:
    QProgressBar *progress;

public slots:
    void setProgress(int value);
    void success();
    void failure();
};

// /////////////////////////////////////////////////////////////////
// medMessageController
// /////////////////////////////////////////////////////////////////

class MEDCORE_EXPORT medMessageController : public QObject
{
    Q_OBJECT

public:
    ~medMessageController() = default;
    static medMessageController &instance();

public slots:
    void     showInfo(const QString& text,unsigned int timeout=0);
    void     showError(const QString& text,unsigned int timeout=0);
    medMessageProgress * showProgress(const QString& text);

    void remove(medMessage *message);

signals:
  void addMessage(medMessage * message);
  void removeMessage(medMessage * message);

protected:
    medMessageController();

    static std::unique_ptr<medMessageController> s_instance;

private:
    medMessageControllerPrivate *d;
};
