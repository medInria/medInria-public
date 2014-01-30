/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medToolBox.h>

#include <v3dViewPluginExport.h>

class dtkAbstractView;
class medAbstractData;
class medFiberBundlingToolBoxPrivate;
class medDataIndex;

/**
  * @class medFiberBundlingToolBox
  * @author Pierre Fillard
  * @brief Toolbox to extract bundle from fibers and to compute statistics of those
  * This toolbox allows to perform fiber bundling. It allows to bundle fibers (i.e.,
  * declare groups of fibers as belonging to the same anatomical bundle), name
  * and color bundles, and compute and display image and length statistics.
  */
class V3DVIEWPLUGIN_EXPORT medFiberBundlingToolBox : public medToolBox
{
    Q_OBJECT
public:
     medFiberBundlingToolBox(QWidget *parent);
    ~medFiberBundlingToolBox();

    static bool registered();

    /**
      * Set input fibers as a medAbstractData object. Subclass should
      * inherit it and cass it into proper fiber data type.
      */
    virtual void setData(medAbstractData *data);

signals:
    /**
      * This signal is emitted when the user want to bundle fibers on screen.
      * @param name Name of the bundle
      * @param color Color of the bundle
      */
    void fiberSelectionValidated (const QString &name, const QColor &color);

    /**
      * This signal is emitted when the user wants to navigate into the
      * subset of visible fibers (usefull for recursive fiber bundling).
      */
    void fiberSelectionTagged();

    /**
      * This signal is emitted when the user wants to reset the fiber
      * navigation to the entire set of fibers.
      */
    void fiberSelectionReset();

public slots:

    void setInput(medAbstractData * data);

protected slots:

    /**
      * When update() is called, this toolbox automatically searches for a subclass
      * of a medAbstractFiberViewInteractor and set its input to the interactor's
      * input. It adapts its GUI automatically (populate the list of bundles).
      */
    virtual void update (dtkAbstractView *view);

    /**
     * Clears the toolbox. Removes any bundle in the fiber bundle treeview,
     * any ROI previously loaded (if any), etc.
     */
    virtual void clear();

    /**
      * Add a bundle (identified by a name and a color) to the list of bundles.
      */
    virtual void addBundle (const QString &name, const QColor &color);

    /**
     * Save all bundles to database as individual files
     */
    virtual void saveBundles ();
    
    /** Slot called when external ROI image finishes being imported. */
    virtual void importROI(const medDataIndex &index);

    /**
     * Slot called when the @meDropSite is clicked.
     * Will open a @QFileDialog so the user can choose
     * and external ROI image to open.
     */
    virtual void onDropSiteClicked();

    /**
     * Sets the image passed as parameter as the @medDropSite image.
     */
    void setImage(const QImage& thumbnail);

    // internal method, doc to come
    virtual void validateBundling();
    virtual void setBoxBooleanOperation (bool);
    virtual void showBundlingBox (bool);
    virtual void showBundling (bool show);

    virtual void clearRoi();
    virtual void selectRoi  (int value);
    virtual void setRoiAddOperation      (bool value);
    virtual void setRoiNotOperation      (bool value);
    virtual void setRoiNullOperation     (bool value);

    virtual void changeBundlingItem (QStandardItem *item);

private:
    medFiberBundlingToolBoxPrivate *d;

};



