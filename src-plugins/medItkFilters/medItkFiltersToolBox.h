/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medProcessSelectorToolBox.h>

class medItkFiltersToolBoxPrivate;

class medItkFiltersToolBox : public medProcessSelectorToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("ITK Basic Filters",
                          "ITK filters, processing examples",
                          <<"filtering")
public:
    medItkFiltersToolBox(QWidget *parentToolBox = 0);
    ~medItkFiltersToolBox();

public:
    static bool registered();

public slots:
    void clear();
    void update(medAbstractData *data);

private:
    medItkFiltersToolBoxPrivate *d;
};

medToolBox *createmedItkFiltersToolBox(QWidget *parent);


