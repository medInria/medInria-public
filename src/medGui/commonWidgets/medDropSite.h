/* medDropSite.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 17:39:32 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Mar  2 15:44:37 2011 (+0100)
 *           By: Jaime Garcia_Guevara
 *     Update #: 10
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#pragma once

#include "medGuiExport.h"

#include <QtGui/QLabel>

#include <medDataIndex.h>

class medDropSitePrivate;

class MEDGUI_EXPORT medDropSite : public QLabel
{
    Q_OBJECT

public:
     medDropSite(QWidget *parent = 0);
    ~medDropSite(void);

    QSize sizeHint(void) const;

    /**
     * Whenever an object is dropped on the @medDropSite it
     * automatically updates itself to use the correspondent
     * thumbnail. If you prefer to manually modify it from
     * outside set this to false.
     */
    void setCanAutomaticallyChangeAppereance(bool can);

public:
    medDataIndex index(void) const;

public slots:
    void clear(void);

signals:

    /**
     * This signal is emitted when an object containing
     * a valid @medDataIndex is dropped.
     */
    void objectDropped (const medDataIndex& index);

    /** Signal emitted when the user clicks on the @medDropSite. */
    void clicked();
    
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent* event);
    
private:
    medDropSitePrivate *d;
};


