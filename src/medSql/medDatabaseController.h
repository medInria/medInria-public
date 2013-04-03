/* medDatabaseController.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Mar 31 11:05:10 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Sun Jun 27 17:39:21 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 19
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASECONTROLLER_H
#define MEDDATABASECONTROLLER_H

#include "medSqlExport.h"
#include "medDatabaseControllerImpl.h"

class medAbstractDbController;

/**
 * This is just a singleton that instantiates one medDatabaseControllerImpl implementation
 */
class MEDSQL_EXPORT medDatabaseController : public QObject
{
    Q_OBJECT

public:

    /**
    * get an instance of medDatabaseControllerImpl, not of medDatabaseController
    * @return QPointer<medDbController>
    */
    static QPointer<medDatabaseControllerImpl> instance();
    
    /**
    * destroy instance of medDbController
    */
    static void destroy();

private:
    static medDatabaseControllerImpl *s_instance;
};

MEDSQL_EXPORT medAbstractDbController* createDbController();

#endif
