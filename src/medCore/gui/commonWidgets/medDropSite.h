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
    virtual ~medDropSite();

    QSize sizeHint() const;

    void setCanAutomaticallyChangeAppereance(bool can);

    void setAcceptedTypes(const QStringList & types);
    QStringList acceptedTypes() const;

    medDataIndex index() const;

public slots:
    void clear();

signals:

    /**
     * This signal is emitted when an object containing
     * a valid medDataIndex is dropped.
     */
    void objectDropped (const medDataIndex& index);

    /** Signal emitted when the user clicks on the medDropSite. */
    void clicked();
    
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


