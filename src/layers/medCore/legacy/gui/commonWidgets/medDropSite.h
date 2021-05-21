#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QLabel>

#include <medCoreLegacyExport.h>
#include <medDataIndex.h>

class medDropSitePrivate;

class MEDCORELEGACY_EXPORT medDropSite : public QLabel
{
    Q_OBJECT

public:
     medDropSite(QWidget *parent = nullptr);
    ~medDropSite();

    QSize sizeHint() const;

    void setCanAutomaticallyChangeAppereance(bool can);

public:
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
