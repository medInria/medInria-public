/* medViewPool.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 17:39:32 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Feb 22 21:14:38 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 9
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDVIEWPOOL_H
#define MEDVIEWPOOL_H

#include <QtCore>

#include "medGuiExport.h"

class medViewPoolPrivate;
class dtkAbstractView;

class MEDGUI_EXPORT medViewPool : public QObject
{
    Q_OBJECT

public:
    medViewPool (void);
    ~medViewPool (void);

    void appendView (dtkAbstractView *view);
    void removeView (dtkAbstractView *view);

    dtkAbstractView *daddy (void);

    void setViewProperty (QString key, QString value);

    int count (void);

signals:
    void linkPosition  (dtkAbstractView *view, bool value);
    void linkCamera    (dtkAbstractView *view, bool value);
    void linkWindowing (dtkAbstractView *view, bool value);
    
    void showInfo(QObject*,const QString&,unsigned int timeout);
    void showError(QObject*,const QString&,unsigned int timeout);
    
public slots:
    void onViewDaddy         (bool);
    void onViewSyncPosition  (bool);
    void onViewSyncCamera    (bool);
    void onViewSyncWindowing (bool);
    void onViewReg           (bool);

private:
    medViewPoolPrivate *d;
  
};

#endif
