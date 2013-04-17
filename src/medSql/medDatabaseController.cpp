/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

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

