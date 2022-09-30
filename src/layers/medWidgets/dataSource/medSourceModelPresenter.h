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
class medSourceModelPresenterPrivate;

class MEDWIDGETS_EXPORT medSourceModelPresenter : public QObject
{
    Q_OBJECT
public:
    medSourceModelPresenter(medDataHub *parent = nullptr);
    virtual ~medSourceModelPresenter();

    virtual QWidget *buildWidget();

    virtual medSourcesWidget *buildTree();

    virtual QStackedWidget *buildBrowser();

    virtual QListWidget *buildSourceList();

    virtual  QStackedWidget *buildFilters();

    //virtual medDataHub *dataHub() const;

signals:
    void filterProxy(const QString &);

private:
    const QScopedPointer<medSourceModelPresenterPrivate> d;
};
