/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDatabaseNavigatorController.h>

medDatabaseNavigatorController *medDatabaseNavigatorController::instance(void)
{
    if(!s_instance)
        s_instance = new medDatabaseNavigatorController;

    return s_instance;
}

Qt::Orientation medDatabaseNavigatorController::orientation(void)
{
    return m_orientation;
}

qreal medDatabaseNavigatorController::itemWidth(void) const
{
    return this->m_width;
}

qreal medDatabaseNavigatorController::itemHeight(void) const
{
    return this->m_height;
}

qreal medDatabaseNavigatorController::itemSpacing(void) const
{
    return 10;
}

qreal medDatabaseNavigatorController::groupWidth(void) const
{
    return this->itemWidth() + 2 * this->itemSpacing();
}

qreal medDatabaseNavigatorController::groupHeight(void) const
{
    return this->itemHeight() + 2 * this->itemSpacing();
}

qreal medDatabaseNavigatorController::groupSpacing(void) const
{
    return 10;
}

qreal medDatabaseNavigatorController::selectorWidth(void) const
{
    return this->itemWidth() + 8;
}

qreal medDatabaseNavigatorController::selectorHeight(void) const
{
    return this->itemHeight() + 8;
}

qreal medDatabaseNavigatorController::selectorSpacing(void) const
{
    return 10;
}

qreal medDatabaseNavigatorController::queryOffset(void) const
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

medDatabaseNavigatorController::medDatabaseNavigatorController(void)
{
    this->m_width = 72;
    this->m_height = 72;

    this->m_orientation = Qt::Horizontal;
}

medDatabaseNavigatorController::~medDatabaseNavigatorController(void)
{

}

medDatabaseNavigatorController *medDatabaseNavigatorController::s_instance = NULL;
