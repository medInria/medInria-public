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

class QWidget;
class QTreeView;
class medVirtualRepresentation;
class medVirtualRepresentationPresenterPrivate;

class MEDWIDGETS_EXPORT medVirtualRepresentationPresenter : public QObject
{
    Q_OBJECT
public:
    medVirtualRepresentationPresenter(medVirtualRepresentation *parent = nullptr);
    virtual ~medVirtualRepresentationPresenter();

    virtual QWidget *buildWidget();

    virtual QTreeView *buildTree();

signals:

private:
    const QScopedPointer<medVirtualRepresentationPresenterPrivate> d;
};
