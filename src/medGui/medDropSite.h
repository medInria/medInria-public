/* medDropSite.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 17:39:32 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Feb 19 17:41:33 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 6
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDROPSITE_H
#define MEDDROPSITE_H

#include "medGuiExport.h"

#include <QtGui/QLabel>

class medDropSitePrivate;

class MEDGUI_EXPORT medDropSite : public QLabel
{
    Q_OBJECT

public:
     medDropSite(QWidget *parent = 0);
    ~medDropSite(void);

    QSize sizeHint(void) const;

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    medDropSitePrivate *d;
};

#endif
