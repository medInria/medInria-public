/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractSelectableToolBox.h>

class DiffeomorphicDemonsToolBoxPrivate;

/*!
 * \brief Toolbox to apply automatic registration using the ITK Diffeomorphic demons implementation.
 */
class DiffeomorphicDemonsToolBox : public medAbstractSelectableToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("Diffeomorphic Demons",
                          "Diffeomorphic demons registration using the ITK implementation.",
                          <<"Registration")
public:
     DiffeomorphicDemonsToolBox(QWidget *parentToolBox = 0);
    ~DiffeomorphicDemonsToolBox();

public:
    static bool registered();
    dtkPlugin* plugin();
    medAbstractData* processOutput();

public slots:
    void run();

private:
    DiffeomorphicDemonsToolBoxPrivate *d;
};



