/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>

#include <QtWidgets/QWidget>

class medButtonPrivate;

/**
 * @brief General custom button that loads shows an icon.
 */
class MEDCORE_EXPORT medButton : public QWidget
{
    Q_OBJECT

public:

    medButton(QWidget *parent, QString resourceLocation, QString toolTip);
    medButton(QWidget *parent, QPixmap pixmap, QString toolTip);

    ~medButton();

    QSize sizeHint() const;

    void setIcon(QPixmap icon);

signals:
    //!  Emitted when the button is clicked.
    void triggered();

protected:
    void mousePressEvent(QMouseEvent *event);

private:
    medButtonPrivate * d;
};

