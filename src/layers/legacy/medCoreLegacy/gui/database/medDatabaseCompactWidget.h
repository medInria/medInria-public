#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QWidget>
#include <medCoreLegacyExport.h>

class medDatabaseCompactWidgetPrivate;
class medDatabaseSearchPanel;
class medDatabaseView;
class medDatabasePreview;
class medDataIndex;

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
