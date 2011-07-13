/* medDatabasePreviewController.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:42:42 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:42:42 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASEPREVIEWCONTROLLER_H
#define MEDDATABASEPREVIEWCONTROLLER_H

#include <Qt>
#include <QtCore>

#include "medGuiExport.h"

class MEDGUI_EXPORT medDatabasePreviewController : public QObject
{
public:
    static medDatabasePreviewController *instance(void);

    Qt::Orientation orientation(void);

    qreal itemWidth(void) const;
    qreal itemHeight(void) const;
    qreal itemSpacing(void) const;

    qreal groupWidth(void) const;
    qreal groupHeight(void) const;
    qreal groupSpacing(void) const;

    qreal selectorWidth(void) const;
    qreal selectorHeight(void) const;
    qreal selectorSpacing(void) const;

    qreal queryOffset(void) const;

    void setItemSize(qreal width, qreal height);

    void setOrientation(Qt::Orientation orientation);

protected:
    static medDatabasePreviewController *s_instance;

private:
     medDatabasePreviewController(void);
    ~medDatabasePreviewController(void);

private:
    qreal m_width;
    qreal m_height;

    Qt::Orientation m_orientation;
};

#endif // MEDDATABASEPREVIEWCONTROLLER_H
