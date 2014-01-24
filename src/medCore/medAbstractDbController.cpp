/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medAbstractDbController.h"
#include "medDataIndex.h"


void medAbstractDbController::import( const medDataIndex& index, const medAbstractDbController& controller )
{
    qDebug() << "DEBUG : entering medAbstractDbController::import(const medDataIndex& index, const medAbstractDbController& controller )";
    dtkSmartPointer<medAbstractData> data( controller.read(index) );
    import(data);
}

void medAbstractDbController::import(const QString& file,bool indexWithoutCopying)
{
    //qDebug() << "DEBUG : entering medAbstractDbController::import(const QString& file,bool indexWithoutCopying)";
}

void medAbstractDbController::exportDataToFile(medAbstractData *data, const QString &filename, const QString &writer)
{
    //qDebug() << "DEBUG : entering medAbstractDbController::exportDataToFile(medAbstractData *data, const QString &filename)";
}

void medAbstractDbController::clear()
{
}

void medAbstractDbController::remove (const medDataIndex &index)
{
}
