/* medDatabaseNonPersitentController.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Sun Jun 27 17:58:04 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Sun Jun 27 19:09:46 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 81
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDatabaseNonPersitentItem.h"
#include "medDatabaseNonPersitentItem_p.h"
#include "medDatabaseNonPersitentController.h"

#include <medCore/medDataIndex.h>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractData.h>

#include <QtCore/QHash>

// /////////////////////////////////////////////////////////////////
// medDatabaseNonPersitentControllerPrivate
// /////////////////////////////////////////////////////////////////

class medDatabaseNonPersitentControllerPrivate
{
public:
    static int pt_index;
    static int st_index;
    static int se_index;
    static int im_index;

    QHash<medDataIndex, medDatabaseNonPersitentItem *> items;
};

int medDatabaseNonPersitentControllerPrivate::pt_index = medDatabaseNonPersitentController::nonPersitentDataStartingIndex();
int medDatabaseNonPersitentControllerPrivate::st_index = 0;
int medDatabaseNonPersitentControllerPrivate::se_index = 0;
int medDatabaseNonPersitentControllerPrivate::im_index = 0;

// /////////////////////////////////////////////////////////////////
// medDatabaseNonPersitentController
// /////////////////////////////////////////////////////////////////

medDatabaseNonPersitentController *medDatabaseNonPersitentController::instance(void)
{
    if(!s_instance)
        s_instance = new medDatabaseNonPersitentController;

    return s_instance;
}

QList<medDatabaseNonPersitentItem *> medDatabaseNonPersitentController::items(void)
{
    return d->items.values();
}

dtkAbstractData *medDatabaseNonPersitentController::data(const medDataIndex& index)
{
    if(d->items.keys().contains(index))
        return d->items.value(index)->data();

    return NULL;
}

medDataIndex medDatabaseNonPersitentController::read(const QString& file)
{
    QFileInfo info(file);
    
    typedef dtkAbstractDataFactory::dtkAbstractDataTypeHandler dtkAbstractDataTypeHandler;
    
    QList<dtkAbstractDataTypeHandler> readers = dtkAbstractDataFactory::instance()->readers();

    dtkAbstractData* data = NULL;
    
    for(int i = 0; i < readers.size(); i++) {

        dtkAbstractDataReader* reader = dtkAbstractDataFactory::instance()->reader(readers[i].first, readers[i].second);

	if (reader->canRead(info.filePath())) {
	    reader->read(info.filePath());
	    data = reader->data();
	    delete reader;
	    break;
	}
    }

    if(!data)
	return medDataIndex();

    medDataIndex index(d->pt_index++, 0, 0, 0);

    medDatabaseNonPersitentItem *item = new medDatabaseNonPersitentItem;
    item->d->name = info.baseName();
    item->d->file = file;
    item->d->thumb = data->thumbnail();
    item->d->index = index;
    item->d->data = data;

    d->items.insert(index, item);

    emit updated();

    return index;
}

int medDatabaseNonPersitentController::nonPersitentDataStartingIndex(void)
{
    return 10000;
}

medDatabaseNonPersitentController::medDatabaseNonPersitentController(void) : QObject(), d(new medDatabaseNonPersitentControllerPrivate)
{

}

medDatabaseNonPersitentController::~medDatabaseNonPersitentController(void)
{
    foreach(medDatabaseNonPersitentItem *item, d->items)
        delete item;

    delete d;

    d = NULL;
}

medDatabaseNonPersitentController *medDatabaseNonPersitentController::s_instance = NULL;
