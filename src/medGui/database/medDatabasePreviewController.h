/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <Qt>
#include <QtCore>

#include "medGuiExport.h"

class MEDGUI_EXPORT medDatabasePreviewController : public QObject
{
public:
    static medDatabasePreviewController *instance();

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
    static medDatabasePreviewController *s_instance;

private:
     medDatabasePreviewController();
    ~medDatabasePreviewController();

private:
    qreal m_width;
    qreal m_height;

    Qt::Orientation m_orientation;
};


