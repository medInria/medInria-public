#ifndef MEDVIEWLAYERSTOOLBOX_H
#define MEDVIEWLAYERSTOOLBOX_H

#include <medGuiExport.h>
#include <medToolBox.h>

class QListWidget;
class medVtkView;
class medAbstractVtkViewInteractor;
/**

   \class medLayersPropertiesToolBox
   \brief This toolboxes manages the different layers in the views.

   \ingroup Toolboxes
*/
class MEDGUI_EXPORT medViewLayersToolBox : public medToolBox
{
    Q_OBJECT
public:
    explicit medViewLayersToolBox(QWidget *parent = 0);

public slots:
    void updateLayerList();
    void resetList();

public:
    // --------------- Temporary view --------------------------
    void update(dtkAbstractView * view);
    static bool registered();

private:
    QListWidget * layersList;
    medVtkView * vtkView;
};

#endif // MEDVIEWLAYERSTOOLBOX_H
