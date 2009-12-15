/* medDatabaseNavigatorTooltip.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:41:24 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:41:24 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASENAVIGATORTOOLTIP_H
#define MEDDATABASENAVIGATORTOOLTIP_H

#include <QtCore>
#include <QtGui>

class medDatabaseNavigatorTooltip : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

    Q_PROPERTY(float opacity READ opacity WRITE setOpacity)

public:
    medDatabaseNavigatorTooltip(const QPixmap& pixmap, QGraphicsItem *parent = 0);
};

#endif // MEDDATABASENAVIGATORTOOLTIP_H
