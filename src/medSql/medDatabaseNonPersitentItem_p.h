/* medDatabaseNonPersitentItem_p.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Sun Jun 27 18:51:32 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Sun Jun 27 18:54:12 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 7
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASENONPERSITENTITEM_P_H
#define MEDDATABASENONPERSITENTITEM_P_H

#include <medCore/medDataIndex.h>

#include <QtCore/QString>
#include <QtGui/QImage>

class dtkAbstractData;

class medDatabaseNonPersitentItemPrivate
{
public:
    QString name;
    QString file;
    QImage thumb;

    medDataIndex index;

    dtkAbstractData *data;
};

#endif
