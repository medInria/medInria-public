/* medDatabaseNavigatorController.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:39:06 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:39:07 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASENAVIGATORCONTROLLER_H
#define MEDDATABASENAVIGATORCONTROLLER_H

#include <Qt>
#include <QtCore>

#include "medGuiExport.h"

class MEDGUI_EXPORT medDatabaseNavigatorController : public QObject
{
public:
    static medDatabaseNavigatorController *instance();

    Qt::Orientation orientation();

    qreal itemWidth() const;
    qreal itemHeight() const;
    qreal itemSpacing() const;

    qreal groupWidth() const;
    qreal groupHeight() const;
    qreal groupSpacing() const;

    qreal selectorWidth() const;
    qreal selectorHeight() const;
    qreal selectorSpacing() const;

    qreal queryOffset() const;

    void setItemSize(qreal width, qreal height);

    void setOrientation(Qt::Orientation orientation);

protected:
    static medDatabaseNavigatorController *s_instance;

private:
     medDatabaseNavigatorController();
    ~medDatabaseNavigatorController();

private:
    qreal m_width;
    qreal m_height;

    Qt::Orientation m_orientation;
};

#endif // MEDDATABASENAVIGATORCONTROLLER_H
