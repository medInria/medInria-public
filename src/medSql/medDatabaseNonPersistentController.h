#ifndef medDatabaseNonPersistentController_h__
#define medDatabaseNonPersistentController_h__

#include "medSqlExport.h"
#include "medDatabaseNonPersistentControllerImpl.h"

class medAbstractDbController;

/**
 * This is just a singleton that instantiates one medDatabaseControllerImpl implementation
 */
class MEDSQL_EXPORT medDatabaseNonPersistentController : public QObject
{
    Q_OBJECT

public:

    /**
    * get an instance of medDatabaseControllerImpl, not of medDatabaseController
    * @return QPointer<medDbController>
    */
    static QPointer<medDatabaseNonPersistentControllerImpl> instance();
    
    /**
    * destroy instance of medDbController
    */
    static void destroy();

private:
    static medDatabaseNonPersistentControllerImpl *s_instance;
};

MEDSQL_EXPORT medAbstractDbController* createNonPersistentDbController();

#endif // medDatabaseNonPersistentController_h__