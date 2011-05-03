/* medBrowserArea.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Oct  9 08:28:09 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct  6 15:51:28 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 20
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDAPPLICATION_H
#define MEDAPPLICATION_H

#include <QApplication>

class medApplicationPrivate;

// /////////////////////////////////////////////////////////////////
// medApplication
// /////////////////////////////////////////////////////////////////

class medApplication : public QApplication
{
    Q_OBJECT

public:
    medApplication(int & argc, char **argv);
    ~medApplication();


protected:

private:
    medApplicationPrivate *d;
};

#endif // MEDAPPLICATION_H
