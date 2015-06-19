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

#include <QtGui>
#include <QtWidgets>

class medToolBoxHeaderPrivate;
class medButton;

/**
 * @brief Contains the header of a medToolBox.
 *
 * This is a QFrame that draws a rectangle, a title, and reacts to mousePressEvent()s.
 *
*/
class MEDCORE_EXPORT medToolBoxHeader : public QFrame
{
    Q_OBJECT

public:

     medToolBoxHeader(QWidget *parent = 0);
    ~medToolBoxHeader();

    void setTitle(const QString& title);
    QString title() const;

    void setTitleOffset(const QPoint & titleOffset);
    void setAboutButtonVisibility(bool visible);
    medButton* aboutButton();

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

signals:
    //!  Emitted when a mouse press event is caught.
    void triggered();

private:
    medToolBoxHeaderPrivate *d;
};


