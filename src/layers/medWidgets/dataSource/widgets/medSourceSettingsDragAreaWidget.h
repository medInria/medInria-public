#pragma once
/*
 * medInria
 * Copyright (c) INRIA 2013. All rights reserved.
 * 
 * medInria is under BSD-2-Clause license. See LICENSE.txt for details in the root of the sources or:
 * https://github.com/medInria/medInria-public/blob/master/LICENSE.txt
 * 
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidget>

#include <medSourcesLoader.h>
#include <medWidgetsExport.h>

class MEDWIDGETS_EXPORT medSourceSettingsDragAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit medSourceSettingsDragAreaWidget(medSourcesLoader * pSourceLoader, QTreeWidget * sourceInformation, QWidget *parent = nullptr);

    void addNewSourceItem(medAbstractSource * pSource);
protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    medSourcesLoader * _pSourceLoader;
    QTreeWidget * _sourceInformation;

    QVBoxLayout * sourceColumnLayout;
};
