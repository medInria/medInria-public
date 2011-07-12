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
 * @brief Common toolbox for all filtering processes
 *
 * This toolbox provides a comboBox to switch between filtering process plugins and "Save" buttons to store results in file / database.
 * To specify an input image, drag and drop from the navigator to the medDropSite.
 */
class MEDGUI_EXPORT medToolBoxFiltering : public medToolBox
{
	Q_OBJECT
public:
	medToolBoxFiltering(QWidget *parent);
	~medToolBoxFiltering();
	
	/**
	 * @brief Returns input data
	 */
	dtkAbstractData* data(void);

	/**
	 * @brief Returns current selected toolbox
	 */
	medToolBoxFilteringCustom* customToolbox(void);

	/**
	 * @brief set data index
	 * @param index
	 */
	void setDataIndex(medDataIndex index);

signals:
        /**
         * @brief signals emitted if filtering process is successful
         */
	void processFinished();
	/**
	 * @brief signals emitted when a new input image has been dropped in the medToolBoxFiltering
	 */
	void dataSelected(dtkAbstractData *data);
 
public slots:
    
        /**
         * @brief Instanciates the process toolbox according to the process plugin description
         */
	void onToolBoxChosen(const QString&);
	/**
	 * @brief Clear input data and the current process toolbox
	 */
	void clear(void);
	/**
	 * @brief Instanciates an input data according to the index of data dropped in medDropSite
	 */
	void onObjectDropped(void);
	/**
	 * @brief Stores output image to the persistent database
	 */
	void onSavedImage(void);

    
protected:
	medToolBoxFilteringPrivate *d;
};

#endif
