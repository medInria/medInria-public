/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCore/dtkGlobal.h>

#include <medDataIndex.h>

#include <medDropSite.h>
#include <medDataManager.h>
#include <medDataSourceDialog.h>
#include <medAbstractData.h>

#include <QtGui>

class medDropSitePrivate
{
public:
    medDataIndex index;
    bool canAutomaticallyChangeAppereance;
    QUuid expectedUuid;
};

medDropSite::medDropSite(QWidget *parent) : QLabel(parent), d(new medDropSitePrivate)
{
    setAlignment(Qt::AlignCenter);
    setAcceptDrops(true);
    setBackgroundRole(QPalette::Base);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    d->index = medDataIndex();
    d->expectedUuid = QUuid();
    d->canAutomaticallyChangeAppereance = true;
}

medDropSite::~medDropSite(void)
{
    delete d;
    d = NULL;
}

QSize medDropSite::sizeHint(void) const
{
    return QSize(128, 128);
}

/**
 * Whenever an object is dropped on the medDropSite it
 * automatically updates itself to use the correspondent
 * thumbnail. If you prefer to manually modify it from
 * outside set this to false.
 */
void medDropSite::setCanAutomaticallyChangeAppereance(bool can)
{
    d->canAutomaticallyChangeAppereance = can;
}

medDataIndex medDropSite::value(void) const
{
    return d->index;
}

void medDropSite::setValue(const medDataIndex &index)
{
    if(!index.isValidForImage())
        return;

    d->index = index;
    if (d->canAutomaticallyChangeAppereance )
        setPixmap(QPixmap::fromImage(medDataManager::instance()->retrieveData(index)->thumbnail()));
}

void medDropSite::dragEnterEvent(QDragEnterEvent *event)
{
    setBackgroundRole(QPalette::Highlight);
    event->acceptProposedAction();
}

void medDropSite::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void medDropSite::dragLeaveEvent(QDragLeaveEvent *event)
{
    setBackgroundRole(QPalette::Base);
    event->accept();
}

void medDropSite::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    medDataIndex index( medDataIndex::readMimeData(mimeData) );
    if (index.isValid())
        this->setValue(d->index);

    setBackgroundRole(QPalette::Base);
    event->acceptProposedAction();
}

void medDropSite::clear(){
    QLabel::clear();
    d->index = medDataIndex();
}


void medDropSite::dataReady(medDataIndex index, QUuid uuid)
{
    if(d->expectedUuid != uuid)
        return;

    d->expectedUuid = QUuid();
    disconnect(medDataManager::instance(), SIGNAL(dataImported(medDataIndex,QUuid)), this, SLOT(dataReady(medDataIndex,QUuid)));
    this->setValue(index);
}

void medDropSite::mousePressEvent(QMouseEvent* event)
{
    Qt::MouseButtons mouseButtons = event->buttons();
    if( mouseButtons & Qt::LeftButton )
    {
        QString path = medDataSourceDialog::getFilenameFromFileSystem(this);
        if (path.isEmpty())
            return;

        connect(medDataManager::instance(), SIGNAL(dataImported(medDataIndex,QUuid)), this, SLOT(dataReady(medDataIndex,QUuid)));
        d->expectedUuid = medDataManager::instance()->importPath(path, true, false);
    }
}
