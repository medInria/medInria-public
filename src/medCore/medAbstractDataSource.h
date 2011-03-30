#ifndef MEDABSTRACTDATASOURCE_H
#define MEDABSTRACTDATASOURCE_H

//#include <dtkCore/dtkAbstractObject.h>
#include <QtCore>
#include "medCoreExport.h"

class medToolBox;

/** 
 * @class medAbstractDataSource
 * @brief Abstract base class for dynamic data sources, e.g. plugins that act as database clients
 * This class defines access methods to the following widgets: a mainViewWidget, 
 * a source selection widget and several toolboxes.
 * All dynamic data source implementation should derive from this class.
 **/
class MEDCORE_EXPORT medAbstractDataSource : public QObject
{
    Q_OBJECT
    
public:
    medAbstractDataSource(QWidget *parent = 0);
    ~medAbstractDataSource(void);

    /** 
    * Returns the main view widget 
    * This widget is used to navigate within the data, e.g. a qTreeWidget
    */
    virtual QWidget *mainViewWidget() = 0;
    
    /** 
    * Returns the source selector widget 
    * A widget that let's the user choose between different data locations
    */
    virtual QWidget *sourceSelectorWidget() = 0;
    
    /** Returns the tab name for the plugin using the data source*/
    virtual QString tabName() = 0;
  
    /** Returns all toolboxes owned by the source data plugin*/
    virtual QList<medToolBox*> getToolboxes() = 0;

signals:
    /** A source data emits a signal when it successfully received the data and is ready for importing*/
    void dataReceived(QString pathToData);
  
    /** A data source emits a signal when it failed to get the data*/
    void dataReceivingFailed(QString pathToData);

};

#endif
