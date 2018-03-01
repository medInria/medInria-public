/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractArea.h>

class medComposerAreaPrivate;

class medComposerArea: public medAbstractArea
{
public:
    medComposerArea(QWidget* parent = NULL);
    ~medComposerArea();

    virtual QString title() const;
    virtual QString description() const;

private:
    medComposerAreaPrivate *d;
};

// ///////////////////////////////////////////////////////////////////

inline medAbstractArea* medComposerAreaCreator(void)
{
    return new medComposerArea();
}
