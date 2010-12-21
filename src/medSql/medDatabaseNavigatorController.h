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

#include "medSqlExport.h"

class MEDSQL_EXPORT medDatabaseNavigatorController : public QObject
{
public:
    static medDatabaseNavigatorController *instance(void);

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
    static medDatabaseNavigatorController *s_instance;

private:
     medDatabaseNavigatorController(void);
    ~medDatabaseNavigatorController(void);

private:
    qreal m_width;
    qreal m_height;

    Qt::Orientation m_orientation;
};

#endif // MEDDATABASENAVIGATORCONTROLLER_H
