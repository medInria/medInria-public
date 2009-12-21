/* medViewerArea.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:42:58 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Mon Oct 26 12:56:09 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 17
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDVIEWERAREA_H
#define MEDVIEWERAREA_H

#include <QtGui>

class dtkAbstractView;

class medViewContainer;

// /////////////////////////////////////////////////////////////////
// medViewerAreaStack
// /////////////////////////////////////////////////////////////////

class medViewerAreaStackPrivate;

class medViewerAreaStack : public QStackedWidget
{
    Q_OBJECT

public:
     medViewerAreaStack(QWidget *parent = 0);
    ~medViewerAreaStack(void);

    medViewContainer *current(void);

    void setPatientId(int id);

signals:
    void focused(dtkAbstractView *view);

private:
    medViewerAreaStackPrivate *d;
};

// /////////////////////////////////////////////////////////////////
// medViewerArea
// /////////////////////////////////////////////////////////////////

class medViewerAreaPrivate;

class medViewerArea : public QWidget
{
    Q_OBJECT

public:
     medViewerArea(QWidget *parent = 0);
    ~medViewerArea(void);

    void setup(QStatusBar *status);
    void setdw(QStatusBar *status);

    void setPatientIndex(int id);
    void   setStudyIndex(int id);
    void  setSeriesIndex(int id);
    void   setImageIndex(int id);

public slots:
    void setup(void);
    void split(int rows, int cols);

    void onPatientIndexChanged(int index);
    void   onStudyIndexChanged(int index);
    void  onSeriesIndexChanged(int index);
    void   onImageIndexChanged(int index);

    void onViewFocused(dtkAbstractView *view);

protected slots: // layout settings
    void setStackIndex(int index);

protected slots: // view settings
    void setupForegroundLookupTable(QString table);
    void setupBackgroundLookupTable(QString table);
    void setupAxisVisibility(bool visible);
    void setupScalarBarVisibility(bool visible);

private:
    medViewerAreaPrivate *d;
};

#endif // MEDVIEWERAREA_H
