/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medToolBoxTab.h>

class medToolBoxTabPrivate
{
public:
};

medToolBoxTab::medToolBoxTab(QWidget *parent) : QTabWidget(parent), d(new medToolBoxTabPrivate)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

medToolBoxTab::~medToolBoxTab(void)
{
    delete d;

    d = NULL;
}



