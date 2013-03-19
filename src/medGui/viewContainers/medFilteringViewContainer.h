/* medFilteringViewContainer.h ---
 *
 * @author Clément Philipot <clement.philipot@inria.fr>
 *
 */

#ifndef medFilteringViewContainer_H
#define medFilteringViewContainer_H

#include <medCore/medDataIndex.h>
#include <medCore/medDataManager.h>
#include <medCustomViewContainer.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractView.h>

#include "medGuiExport.h"

class medFilteringViewContainerPrivate;

/**
 * @brief filtering view container to visualize both input / output from image-to-image filtering processes
 */
class MEDGUI_EXPORT medFilteringViewContainer : public medCustomViewContainer
{
    Q_OBJECT

public:
    /**
     * @brief
     * @param parent
     */
    medFilteringViewContainer ( QWidget * parent = 0 );
    ~medFilteringViewContainer();

signals:

    /**
     * @brief signals emitted whenever an image has been dropped in the input view container
     */
    void droppedInput (const medDataIndex& index);
    
public slots:

    /**
     * @brief update output view according to the data in parameter
     * @param data
     */
    void updateOutput ( dtkAbstractData *data );

    void dropEvent ( QDropEvent *event );
    
private:
    medFilteringViewContainerPrivate *d3;
};

#endif // medFilteringViewContainer_H
