/* @file medViewerConfigurationFiltering.h ---
 *
 * @author Clément Philipot <clement.philipot@inria.fr>
 *
 */

#ifndef medViewerConfigurationFiltering_H
#define medViewerConfigurationFiltering_H

#include <QtCore>

#include <medGui/medViewerConfiguration.h>

class medViewerConfigurationFilteringPrivate;
class medViewContainerStack;
class dtkAbstractData;

/**
 * @brief Configuration providing a comparative display of the input and output of image-to-image filtering process plugins
 */
class MEDGUI_EXPORT medViewerConfigurationFiltering : public medViewerConfiguration
{
    Q_OBJECT

public:
    /**
     * @brief
     *
     * @param parent
     */
     medViewerConfigurationFiltering(QWidget *parent = 0);
    ~medViewerConfigurationFiltering(void);
    
    virtual QString description(void) const;
    
    /**
     * @brief Sets up all the signal/slot connections when Viewer is switched to this configuration
     */
    void setupViewContainerStack ();

signals:

    /**
     * @brief Signal emitted when an new input image has been dropped in the medToolBoxFiltering
     *
     * This is a connection between the medToolBoxFiltering and the medViewContainerFiltering which displays input/output images
     */
    void inputDataChanged(dtkAbstractData *);

    /**
     * @brief Signal emitted to refresh the output view with the data resulting from a successful filtering process
     *
     * This is a connection between the medToolBoxFiltering and the medViewContainerFiltering which displays input/output images
     *
     */
    void outputDataChanged(dtkAbstractData *);

public slots:    
    /**
     * @brief Removes filtering toolboxes when patient changes
     *
     * @param patientId
     */
    void patientChanged(int patientId);

    /**
     * @brief Adds metadata to the output and emits a signal outputDataChanged(dtkAbstractData *)
     */
    void onProcessSuccess(void);

private:
    medViewerConfigurationFilteringPrivate *d;
};

MEDGUI_EXPORT medViewerConfiguration *createMedViewerConfigurationFiltering(void);

#endif
