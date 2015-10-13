#pragma once

#include <dtkCore>

#include <QObject>
#include <QRunnable>

#include "medCore.h"
#include "medCoreExport.h"

// ///////////////////////////////////////////////////////////////////
// medAbstractSegmentationProcess process interface
// ///////////////////////////////////////////////////////////////////

class MEDCORE_EXPORT medAbstractSegmentationProcess : public QObject, public QRunnable
{
    Q_OBJECT

public:
             medAbstractSegmentationProcess(QObject *parent = NULL) : QObject(parent), QRunnable() {}
    virtual ~medAbstractSegmentationProcess(void) {}
    
public:
    virtual void setImage(medAbstractImageData* image) = 0;




public:


};

// ///////////////////////////////////////////////////////////////////
// 
// ///////////////////////////////////////////////////////////////////
DTK_DECLARE_OBJECT        (medAbstractSegmentationProcess*)
DTK_DECLARE_PLUGIN        (medAbstractSegmentationProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractSegmentationProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractSegmentationProcess, MEDCORE_EXPORT)

