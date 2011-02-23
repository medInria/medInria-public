#ifndef MEDTOOLBOXFIBERBUNDLING_H
#define MEDTOOLBOXFIBERBUNDLING_H

#include <medGui/medToolBox.h>

class dtkAbstractView;
class dtkAbstractData;
class medToolBoxFiberBundlingPrivate;

class medToolBoxFiberBundling : public medToolBox
{
    Q_OBJECT
public:
    medToolBoxFiberBundling(QWidget *parent);
    ~medToolBoxFiberBundling();
    
    void setData(dtkAbstractData *data);
    
signals:
    void fiberSelectionValidated(QString name);
    void fiberSelectionTagged(void);
    void fiberSelectionReset(void);
    void showBundles(bool);

public slots:
    void onObjectDropped (void);
    
    void addBundle(QString name);
    void onBundlingButtonVdtClicked (void);
    void onBundleBoxCheckBoxToggled (bool);
    
    void clear(void);
    void update (dtkAbstractView *view);
    
private:
    medToolBoxFiberBundlingPrivate *d;
    
};


#endif
