/* medDatabasePreviewTooltip.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:44:26 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:44:26 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDatabasePreviewTooltip.h"

medDatabasePreviewTooltip::medDatabasePreviewTooltip(const QPixmap& pixmap, QGraphicsItem *parent) : QObject(), QGraphicsPixmapItem(pixmap, parent)
{
    this->setZValue(20);
}
