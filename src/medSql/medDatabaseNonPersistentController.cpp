/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDatabaseNonPersistentController.h>

#include <medAbstractDbController.h>

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

