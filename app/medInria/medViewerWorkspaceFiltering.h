/* @file medViewerWorkspaceFiltering.h ---
 *
 * @author Clément Philipot <clement.philipot@inria.fr>
 *
 */

#ifndef medViewerWorkspaceFiltering_H
#define medViewerWorkspaceFiltering_H

#include <QtCore>
#include <medViewerWorkspace.h>

class medViewerWorkspaceFilteringPrivate;
class medViewContainerStack;
class dtkAbstractData;
class dtkAbstractView;

/**
 * @brief Workspace providing a comparative display of the input and output of image-to-image filtering process plugins
 */
class medViewerWorkspaceFiltering : public medViewerWorkspace
{
    Q_OBJECT

public:
    /**
     * @brief
     *
     * @param parent
     */
    medViewerWorkspaceFiltering(QWidget *parent = 0);
    ~medViewerWorkspaceFiltering();

    virtual QString identifier()  const;
    virtual QString description() const;
    static bool isUsable();
    /**
     * @brief sets up all the signal/slot connections when Viewer is switched to this workspace
     */
    void setupViewContainerStack ();

signals:

    /**
     * @brief signal emitted to refresh the output view with the data resulting from a successful filtering process
     *
     * This is a connection between the medToolBoxFiltering and the medViewContainerFiltering which displays input/output images
     *
     */
    void outputDataChanged ( dtkAbstractData * );

public slots:

    /**
     * @brief removes filtering toolboxes when patient changes
     *
     * @param patientId
     */
    void patientChanged ( int patientId );

    /**
     * @brief adds metadata to the output and emits a signal outputDataChanged(dtkAbstractData *)
     */
    void onProcessSuccess ( void );

    /**
     * @brief Imports output data in non persistent database
     *
     * @param dataIndex The index that was imported.
     * @param uuid The identifier of the caller.
     */
    void onOutputImported ( const medDataIndex & dataIndex, const QString& uuid );

    /**
     * @brief Clear the filtering toolboxes when the input view is closed
     *
     */
    void onViewRemoved();

private:
    medViewerWorkspaceFilteringPrivate *d;
};

#endif
