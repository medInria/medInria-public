/* medViewerConfiguration.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu May 13 14:11:43 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Thu May 13 14:22:37 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 5
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDVIEWERCONFIGURATION_H
#define MEDVIEWERCONFIGURATION_H

#include <QtCore>
#include "medViewContainer.h"
#include "medViewContainerCustom.h"

#include "medGuiExport.h"

class medToolBox;
class medViewerConfigurationPrivate;
class medViewContainerStack;

class MEDGUI_EXPORT medViewerConfiguration : public QObject
{
    Q_OBJECT

public:
    medViewerConfiguration(QWidget *parent=0);
    ~medViewerConfiguration(void);
    
    virtual QString description(void) const = 0;
    
    enum LayoutType {
        LeftDbRightTb,
        LeftTbRightDb,        
        TopDbBottomTb,
        TopTbBottomDb
    };


    QList<medToolBox*> toolBoxes(void) const;
    
    void setLayoutType(LayoutType type);
    LayoutType layoutType(void) const;
    
    void setViewLayoutType(int type);
    int viewLayoutType(void) const;
    
    void setCustomLayoutType(int value);
    int customLayoutType(void) const;
    
    void setDatabaseVisibility(bool);
    bool isDatabaseVisible(void) const;

    void setToolBoxesVisibility(bool);
    bool areToolBoxesVisible(void) const;

    void setLayoutToolBoxVisibility(bool visibility);
    void hideLayoutToolBox(void);
    void showLayoutToolBox(void);
    
    bool isLayoutToolBoxVisible() const;
    bool isLayoutToolBox(const medToolBox * toolbox);
    
    virtual void setupViewContainerStack(medViewContainerStack *container);

public slots:
    virtual void addToolBox(medToolBox *toolbox);
    virtual void removeToolBox(medToolBox *toolbox);
    
    virtual void patientChanged(int patientId);

signals:
    void toolboxAdded  (medToolBox *tb);
    void toolboxRemoved(medToolBox *tb);
    void layoutModeChanged (int mode);
    void layoutSplit(int, int);
    void layoutPresetClicked(int);
    
private:
    medViewerConfigurationPrivate *d;
};

#endif
