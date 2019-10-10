/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medComboBox.h>
#include <medCoreLegacyExport.h>
#include <medToolBox.h>

class medAbstractSelectableToolBox;
class medSelectorToolBoxPrivate;

class MEDCORELEGACY_EXPORT medSelectorToolBox : public medToolBox
{
    Q_OBJECT

public:
    medSelectorToolBox(QWidget *parent, QString tlbxId);
    ~medSelectorToolBox();

    /**
     * @brief returns input data
     */
    medAbstractData* data();

    /**
     * @brief returns current selected toolbox
     */
    medAbstractSelectableToolBox* currentToolBox();

    /**
     * @brief returns the comboBox
     */
    medComboBox* comboBox();

    int getIndexOfToolBox(const QString &toolboxName);

signals:
    void inputChanged();
    void currentToolBoxChanged();

public slots:

    /**
     * @brief instantiates the right process toolbox according to its description
     */
    virtual void changeCurrentToolBox(int index);
    virtual void changeCurrentToolBox(const QString &identifier);

    /**
     * @brief clear input data and the current process toolbox
     */
    virtual void clear();

    /**
     * @brief retrieve data from the selected input image index
     * @param index
     */
    void onInputSelected(medAbstractData *data);

private:
    medSelectorToolBoxPrivate *d;
};


