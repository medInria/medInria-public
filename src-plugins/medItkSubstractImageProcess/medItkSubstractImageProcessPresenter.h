/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractSubstractImageProcessPresenter.h>

class medItkSubstractImageProcessPresenterPrivate;

class medItkSubstractImageProcessPresenter: public medAbstractSubstractImageProcessPresenter
{
public:
    medItkSubstractImageProcessPresenter(QObject *parent = NULL);
    virtual ~medItkSubstractImageProcessPresenter();

    virtual medAbstractSubstractImageProcess* process() const;

    virtual QString name() const {return "yolo";}
    virtual QString description() const {return "yolo";}
    virtual QString version() const {return "yolo";}

private:
    medItkSubstractImageProcessPresenterPrivate *d;
};

// ///////////////////////////////////////////////////////////////////

inline medAbstractSubstractImageProcessPresenter* medItkSubstractImageProcessPresenterCreator(void)
{
    return new medItkSubstractImageProcessPresenter();
}
