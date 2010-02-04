/* medDatabaseImporter.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Jan 19 13:42:32 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Thu Feb  4 11:35:57 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 3
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDatabaseImporter.h"

class medDatabaseImporterPrivate
{
public:
};

medDatabaseImporter::medDatabaseImporter(void) : QRunnable(), d(new medDatabaseImporterPrivate)
{

}

medDatabaseImporter::~medDatabaseImporter(void)
{
    delete d;

    d = NULL;
}

void medDatabaseImporter::run(void)
{
    for(int i = 0; i <= 100; i++) {
        emit progressed(i);
    }
}
