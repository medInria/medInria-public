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

class medJobItem;
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

    /**
    * AddJobItem - Add a new subclass of medJobItem to the Stack to create the connection between them
    * @param: medJobItem * job - instance of medJobItem
    * @param: QString label - the label shown on the jobToolBox
    * if no label was given the job will not be added
    */
    void addJobItem(medJobItem* job, QString label);

    void setLabel(QObject *sender, QString label);

    /**
     * @brief Sets the progress bar to move without knowing percentage.
     *
     * The developper can use the slot at any time to switch from a continuous
     * unknown progression phase to a step by step progression.
     *
     * @param sender the object which called the function, to identify the bar in the table.
     * @param active if true, the bar will move on its own to show activity.
     * If false, the progression will be set to a fraction of 100.
     */
    void setActive(QObject* sender, bool active);
    void setProgress(QObject *sender, int progress);
    void onSuccess(QObject* sender);
    void onFailure(QObject* sender);
    void onCancel(QObject* sender);
    void removeItem();

    /**
    * Modifies the GUI so as there is no way of (attempting) to cancel
    * the job anymore, as now it is not cancellable.
    * @param QObject* sender
    */
    void disableCancel(QObject* sender);

private slots:
    /** send a cancel request to the job that the cancelbutton is connected to **/
    void sendCancelRequest();


private:
    /** show a final status and remove the Widget after a defined time **/
    void completeNotification(QObject* sender, QString label);

    medProgressionStackPrivate *d;
};


