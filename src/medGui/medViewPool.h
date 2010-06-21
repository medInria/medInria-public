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

    int synchronized (void);
    
public slots:
    void synchronize   (void);
    void desynchronize (void);

    void onViewPropertySet (QString key, QString value);

private:
    medViewPoolPrivate *d;
  
};

#endif
