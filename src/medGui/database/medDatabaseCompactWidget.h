/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QWidget>

class medDatabaseCompactWidgetPrivate;
class medDatabaseView;
class medDatabasePreview;
class medDataIndex;


/**
* @brief
*/
class medDatabaseCompactWidget: public QWidget
{
    Q_OBJECT
public :
    medDatabaseCompactWidget(QWidget *parent = NULL);
    virtual ~medDatabaseCompactWidget();

    void setViewAndPreview(medDatabaseView *view, medDatabasePreview *preview);

protected:
    void resizeEvent(QResizeEvent *event);


signals:
    void open(const medDataIndex& index);

private:
    medDatabaseCompactWidgetPrivate *d;
};
