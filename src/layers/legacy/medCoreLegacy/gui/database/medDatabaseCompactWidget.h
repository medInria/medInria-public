/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QWidget>
#include <medCoreLegacyExport.h>

class medDatabaseCompactWidgetPrivate;
class medDatabaseSearchPanel;
class medDatabaseView;
class medDatabasePreview;
class medDataIndex;


/**
* @brief
*/
class MEDCORELEGACY_EXPORT medDatabaseCompactWidget: public QWidget
{
    Q_OBJECT
public :
    medDatabaseCompactWidget(QWidget *parent = nullptr);
    virtual ~medDatabaseCompactWidget();

    void setSearchPanelViewAndPreview(medDatabaseSearchPanel *panel, medDatabaseView *view,
                                      medDatabasePreview *preview);

protected:
    void resizeEvent(QResizeEvent *event);

    void createLayout();
    void createDatabaseToolBox();
    void placeDatabaseWidgets();

signals:
    void open(const medDataIndex& index);

private:
    medDatabaseCompactWidgetPrivate *d;
};
