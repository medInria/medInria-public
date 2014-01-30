/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medGuiExport.h>

#include <QtGui>

class medToolBoxHeaderPrivate;
class medButton;

/**
 * @brief Contains the header of a medToolBox.
 *
 * This is a QFrame that draws a rectangle, a title, and reacts to mousePressEvent()s.
 *
*/
class MEDGUI_EXPORT medToolBoxHeader : public QFrame
{
    Q_OBJECT
public:
     /**
      * @brief Instantiates a header.
      *
      * @param parent
     */
     medToolBoxHeader(QWidget *parent = 0);

    /**
     * @brief
     *
     * @param void
    */
    ~medToolBoxHeader();

    /**
     * @brief Sets the header's title.
     *
     * @param title
    */
    void setTitle(const QString& title);

    /**
     * @brief Returns the title of the header.
     *
     * @return const QString
     */
    QString title() const;

    /**
     * @brief Sets the offset of the header's title from upper left corner.
     *
     * @param titleOffset
    */
    void setTitleOffset(const QPoint & titleOffset);

    /**
     * @brief
     *
     * @param visible
     */
    void setAboutButtonVisibility(bool visible);


    /**
     * @brief Returns a pointer to the aboutButton.
     *
     */
    medButton* aboutButton();


protected:
    /**
     * @brief Paints the header.
     *
     * @param event
    */
    void paintEvent(QPaintEvent *event);

    /**
     * @brief Emits the triggered() signal.
     *
     * @param event
    */
    void mousePressEvent(QMouseEvent *event);

signals:
    /**
     * @brief Emitted when a mouse press event is caught.
     *
    */
    void triggered();

private:
    medToolBoxHeaderPrivate *d;
};


