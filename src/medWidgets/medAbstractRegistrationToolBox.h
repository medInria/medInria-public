#pragma once

#include <QWidget>
#include "medCore/process/medAbstractRegistrationProcess.h"
#include "medWidgetsExport.h"

class MEDWIDGETS_EXPORT medAbstractRegistrationToolbox : public QWidget
{
    Q_OBJECT
    
public:
    medAbstractRegistrationToolbox(QWidget* parent=0):QWidget(parent){}
    ~medAbstractRegistrationToolbox(){}
    
public:
    virtual void setProcess(medAbstractRegistrationProcess*)=0;

};
