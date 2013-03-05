/* medDatabasePreviewController.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:42:46 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:42:47 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDatabasePreviewController.h"

medDatabasePreviewController *medDatabasePreviewController::instance()
{
    if(!s_instance)
        s_instance = new medDatabasePreviewController;

    return s_instance;
}

Qt::Orientation medDatabasePreviewController::orientation()
{
    return m_orientation;
}

qreal medDatabasePreviewController::itemWidth() const
{
    return this->m_width;
}

qreal medDatabasePreviewController::itemHeight() const
{
    return this->m_height;
}

qreal medDatabasePreviewController::itemSpacing() const
{
    return 10;
}

qreal medDatabasePreviewController::groupWidth() const
{
    return 2 * this->itemWidth() + 2 * this->itemSpacing() - this->selectorWidth();
}

qreal medDatabasePreviewController::groupHeight() const
{
    return 2 * this->itemHeight() + 2 * this->itemSpacing() - this->selectorHeight();
}

qreal medDatabasePreviewController::groupSpacing() const
{
    return 10;
}

qreal medDatabasePreviewController::selectorWidth() const
{
    return this->itemWidth() + 8;
}

qreal medDatabasePreviewController::selectorHeight() const
{
    return this->itemHeight() + 8;
}

qreal medDatabasePreviewController::selectorSpacing() const
{
    return 10;
}

qreal medDatabasePreviewController::queryOffset() const
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

medDatabasePreviewController::medDatabasePreviewController()
{
    this->m_width = 128;
    this->m_height = 128;

    this->m_orientation = Qt::Horizontal;
}

medDatabasePreviewController::~medDatabasePreviewController()
{

}

medDatabasePreviewController *medDatabasePreviewController::s_instance = NULL;
