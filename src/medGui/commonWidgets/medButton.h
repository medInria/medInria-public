/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medGuiExport.h"

#include <QtGui/QWidget>

class medButtonPrivate;

/**
 * @brief General custom button that loads shows an icon.
 */

class MEDGUI_EXPORT medButton : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Instantiates the button from a resource path.
     *
     * @param parent
     * @param resourceLocation
     * @param toolTip
    */
    medButton(QWidget *parent, QString resourceLocation, QString toolTip);

    /**
     * @brief Instantiates the button from an existing pixmap.
     *
     * @param parent
     * @param pixmap
     * @param toolTip
    */
    medButton(QWidget *parent, QPixmap pixmap, QString toolTip);

    /**
     * @brief Destroys the button
     *
     * @param void
    */
    ~medButton();

    /**
     * @brief Returns the pixmap's size.
     *
     * @param void
     * @return QSize
    */
    QSize sizeHint() const;

    /**
     * @brief Sets a new existing pixmap as an icon.
     *
     * @param icon
    */
    void setIcon(QPixmap icon);

signals:
    /**
     * @brief Emitted when the button is clicked.
     *
     * @param void
    */
    void triggered();

protected:
    /**
     * @brief Reimplemented mouse pressed event.
     *
     * @param event
    */
    void mousePressEvent(QMouseEvent *event);

private:
    medButtonPrivate * d;
};

