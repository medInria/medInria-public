#include "medDatabaseController.h"

QPointer<medDatabaseControllerImpl> medDatabaseController::instance(void)
{
    if(!s_instance)
        s_instance = new medDatabaseControllerImpl();

    return s_instance;
}

void medDatabaseController::destroy(void)
{
    if (s_instance) {
        delete s_instance;
        s_instance = 0;
    }
}

medAbstractDbController* createDbController()
{
    return medDatabaseController::instance();
}

medDatabaseControllerImpl *medDatabaseController::s_instance = NULL;

