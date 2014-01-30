/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medGuiExport.h>
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
class MEDGUI_EXPORT medFilteringSelectorToolBox : public medToolBox
{
    Q_OBJECT
public:
    medFilteringSelectorToolBox(QWidget *parent);
    ~medFilteringSelectorToolBox();

    /**
     * @brief returns input data
     */
    medAbstractData* data();

    /**
     * @brief returns current selected toolbox
     */
    medFilteringAbstractToolBox* currentToolBox();

    /**
     * @brief set data index
     * @param index
     */
    void setDataIndex(medDataIndex index);

signals:
    /**
     * @brief signals emitted whenever a filtering process is successful
     */
    void processFinished();

public slots:

    /**
     * @brief instantiates the right process toolbox according to its description
     */
    void onToolBoxChosen(int index);
    /**
     * @brief clear input data and the current process toolbox
     */
    void clear();

    /**
     * @brief retrieve data from the selected input image index
     * @param index
     */
    void onInputSelected(const medDataIndex& index);


protected:
    medFilteringSelectorToolBoxPrivate *d;
};


