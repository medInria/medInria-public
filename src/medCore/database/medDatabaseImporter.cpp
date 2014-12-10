/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDatabaseImporter.h>

#include <medAbstractImageData.h>

#include <medAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <medAbstractData.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkLog/dtkLog.h>
#include <medDatabaseController.h>
#include <medMetaDataKeys.h>
#include <medStorage.h>



//-----------------------------------------------------------------------------------------------------------

medDatabaseImporter::medDatabaseImporter ( const QString& file, const QUuid& uuid, medAbstractDbController* iController, bool indexWithoutImporting) :
    medAbstractDatabaseImporter(file, uuid, iController, indexWithoutImporting)
{

}

//-----------------------------------------------------------------------------------------------------------

medDatabaseImporter::medDatabaseImporter ( medAbstractData* medData, const QUuid& uuid, medAbstractDbController* iController) :
    medAbstractDatabaseImporter(medData, uuid, iController)
{

}

//-----------------------------------------------------------------------------------------------------------

medDatabaseImporter::~medDatabaseImporter ( void )
{

}








