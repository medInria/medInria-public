/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtGui>
#include <QtWidgets>

class medComposerAreaPrivate;

class medComposerArea : public QWidget
{
    Q_OBJECT

public:
     medComposerArea(QWidget *parent = 0);
    ~medComposerArea();

public slots:

private:
    medComposerAreaPrivate *d;
};
