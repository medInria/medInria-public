/* medViewManager.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Dec 21 12:47:46 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Dec 21 12:47:46 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDVIEWMANAGER_H
#define MEDVIEWMANAGER_H

#include <QtCore/QObject>

#include "medDataIndex.h"

class dtkAbstractView;
class dtkAbstractViewFactory;

class medViewManagerPrivate;

class medViewManager : public QObject
{
    Q_OBJECT

public:
    static medViewManager *instance(void);

protected:
     medViewManager(void);
    ~medViewManager(void);

protected:
    static medViewManager *s_instance;

private:
    medViewManagerPrivate *d;
};

#endif // MEDVIEWMANAGER_H
