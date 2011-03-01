#include "medDatabaseNonPersistentController.h"

#include <medCore/medAbstractDbController.h>

QPointer<medDatabaseNonPersistentControllerImpl> medDatabaseNonPersistentController::instance(void)
{
    if(!s_instance)
        s_instance = new medDatabaseNonPersistentControllerImpl();

    return s_instance;
}

void medDatabaseNonPersistentController::destroy(void)
{
    if (s_instance) {
        delete s_instance;
        s_instance = 0;
    }
}

medAbstractDbController* createNonPersistentDbController()
{
    return medDatabaseNonPersistentController::instance();
}

medDatabaseNonPersistentControllerImpl *medDatabaseNonPersistentController::s_instance = NULL;

