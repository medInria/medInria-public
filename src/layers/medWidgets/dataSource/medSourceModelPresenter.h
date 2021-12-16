#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QObject>

#include <medWidgetsExport.h>

class medDataModel;
class medSourcesWidget;
class medSourceModelPresenterPrivate;

class MEDWIDGETS_EXPORT medSourceModelPresenter : public QObject
{
    Q_OBJECT
public:
    medSourceModelPresenter(medDataModel *parent = nullptr);
    virtual ~medSourceModelPresenter();

    virtual QWidget *buildWidget();

    virtual medSourcesWidget *buildTree();

    virtual medDataModel *dataModel() const;

private:
    const QScopedPointer<medSourceModelPresenterPrivate> d;
};
