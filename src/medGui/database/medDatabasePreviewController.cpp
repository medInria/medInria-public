/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDatabasePreviewController.h>

medDatabasePreviewController *medDatabasePreviewController::instance(void)
{
    if(!s_instance)
        s_instance = new medDatabasePreviewController;

    return s_instance;
}

Qt::Orientation medDatabasePreviewController::orientation(void)
{
    return m_orientation;
}

qreal medDatabasePreviewController::itemWidth(void) const
{
    return this->m_width;
}

qreal medDatabasePreviewController::itemHeight(void) const
{
    return this->m_height;
}

qreal medDatabasePreviewController::itemSpacing(void) const
{
    return 10;
}

qreal medDatabasePreviewController::groupWidth(void) const
{
    return 2 * this->itemWidth() + 2 * this->itemSpacing() - this->selectorWidth();
}

qreal medDatabasePreviewController::groupHeight(void) const
{
    return 2 * this->itemHeight() + 2 * this->itemSpacing() - this->selectorHeight();
}

qreal medDatabasePreviewController::groupSpacing(void) const
{
    return 10;
}

qreal medDatabasePreviewController::selectorWidth(void) const
{
    return this->itemWidth() + 8;
}

qreal medDatabasePreviewController::selectorHeight(void) const
{
    return this->itemHeight() + 8;
}

qreal medDatabasePreviewController::selectorSpacing(void) const
{
    return 10;
}

qreal medDatabasePreviewController::queryOffset(void) const
{
    return this->itemWidth()/2;
}

void medDatabasePreviewController::setItemSize(qreal width, qreal height)
{
    this->m_width = width;
    this->m_height = height;
}

void medDatabasePreviewController::setOrientation(Qt::Orientation orientation)
{
    this->m_orientation = orientation;
}

medDatabasePreviewController::medDatabasePreviewController(void)
{
    this->m_width = 128;
    this->m_height = 128;

    this->m_orientation = Qt::Horizontal;
}

medDatabasePreviewController::~medDatabasePreviewController(void)
{

}

medDatabasePreviewController *medDatabasePreviewController::s_instance = NULL;
