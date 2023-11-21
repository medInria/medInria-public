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

#include <QObject>

#include <medSourcesLoader.h>
#include <medCoreGuiExport.h>

class medSourcesLoaderPresenterPrivate;
class MEDCOREGUI_EXPORT medSourcesLoaderPresenter : public QObject
{
    Q_OBJECT

public:
    medSourcesLoaderPresenter(medSourcesLoader * parent);
    ~medSourcesLoaderPresenter();

    QWidget * buildWidget();

private:
    medSourcesLoaderPresenterPrivate *d;
};
