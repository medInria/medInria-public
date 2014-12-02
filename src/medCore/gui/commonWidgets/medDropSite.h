/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>

#include <QtGui/QLabel>

#include <medDataIndex.h>

class medDropSitePrivate;
class MEDCORE_EXPORT medDropSite : public QLabel
{
    Q_OBJECT

public:
     medDropSite(QWidget *parent = 0);
    ~medDropSite();

    QSize sizeHint() const;
    void setCanAutomaticallyChangeAppereance(bool can);

public slots:
    void clear();

public slots:
    void setValue(const medDataIndex& index);
public:
    medDataIndex value() const;

protected slots:
    void dataReady(medDataIndex data, QUuid uuid);

signals:
    /**
     * This signal is emitted when an object containing
     * a valid medDataIndex is dropped.
     */
    void valueChange (const medDataIndex& value);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent* event);
    
private:
    medDropSitePrivate *d;
};


