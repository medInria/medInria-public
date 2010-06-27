/* medDatabaseNonPersitentItem.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Sun Jun 27 18:42:08 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Sun Jun 27 18:54:44 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 17
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDatabaseNonPersitentItem.h"
#include "medDatabaseNonPersitentItem_p.h"

medDatabaseNonPersitentItem::medDatabaseNonPersitentItem(void) : QObject(), d(new medDatabaseNonPersitentItemPrivate)
{
    d->data = NULL;
}

medDatabaseNonPersitentItem::~medDatabaseNonPersitentItem(void)
{
    delete d;

    d = NULL;
}

const QString& medDatabaseNonPersitentItem::name(void) const
{
    return d->name;
}

const QString& medDatabaseNonPersitentItem::file(void) const
{
    return d->file;
}

const QImage& medDatabaseNonPersitentItem::thumb(void) const
{
    return d->thumb;
}

const medDataIndex& medDatabaseNonPersitentItem::index(void) const
{
    return d->index;
}

dtkAbstractData *medDatabaseNonPersitentItem::data(void)
{
    return d->data;
}
