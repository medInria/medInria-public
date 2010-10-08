/* medToolBoxSource.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu May 13 15:19:01 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Oct  5 12:56:54 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 15
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDTOOLBOXSOURCE_H
#define MEDTOOLBOXSOURCE_H

#include "medToolBox.h"

class medToolBoxSourcePrivate;

class MEDGUI_EXPORT medToolBoxSource : public medToolBox
{
    Q_OBJECT

public:
     medToolBoxSource(QWidget *parent = 0);
    ~medToolBoxSource(void);

    void setFileSystemWidget(QWidget *widget);
    
signals:
    void indexChanged(int);
    // void echoPressed();
    // void findPressed();

private:
    medToolBoxSourcePrivate *d;
};

#endif
