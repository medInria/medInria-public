/* medBrowserToolBoxSource.h ---
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu May 13 15:19:01 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 10:24:03 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 16
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDBROWSERTOOLBOXSOURCE_H
#define MEDBROWSERTOOLBOXSOURCE_H

#include <medGui/medToolBox.h>

class medBrowserToolBoxSourcePrivate;

class medBrowserToolBoxSource : public medToolBox
{
    Q_OBJECT

public:
     medBrowserToolBoxSource(QWidget *parent = 0);
    ~medBrowserToolBoxSource(void);

    void setFileSystemWidget(QWidget *widget);
    void setPacsWidget(QWidget* widget);
		void addAdditionalTab(QString tabName, QWidget *widget);

signals:
    void indexChanged(int);
    // void echoPressed();
    // void findPressed();

private:
    medBrowserToolBoxSourcePrivate *d;
};

#endif
