#ifndef MEDTOOLBOXDIFFUSIONFIBERBUNDLING_H
#define MEDTOOLBOXDIFFUSIONFIBERBUNDLING_H

#include "medToolBox.h"
#include "medGuiExport.h"

class dtkAbstractView;
class dtkAbstractData;
class medToolBoxDiffusionFiberBundlingPrivate;

class MEDGUI_EXPORT medToolBoxDiffusionFiberBundling : public medToolBox
{
    Q_OBJECT
public:
    medToolBoxDiffusionFiberBundling(QWidget *parent);
    ~medToolBoxDiffusionFiberBundling();
    
    void setData(dtkAbstractData *data);
    
signals:
    void fiberSelectionValidated(const QString &name, const QColor &color);
    void fiberSelectionTagged(void);
    void fiberSelectionReset(void);
    void showBundles(bool);

public slots:
    void addBundle(const QString &name, const QColor &color);
    void onBundlingButtonVdtClicked (void);
    void onBundleBoxCheckBoxToggled (bool);

    void onOpenRoiButtonClicked (void);
    void onRoiComboIndexChanged (int value);
    void onAddButtonToggled (bool value);
    void onNotButtonToggled (bool value);
    void onNullButtonToggled (bool value);
    
    void clear(void);
    void update (dtkAbstractView *view);
    
private:
    medToolBoxDiffusionFiberBundlingPrivate *d;
    
};


#endif
