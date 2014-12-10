/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDatabaseNonPersistentController.h>
#include <medDatabaseController.h>
#include <medDatabaseNonPersistentItem.h>
#include <medDatabaseNonPersistentItem_p.h>
#include <medDatabaseNonPersistentImporter.h>

#include <medAbstractImageData.h>
#include <medMetaDataKeys.h>

#include <medAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <medAbstractData.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkLog/dtkLog.h>



medDatabaseNonPersistentImporter::medDatabaseNonPersistentImporter (const QString& file, const QUuid& uuid , medAbstractDbController *iController)
: medAbstractDatabaseImporter(file, uuid, iController, true)
{
    qDebug() << "medDatabaseNonPersistentImporter created with uuid:" << this->callerUuid();
}

//-----------------------------------------------------------------------------------------------------------

medDatabaseNonPersistentImporter::medDatabaseNonPersistentImporter (medAbstractData* medData, const QUuid &uuid, medAbstractDbController *iController)
: medAbstractDatabaseImporter(medData, uuid, iController)
{
    qDebug() << "medDatabaseNonPersistentImporter created with uuid:" << this->callerUuid();
}

//-----------------------------------------------------------------------------------------------------------

medDatabaseNonPersistentImporter::~medDatabaseNonPersistentImporter ()
{

}
