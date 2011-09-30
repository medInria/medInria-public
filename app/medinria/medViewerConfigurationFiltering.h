/* @file medViewerConfigurationFiltering.h ---
 *
 * @author Clément Philipot <clement.philipot@inria.fr>
 *
 */

#ifndef medViewerConfigurationFiltering_H
#define medViewerConfigurationFiltering_H

#include <QtCore>
#include <medViewerConfiguration.h>

class medViewerConfigurationFilteringPrivate;
class medViewContainerStack;
class dtkAbstractData;

/**
 * @brief Configuration providing a comparative display of the input and output of image-to-image filtering process plugins
 */
class medViewerConfigurationFiltering : public medViewerConfiguration
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
     * @brief sets up all the signal/slot connections when Viewer is switched to this configuration
     */
    void setupViewContainerStack ();

signals:

    /**
     * @brief signal emitted to refresh the output view with the data resulting from a successful filtering process
     *
     * This is a connection between the medToolBoxFiltering and the medViewContainerFiltering which displays input/output images
     *
     */
    void outputDataChanged(dtkAbstractData *);

public slots:

    /**
     * @brief removes filtering toolboxes when patient changes
     *
     * @param patientId
     */
    void patientChanged(int patientId);

    /**
     * @brief adds metadata to the output and emits a signal outputDataChanged(dtkAbstractData *)
     */
    void onProcessSuccess(void);

    /**
     * @brief Imports output data in non persistent database
     *
     * @param dataIndex The index that was imported.
     * @param uuid The identifier of the caller.
     */
    void onOutputImported(const medDataIndex & dataIndex, const QString& uuid);

private:
    medViewerConfigurationFilteringPrivate *d;
};

medViewerConfiguration *createMedViewerConfigurationFiltering(QWidget* parent);

#endif
