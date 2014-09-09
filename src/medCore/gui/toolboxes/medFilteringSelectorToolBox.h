/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>
#include <medToolBox.h>
#include <medDataIndex.h>

class medFilteringSelectorToolBoxPrivate;
class medAbstractData;
class medFilteringAbstractToolBox;

/**
 * @brief main toolbox for filtering processes
 *
 * This toolbox provides a comboBox to switch between filtering process plugins and buttons to store results in a file or database.
 */
class MEDCORE_EXPORT medFilteringSelectorToolBox : public medToolBox
{
    Q_OBJECT
public:
    medFilteringSelectorToolBox(QWidget *parent);
    ~medFilteringSelectorToolBox();

    medAbstractData* data();

    medFilteringAbstractToolBox* currentToolBox();

signals:
    //! signal emitted whenever a filtering process is successful
    void processFinished();

public slots:
    void changeCurrentToolBox(int index);
    void clear();
    void onInputSelected(medAbstractData *data);

protected:
    medFilteringSelectorToolBoxPrivate *d;
};


