/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkSubtractImageProcessPresenter.h>
#include <medItkSubtractImageProcess.h>

#include <QLabel>

class medItkSubtractImageProcessPresenterPrivate
{
public:
    medItkSubtractImageProcess* process;
};


medItkSubtractImageProcessPresenter::medItkSubtractImageProcessPresenter(QObject *parent): medAbstractSubtractImageProcessPresenter(parent),
    d(new medItkSubtractImageProcessPresenterPrivate)
{
    d->process = new medItkSubtractImageProcess(this);
}

medItkSubtractImageProcessPresenter::~medItkSubtractImageProcessPresenter()
{
    delete d;
}

medAbstractSubtractImageProcess* medItkSubtractImageProcessPresenter::process() const
{
    return d->process;
}
