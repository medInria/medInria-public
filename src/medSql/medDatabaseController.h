/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medSqlExport.h"
#include <medDatabaseControllerImpl.h>

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


