/* medViewContainerFiltering.h ---
 *
 * @author Clément Philipot <clement.philipot@inria.fr>
 *
 */

#ifndef MEDVIEWCONTAINERFILTERING_H
#define MEDVIEWCONTAINERFILTERING_H

#include <medGui/medViewContainerCustom.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractView.h>

#include "medGuiExport.h"

class medViewContainerFilteringPrivate;


/**
 * @brief Comparative view container to visualize both input / output from image-to-image filtering processes
 */
class MEDGUI_EXPORT medViewContainerFiltering : public medViewContainerCustom
{
    Q_OBJECT

public:
    /**
     * @brief
     * @param parent
     */
    medViewContainerFiltering(QWidget * parent = 0);
    ~medViewContainerFiltering();

public slots:

    /**
     * @brief Update input view with the data in parameter
     * @param data
     */
    void updateInput(dtkAbstractData *data);
    /**
     * @brief Update output view with the data in parameter
     * @param data
     */
    void updateOutput(dtkAbstractData *data);

private:
    medViewContainerFilteringPrivate *d3;
};

#endif // MEDVIEWCONTAINERFILTERING_H
