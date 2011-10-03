/* medViewContainerFiltering.h ---
 *
 * @author Clément Philipot <clement.philipot@inria.fr>
 *
 */

#ifndef MEDVIEWCONTAINERFILTERING_H
#define MEDVIEWCONTAINERFILTERING_H

#include <medCore/medDataIndex.h>
#include <medCore/medDataManager.h>
#include <medViewContainerCustom.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractView.h>

#include "medGuiExport.h"

class medViewContainerFilteringPrivate;

/**
 * @brief filtering view container to visualize both input / output from image-to-image filtering processes
 */
class MEDGUI_EXPORT medViewContainerFiltering : public medViewContainerCustom
{
    Q_OBJECT

public:
    /**
     * @brief
     * @param parent
     */
    medViewContainerFiltering ( QWidget * parent = 0 );
    ~medViewContainerFiltering();

signals:

    /**
     * @brief signals emitted whenever an image has been dropped in the input view container
     */
    void droppedInput ( const medDataIndex& index );

public slots:

    /**
     * @brief refresh input view according to the data index given in parameter
     * @param index
     */
    void updateInput ( const medDataIndex& index );
    /**
     * @brief update output view according to the data in parameter
     * @param data
     */
    void updateOutput ( dtkAbstractData *data );

private:
    medViewContainerFilteringPrivate *d3;
};

#endif // MEDVIEWCONTAINERFILTERING_H
