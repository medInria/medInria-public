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
    void linkwl (dtkAbstractView *view, bool value);
    
public slots:
    void onViewDaddy   (bool);
    void onViewSync    (bool);
    void onViewSyncWL  (bool);
	void onViewReg     (bool);

private:
    medViewPoolPrivate *d;
  
};

#endif
