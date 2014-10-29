/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>

#include <QtGui/QWidget>

class medAbstractJob;
class medProgressionStackPrivate;

/**
 * @class medProgressionStack
 * @brief Creates JobWidgets that represent the progression of a jobItem.
 * All visible jobs will be managed here. The class stores pointers in hashTables to identify the sender of status messages
 * It provides methods to communicate with the jobItems (e.g. to cancel the job)
 */
class MEDCORE_EXPORT medProgressionStack : public QWidget
{
    Q_OBJECT

public:
     medProgressionStack(QWidget *parent = 0);
    ~medProgressionStack();

    QSize sizeHint() const;

signals:
    void shown();
    void hidden();
    void cancelRequest(QObject*);

public slots:

    void addJobItem(medAbstractJob* job, QString label);
    void setLabel(QObject *sender, QString label);
    void setActive(QObject* sender, bool active);
    void setProgress(QObject *sender, int progress);
    void onSuccess(QObject* sender);
    void onFailure(QObject* sender);
    void onCancel(QObject* sender);
    void removeItem();
    void disableCancel(QObject* sender);

private slots:
    void sendCancelRequest();

private:
    void completeNotification(QObject* sender, QString label);

    medProgressionStackPrivate *d;
};


