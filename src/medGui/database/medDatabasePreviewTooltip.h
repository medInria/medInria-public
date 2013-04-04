/* medDatabasePreviewTooltip.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:44:20 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:44:21 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#pragma once

#include <QtCore>
#include <QtGui>

class medDatabasePreviewTooltip : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

    Q_PROPERTY(float opacity READ opacity WRITE setOpacity)

public:
    medDatabasePreviewTooltip(const QPixmap& pixmap, QGraphicsItem *parent = 0);
};


