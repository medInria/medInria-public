#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QListWidget>
#include <QObject>
#include <QStackedWidget>

#include <medWidgetsExport.h>

class medDataHub;
class medSourcesWidget;
class medDataHubPresenterPrivate;

class MEDWIDGETS_EXPORT medDataHubPresenter : public QObject
{
    Q_OBJECT
public:
    medDataHubPresenter(medDataHub *parent = nullptr);
    virtual ~medDataHubPresenter();

    virtual QWidget *buildWidget();

    virtual medSourcesWidget *buildTree();

    virtual QStackedWidget *buildBrowser();

    virtual QListWidget *buildSourceList();

    virtual  QStackedWidget *buildFilters();

    //virtual medDataHub *dataHub() const;

signals:
    void filterProxy(const QString &);

private:
    const QScopedPointer<medDataHubPresenterPrivate> d;
};
