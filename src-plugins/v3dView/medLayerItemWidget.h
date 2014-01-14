#pragma once

#include <QWidget>

class QPushButton;
class QSlider;
class QLabel;
class medAbstractData;
class medAbstractVtkViewInteractor;
class QListWidget;

class medLayerItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit medLayerItemWidget(QString name,
                                QString thumbnailPath,
                                medAbstractData * dataItem,
                                QList<medAbstractVtkViewInteractor*> interactorsList,
                                QWidget * parent = 0);

    virtual ~medLayerItemWidget();

    virtual QSize sizeHint();

    medAbstractData* dataItem() { return data; }


protected slots:
    void changeOpacity(int opacity);
    void showLayer(bool show);
    void removeLayer();

signals:
    void deletionRequested();

private:
    QPushButton * thumbnailButton;
    QSlider * opacitySlider;
    QPushButton * removeButton;
    QLabel * layerName;
    medAbstractData * data;
    QList<medAbstractVtkViewInteractor*> interactors;
};
