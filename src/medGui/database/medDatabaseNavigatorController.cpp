/* medDatabaseNavigatorController.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:39:11 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:39:11 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDatabaseNavigatorController.h"

medDatabaseNavigatorController *medDatabaseNavigatorController::instance()
{
    if(!s_instance)
        s_instance = new medDatabaseNavigatorController;

    return s_instance;
}

Qt::Orientation medDatabaseNavigatorController::orientation()
{
    return m_orientation;
}

qreal medDatabaseNavigatorController::itemWidth() const
{
    return this->m_width;
}

qreal medDatabaseNavigatorController::itemHeight() const
{
    return this->m_height;
}

qreal medDatabaseNavigatorController::itemSpacing() const
{
    return 10;
}

qreal medDatabaseNavigatorController::groupWidth() const
{
    return this->itemWidth() + 2 * this->itemSpacing();
}

qreal medDatabaseNavigatorController::groupHeight() const
{
    return this->itemHeight() + 2 * this->itemSpacing();
}

qreal medDatabaseNavigatorController::groupSpacing() const
{
    return 10;
}

qreal medDatabaseNavigatorController::selectorWidth() const
{
    return this->itemWidth() + 8;
}

qreal medDatabaseNavigatorController::selectorHeight() const
{
    return this->itemHeight() + 8;
}

qreal medDatabaseNavigatorController::selectorSpacing() const
{
    return 10;
}

qreal medDatabaseNavigatorController::queryOffset() const
{
    return this->itemWidth()/2;
}

void medDatabaseNavigatorController::setItemSize(qreal width, qreal height)
{
    this->m_width = width;
    this->m_height = height;
}

void medDatabaseNavigatorController::setOrientation(Qt::Orientation orientation)
{
    this->m_orientation = orientation;
}

medDatabaseNavigatorController::medDatabaseNavigatorController()
{
    this->m_width = 72;
    this->m_height = 72;

    this->m_orientation = Qt::Horizontal;
}

medDatabaseNavigatorController::~medDatabaseNavigatorController()
{

}

medDatabaseNavigatorController *medDatabaseNavigatorController::s_instance = NULL;
