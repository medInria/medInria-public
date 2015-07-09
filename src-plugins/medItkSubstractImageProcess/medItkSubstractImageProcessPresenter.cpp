/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkSubstractImageProcessPresenter.h>
#include <medItkSubstractImageProcess.h>

#include <QLabel>

class medItkSubstractImageProcessPresenterPrivate
{
public:
    medItkSubstractImageProcess* process;
};


medItkSubstractImageProcessPresenter::medItkSubstractImageProcessPresenter(QObject *parent): medAbstractSubstractImageProcessPresenter(parent),
    d(new medItkSubstractImageProcessPresenterPrivate)
{
    d->process = new medItkSubstractImageProcess(this);
}

medItkSubstractImageProcessPresenter::~medItkSubstractImageProcessPresenter()
{
    delete d;
}

medAbstractSubstractImageProcess* medItkSubstractImageProcessPresenter::process() const
{
    return d->process;
}
