/* medDocumentationArea.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 22 09:04:15 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Mar 19 18:45:11 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 6
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDOCUMENTATIONAREA_H
#define MEDDOCUMENTATIONAREA_H

#include <QtCore>
#include <QtGui>

class medDocumentationAreaPrivate;

class medDocumentationArea : public QWidget
{
    Q_OBJECT

public:
     medDocumentationArea(QWidget *parent = 0);
    ~medDocumentationArea(void);

    void setup(QStatusBar *status);
    void setdw(QStatusBar *status);

private:
    medDocumentationAreaPrivate *d;
};

#endif // MEDDOCUMENTATIONAREA_H
