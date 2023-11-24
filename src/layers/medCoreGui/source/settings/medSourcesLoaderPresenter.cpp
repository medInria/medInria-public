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

#include "medSourcesLoaderPresenter.h"

#include <medAbstractParameterPresenter.h>
#include <medSourcesSettings.h>

class medSourcesLoaderPresenterPrivate
{
public:
    medSourcesLoader * pSourceLoader;
};

medSourcesLoaderPresenter::medSourcesLoaderPresenter(medSourcesLoader * parent)
    : d(new medSourcesLoaderPresenterPrivate())
{
    d->pSourceLoader = parent;
}

medSourcesLoaderPresenter::~medSourcesLoaderPresenter()
{
    delete d;
}

QWidget * medSourcesLoaderPresenter::buildWidget()
{
    return new medSourcesSettings(d->pSourceLoader);;
}
