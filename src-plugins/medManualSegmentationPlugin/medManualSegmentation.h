#pragma once

#include <dtkCore>
#include <medCore.h>

#include <medAbstractSegmentationProcess.h>
#include "medViewContainer.h"


#include "../legacy/medSegmentation/medClickAndMoveEventFilter.h"

class medManualSegmentation : public medAbstractSegmentationProcess
{
    Q_OBJECT

public:
     medManualSegmentation(void);
    ~medManualSegmentation(void);



public:
        virtual void setImage(medAbstractImageData* image);


public: 
    
public:
    void run();

public slots:
    void installFilter(medViewEventFilter*);

private:
        medAbstractImageData* m_image; 
        medViewContainer* m_view;
};


inline medAbstractSegmentationProcess *medManualSegmentationCreator(void)
{
    return new medManualSegmentation();
}
