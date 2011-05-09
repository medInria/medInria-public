#ifndef MEDTOOLBOXSEGMENTATIONVIEW_H
#define MEDTOOLBOXSEGMENTATIONVIEW_H

#include <medGui/medToolBox.h>

#include "medViewSegmentationPluginExport.h"


class medToolBoxSegmentationViewPrivate;

class MEDVIEWSEGMENTATIONPLUGIN_EXPORT medToolBoxSegmentationView : public medToolBox
{
    Q_OBJECT
public:
    medToolBoxSegmentationView(QWidget *parent);
    ~medToolBoxSegmentationView();

signals:
    void addSeedPointPressed();

public slots:
    void update (dtkAbstractView *view);

protected:
    medToolBoxSegmentationViewPrivate *d;
};

#endif // MEDTOOLBOXSEGMENTATIONVIEW_H
