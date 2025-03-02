#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QtGui>
#include <QtWidgets>

#include <medCoreExport.h>

class medToolBoxHeaderPrivate;

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

     medToolBoxHeader(QWidget *parent = nullptr);
    ~medToolBoxHeader();

    void setTitle(const QString& title);
    QString title() const;

    void setTitleOffset(const QPoint & titleOffset);

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

signals:
    //!  Emitted when a mouse press event is caught.
    void triggered();

private:
    medToolBoxHeaderPrivate *d;
};
