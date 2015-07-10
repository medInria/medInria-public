/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractSubtractImageProcessPresenter.h>

class medItkSubtractImageProcessPresenterPrivate;

class medItkSubtractImageProcessPresenter: public medAbstractSubtractImageProcessPresenter
{
public:
    medItkSubtractImageProcessPresenter(QObject *parent = NULL);
    virtual ~medItkSubtractImageProcessPresenter();

    virtual medAbstractSubtractImageProcess* process() const;

    virtual QString name() const {return "yolo";}
    virtual QString description() const {return "yolo";}
    virtual QString version() const {return "yolo";}

private:
    medItkSubtractImageProcessPresenterPrivate *d;
};

// ///////////////////////////////////////////////////////////////////

inline medAbstractSubtractImageProcessPresenter* medItkSubtractImageProcessPresenterCreator(void)
{
    return new medItkSubtractImageProcessPresenter();
}
