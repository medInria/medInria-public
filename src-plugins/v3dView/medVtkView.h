#pragma once

#include <medVtkViewPublicInterface.h>

class medVtkViewPrivate;
class medAbstractData;
class medAbstractDataImage;



class medVtkView : public medVtkViewPublicInterface
{
    Q_OBJECT
    
public:

    medVtkView();
    virtual ~medVtkView();

    virtual QString  identifier() const;
    static QString  s_identifier();
    
    static bool registered();

    void setOrientation(medVtkViewOrientation orientation);

protected:
    virtual void addLayer_impl(int layer);
    QWidget *constructToolWidget(QWidget *parent);
    QStringList& getAvailableTransferFunctionPresets() const;
    void setPatientStudySeriesName(medAbstractData *data);
    void setPreferedOrientation(medAbstractDataImage *imageData);

protected slots:
    void processLostOfFocus();
    void moveToSlice(int slice);
    void updateToolWidgets(medAbstractData *data);
    void setZoomMode(medVtkViewZoomMode zoomMode);


private:
    medVtkViewPrivate *d;
};

dtkAbstractView *createMedVtkView();
