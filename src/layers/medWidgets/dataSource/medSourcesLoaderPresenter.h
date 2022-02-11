#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QObject>
#include <QWidget>

#include <medWidgetsExport.h>

#include <medSourcesLoader.h>

class medSourcesLoaderPresenterPrivate;
class MEDWIDGETS_EXPORT medSourcesLoaderPresenter : public QObject
{
    Q_OBJECT

public:
    medSourcesLoaderPresenter(medSourcesLoader * parent);
    ~medSourcesLoaderPresenter();

    QWidget * buildWidget();

private:
    medSourcesLoaderPresenterPrivate *d;
};
