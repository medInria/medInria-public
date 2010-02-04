/* medDatabaseExporter.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Jan 19 13:42:53 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Thu Feb  4 11:36:11 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 3
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDatabaseExporter.h"

class medDatabaseExporterPrivate
{
public:
};

medDatabaseExporter::medDatabaseExporter(void) : QRunnable(), d(new medDatabaseExporterPrivate)
{

}

medDatabaseExporter::~medDatabaseExporter(void)
{
    delete d;

    d = NULL;
}

void medDatabaseExporter::run(void)
{
    for(int i = 0; i <= 100; i++) {
        emit progressed(i);
    }
}
