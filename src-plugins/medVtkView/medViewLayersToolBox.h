//#ifndef MEDVIEWLAYERSTOOLBOX_H
//#define MEDVIEWLAYERSTOOLBOX_H

//#include <medGuiExport.h>
//#include <medToolBox.h>
//#include <medParameter.h>

//class QListWidget;
//class medVtkView;
//class medAbstractVtkViewInteractor;
//class medVtkViewToolBox;
//class medViewLayersToolBoxPrivate;

///**

//   \class medLayersPropertiesToolBox
//   \brief This toolboxes manages the different layers in the views.

//   \ingroup Toolboxes
//*/
//class MEDGUI_EXPORT medViewLayersToolBox : public medToolBox
//{
//    Q_OBJECT
//public:
//    explicit medViewLayersToolBox(QWidget *parent = 0);

//public slots:
//    void updateViews();
//    void updateLayerListWidget(QList<medVtkView*>);
//    void resetList();
//    void updateParameters();
//    void updateViewPool(QList<medVtkView*>);
//    void updateParameters(QMultiMap<medVtkView*, int>);
//    void removeView();
//    void removeLayer();

//public:
//    void update(dtkAbstractView * view);
//    static bool registered();
//    void clearParams();
//    void addView(medVtkView*);
//    void selectView(medVtkView*);

//signals:
//    void viewSelected();

//private:

//    medViewLayersToolBoxPrivate *d;

//};

//#endif // MEDVIEWLAYERSTOOLBOX_H
