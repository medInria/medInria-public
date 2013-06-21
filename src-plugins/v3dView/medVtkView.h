#pragma once

#include <v3dView.h>
#include <dtkCore/dtkSmartPointer.h>

class medVtkView : public v3dView
{
    Q_OBJECT
    
public:
    medVtkView();
    virtual ~medVtkView();
    
    virtual QString identifier() const;
    static QString s_identifier();
    
    static bool registered();
    
    void setData (dtkAbstractData *data, int layer);
    
    dtkAbstractData *layerData(int layer);
    unsigned int numberOfLayers();
    
private:
    QList < dtkSmartPointer <dtkAbstractData> > layersDataList;
};

dtkAbstractView *createMedVtkView();
