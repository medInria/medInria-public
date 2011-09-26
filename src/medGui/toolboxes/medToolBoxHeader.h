/* medToolBoxHeader.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 10:10:59 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Dec 20 13:45:08 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 8
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef MEDTOOLBOXHEADER_H
#define MEDTOOLBOXHEADER_H

#include "medGuiExport.h"

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
    ~medToolBoxHeader(void);

    /**
     * @brief Returns a preferred size hint (32x100px).
     *
     * @param void
     * @return QSize
    */
    QSize sizeHint(void) const;

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

    void showAboutButton(bool visible);
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

#endif
