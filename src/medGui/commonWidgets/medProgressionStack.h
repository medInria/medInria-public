/* medProgressionStack.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Jan 19 13:39:23 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Thu Mar 18 22:52:07 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 5
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef MEDPROGRESSIONSTACK_H
#define MEDPROGRESSIONSTACK_H

#include "medGuiExport.h"

#include <QtGui/QWidget>

class medJobItem;
class medProgressionStackPrivate;

/**
 * @class medProgressionStack
 * @brief Creates JobWidgets that represent the progression of a jobItem.
 * All visible jobs will be managed here. The class stores pointers in hashTables to identify the sender of status messages
 * It provides methods to communicate with the jobItems (e.g. to cancel the job)
 */
class MEDGUI_EXPORT medProgressionStack : public QWidget
{
    Q_OBJECT

public:
     medProgressionStack(QWidget *parent = 0);
    ~medProgressionStack(void);

    QSize sizeHint(void) const;

signals:
    void shown(void);
    void hidden(void);
    void cancelRequest(QObject*);

public slots:

    /**
    * AddJobItem - Add a new subclass of medJobItem to the Stack to create the connection between them
    * @params: medJobItem * job - instance of medJobItem
    * @params: QString label - the label shown on the jobToolBox
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
    * @params QObject* sender
    */
    void becomesNoncancelable(QObject* sender);

private slots:
    /** send a cancel request to the job that the cancelbutton is connected to **/
    void sendCancelRequest();


private:
    /** show a final status and remove the Widget after a defined time **/
    void completeNotification(QObject* sender, QString label);

    medProgressionStackPrivate *d;
};

#endif // MEDPROGRESSIONSTACK_H
