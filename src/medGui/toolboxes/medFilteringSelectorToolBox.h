/* medViewerToolBoxFiltering.h ---
 *
 * @author Clément Philipot <clement.philipot@inria.fr>
 *
 */

#pragma once

#include "medGuiExport.h"
#include "medToolBox.h"
#include <medCore/medDataIndex.h>

class medFilteringSelectorToolBoxPrivate;
class dtkAbstractData;
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
    dtkAbstractData* data();

    /**
     * @brief returns current selected toolbox
     */
    medFilteringAbstractToolBox* customToolbox();

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
     * @brief stores output image to the persistent database
     */
    void onSavedImage();

    /**
     * @brief retrieve data from the selected input image index
     * @param index
     */
    void onInputSelected(const medDataIndex& index);


protected:
    medFilteringSelectorToolBoxPrivate *d;
};


