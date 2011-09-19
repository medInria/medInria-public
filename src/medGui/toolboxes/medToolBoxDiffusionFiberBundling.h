#ifndef MEDTOOLBOXDIFFUSIONFIBERBUNDLING_H
#define MEDTOOLBOXDIFFUSIONFIBERBUNDLING_H

#include "medToolBox.h"

#include "medGuiExport.h"

class dtkAbstractView;
class dtkAbstractData;
class medToolBoxDiffusionFiberBundlingPrivate;
class medDataIndex;

/**
  * @class medToolBoxDiffusionFiberBundling
  * @author Pierre Fillard
  * @brief Toolbox to extract bundle from fibers and to compute statistics of those
  * This toolbox allows to perform fiber bundling. It must be used with subclasses
  * of @class medAbstractViewFiberInteractor. It allows to bundle fibers (i.e.,
  * declare groups of fibers as belonging to the same anatomical bundle), name
  * and color bundles, and compute and display FA, ADC and length statistics.
  */
class MEDGUI_EXPORT medToolBoxDiffusionFiberBundling : public medToolBox
{
    Q_OBJECT
public:
     medToolBoxDiffusionFiberBundling(QWidget *parent);
    ~medToolBoxDiffusionFiberBundling();
    
    /**
      * Set input fibers as a dtkAbstractData object. Subclass should
      * inherit it and cass it into proper fiber data type.
      */
    virtual void setData(dtkAbstractData *data);
    
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
    void fiberSelectionTagged    (void);

    /**
      * This signal is emitted when the user wants to reset the fiber
      * navigation to the entire set of fibers.
      */
    void fiberSelectionReset     (void);

    /**
      * This signal is emitted when the user wants to change the
      * boolean meaning of a ROI.
      * @param value Value of the ROI to be changed
      */
    void bundlingBoxBooleanOperatorChanged (int value);

public slots:
    /**
      * When update() is called, this toolbox automatically searches for a subclass
      * of a medAbstractFiberViewInteractor and set its input to the interactor's
      * input. It adapts its GUI automatically (populate the list of bundles).
      */
    virtual void update (dtkAbstractView *view);
    virtual void clear  (void);

    /**
      * Add a bundle (identified by a name and a color) to the list of bundles.
      */
    virtual void addBundle (const QString &name, const QColor &color);

    // internal method, doc to come
    virtual void onBundlingButtonVdtClicked (void);
    virtual void onBundlingButtonAndToggled (bool);
    virtual void onBundleBoxCheckBoxToggled (bool);
    virtual void onBundlingShowCheckBoxToggled (bool);

    virtual void onOpenRoiButtonClicked  (void);
    virtual void onClearRoiButtonClicked (void);
    virtual void onRoiComboIndexChanged  (int value);
    virtual void onAddButtonToggled      (bool value);
    virtual void onNotButtonToggled      (bool value);
    virtual void onNullButtonToggled     (bool value);

    virtual void onBundlingItemChanged (QStandardItem *item);
    
    virtual void onRoiImported(const medDataIndex &index);

private:
    medToolBoxDiffusionFiberBundlingPrivate *d;
    
};


#endif
