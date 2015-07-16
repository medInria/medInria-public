/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medComposerArea.h>

class medComposerAreaPrivate
{
public:

};


medComposerArea::medComposerArea(QWidget *parent): medAbstractArea(parent),
    d(new medComposerAreaPrivate)
{

}

medComposerArea::~medComposerArea()
{
    delete d;
}

QString medComposerArea::title() const
{
    return "Composer";
}

QString medComposerArea::description() const
{
    return tr("Description of the composer area");
}
