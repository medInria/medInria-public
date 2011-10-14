/* medViewerToolBoxFiltering.h ---
 *
 * @author Clément Philipot <clement.philipot@inria.fr>
 *
 */

#ifndef MEDTOOLBOXFILTERING_H
#define MEDTOOLBOXFILTERING_H

#include "medGuiExport.h"
#include "medToolBox.h"
#include <medCore/medDataIndex.h>

class medToolBoxFilteringPrivate;
class dtkAbstractData;
class medToolBoxFilteringCustom;

/**
 * @brief main toolbox for filtering processes
 *
 * This toolbox provides a comboBox to switch between filtering process plugins and buttons to store results in a file or database.
 */
class MEDGUI_EXPORT medToolBoxFiltering : public medToolBox
{
    Q_OBJECT
public:
    medToolBoxFiltering(QWidget *parent);
    ~medToolBoxFiltering();

    /**
     * @brief returns input data
     */
    dtkAbstractData* data(void);

    /**
     * @brief returns current selected toolbox
     */
    medToolBoxFilteringCustom* customToolbox(void);

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
    void clear(void);

    /**
     * @brief stores output image to the persistent database
     */
    void onSavedImage(void);

    /**
     * @brief retrieve data from the selected input image index
     * @param index
     */
    void onInputSelected(const medDataIndex& index);


protected:
    medToolBoxFilteringPrivate *d;
};

#endif
