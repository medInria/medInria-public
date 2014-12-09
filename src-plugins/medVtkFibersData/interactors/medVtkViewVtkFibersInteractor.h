/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractImageViewInteractor.h>

#include <medVtkFibersDataPluginExport.h>

class medAbstractData;
class medAbstractImageView;
class medAbstractParameter;

class medVtkViewVtkFibersInteractorPrivate;

/**
 * @class medVtkViewVtkFibersInteractor
 * @brief Class for view interactor to display fibers.
 * Fiber visualization can be controlled with Regions of Interest (ROI), which
 * are images of unsigned char defining regions of voxels where fibers should
 * (or shouldn't) pass. The meaning of each ROI can be controlled with the
 * setRoiBoolean() method: 0 is NULL (ROI is deactivated), 1 (fibers should not pass
 * through the ROI), and 2 (fibers should pass through the ROI).
 * Moreover, this class offers an interface to fiber bundle statistics: given a
 * fiber bundle (identified by a name), subclass should implement calculation of
 * FA, ADC, etc. statistics of the entire bundle.
 * Fiber bundle visibility can also be controlled.
 */
class MEDVTKFIBERSDATAPLUGIN_EXPORT medVtkViewVtkFibersInteractor: public medAbstractImageViewInteractor
{
    Q_OBJECT

public:
    medVtkViewVtkFibersInteractor(medAbstractView *parent = NULL);
    virtual ~medVtkViewVtkFibersInteractor();

    virtual QString description() const;
    virtual QString identifier() const;
    virtual QStringList handled() const;

    virtual bool isDataTypeHandled(QString dataType) const;

    static bool registered();

    virtual void setInputData(medAbstractData *data);

    virtual QWidget* buildLayerWidget();
    virtual QWidget* buildToolBarWidget();
    virtual QWidget* buildToolBoxWidget();

    virtual QList<medAbstractParameter*> linkableParameters();
    virtual QList<medBoolParameter*> mouseInteractionParameters();

    /**
     * Set the boolean meaning for a region of a ROI:
     *  - 0: NULL (region has no effect)
     *  - 1: NOT  (fibers should not pass through the region)
     *  - 2: AND  (fibers should pass through the region)
     * Default is 2 (AND) for all ROIs.
     * @param roi the value of the region
     * @param meaning the region meaning
     * @return void
     */

    void setRoiBoolean(int roi, int meaning);
    /**
     * Access method to the roi meaning (see setRoi()).
     * @param roi the value of the region
     * @return the boolean meaning of the region
     */
    int  roiBoolean (int roi);

    /**
     * Access method to the visibility of a fiber bundle.
     * @param name fiber bundle identified by name
     * @return visibility of the fiber bundle.
     */
    bool bundleVisibility(const QString &name) const;


    /**
     * Triggers the computation of image related statistics of a fiber bundle.
     * An internal call to computeBundleImageStatistics() is made.
     * @param name identifies the fiber bundle
     * @param mean mean bundle image related values
     * @param min  minimum bundle image related values
     * @param max  maximum bundle image related values
     * @param var  variance of bundle image related values
     */
    void bundleImageStatistics (const QString &bundleName,
                                QMap <QString, double> &mean,
                                QMap <QString, double> &min,
                                QMap <QString, double> &max,
                                QMap <QString, double> &var);

    /**
     * Triggers the computation of the length statistics of a fiber bundle.
     * An internal call to computeBundleLengthtatistics() is made. Values
     * are cached for faster subsequent access to Length statistics.
     * @param name identifies the fiber bundle
     * @param mean mean bundle Length value
     * @param min  minimum bundle Length value
     * @param max  maximum bundle Length value
     * @param var  variance of bundle Length
     */
    void bundleLengthStatistics (const QString &name,
                                 double &mean,
                                 double &min,
                                 double &max,
                                 double &var);

    enum RenderingMode {
        Lines,
        Ribbons,
        Tubes
    };

    enum BooleanOperation {
        Plus,
        Minus
    };

    void changeBundleName(QString oldName, QString newName);

public slots:
    void setVisibility (bool visible);
    void setBoxVisibility (bool visible);
    void selectLineMode(bool);
    void selectRibbonMode(bool);
    void selectTubeMode(bool);
    void activateGPU (bool activate);
    void setFiberColorMode(QString mode);
    void setBoxBooleanOperation(bool value);
    void setProjection (const QString& value);
    void setRadius(int value);
    void validateBundling();
    void saveBundlesInDataBase();
    void changeBundlingItem(QStandardItem *item);
    void setRoiAddOperation (bool value);
    void setRoiNotOperation (bool value);
    void setRoiNullOperation (bool value);
    void importROI(const medDataIndex &index);
    void selectRoi(int value);

    void bundlingListCustomContextMenu(const QPoint &point);
    void saveCurrentBundle();
    void removeCurrentBundle();

    void setAllBundlesVisibility(bool visibility);
    void setBundleVisibility(const QString &name, bool visibility);

    void tagSelection();
    void resetSelection();
    void validateSelection (const QString &name, const QColor &color);

    // Mandatory implementations from medVtkViewInteractor
    virtual void setOpacity(double opacity);
    virtual void setVisible(bool visible);
    virtual void setWindowLevel (QHash<QString, QVariant>);

    virtual void clearRoi();

    virtual void moveToSlice  (int slice);

    virtual void setUpViewForThumbnail();

protected slots:
    virtual void updateWidgets();

protected:
    /**
     * Internal method to actually compute the Length statistics of a fiber
     * bundle. Should be reimplemented in subclasses.
     * @param name identifies the fiber bundle
     * @param mean mean bundle Length value
     * @param min  minimum bundle Length value
     * @param max  maximum bundle Length value
     * @param var  variance of bundle Length
     */
    virtual void computeBundleLengthStatistics (const QString &name,
                                                double &mean,
                                                double &min,
                                                double &max,
                                                double &var);

    void addBundle (const QString &name, const QColor &color);
    void setBoxBooleanOperation (BooleanOperation op);
    void setRenderingMode (RenderingMode mode);

    void removeData();

    /**
     * Clear the statistics (removes pre-computed from the cache).
     */
    void clearStatistics();


private:
    static QStringList dataHandled();

private slots:
    void updateSlicingParam();

private:
    medVtkViewVtkFibersInteractorPrivate *d;
};


