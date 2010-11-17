/* medStorage.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 17 18:45:22 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 17 18:45:41 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 4
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDSTORAGE_H
#define MEDSTORAGE_H

#include <QString>

class medStorage
{
public:
     medStorage(void);
    ~medStorage(void);

    static bool mkpath(const QString& dirPath);
    static bool rmpath(const QString& dirPath);

    static QString   dataLocation(void);
    static QString configLocation(void);
};

#endif // MEDSTORAGE_H
