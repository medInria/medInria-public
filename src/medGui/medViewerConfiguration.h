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

class medToolBox;
class medViewerConfigurationPrivate;

class medViewerConfiguration : public QObject
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

    void addToolBox(medToolBox *toolbox);
    void removeToolBox(medToolBox *toolbox);
    QList<medToolBox*> toolBoxes(void) const;
    
    void setLayoutType(LayoutType type);
    int  layoutType(void) const;
    
    void setViewLayoutType(int type);
    int  viewLayoutType(void) const;
    
    void setCustomLayoutType(int value);
    int customLayoutType(void) const;
    
    void hideDatabase(void);
    void showDatabase(void);
    int databaseVisibility(void) const;
    
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
