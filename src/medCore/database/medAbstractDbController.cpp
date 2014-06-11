/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractDbController.h>
#include <medDataIndex.h>


void medAbstractDbController::import( const medDataIndex& index, const medAbstractDbController& controller )
{
    qDebug() << "DEBUG : entering medAbstractDbController::import(const medDataIndex& index, const medAbstractDbController& controller )";
    medAbstractData* data( controller.retrieve(index));
    this->import(data);
}
