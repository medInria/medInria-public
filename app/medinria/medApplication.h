/* medApplication.h ---
 * 
 * Author: John Stark
 * Copyright (C) 2011 - John Stark, Inria.
 * Created: May 2011
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
