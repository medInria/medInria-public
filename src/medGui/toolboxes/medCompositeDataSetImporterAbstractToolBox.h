/* medCompositeDataSetImporterAbstractToolBox.h ---
 *
 * Author: Nicolas Toussaint
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */


#pragma once

#include "medToolBox.h"
#include "medGuiExport.h"

class medCompositeDataSetImporterSelectorToolBox;
class medCompositeDataSetImporterAbstractToolBoxPrivate;
class dtkAbstractProcess;
class dtkAbstractData;

class MEDGUI_EXPORT medCompositeDataSetImporterAbstractToolBox : public medToolBox
{
    Q_OBJECT
public:
             medCompositeDataSetImporterAbstractToolBox(QWidget *parent = 0);
    virtual ~medCompositeDataSetImporterAbstractToolBox(void);
    virtual void setCompositeDataSetImporterToolBox(medCompositeDataSetImporterSelectorToolBox *toolbox);

    virtual QString description (void) const = 0;

    virtual dtkAbstractData *output (void) const;

public slots:
    virtual bool import() = 0;
    virtual void reset()  {};
    virtual void load() {};

protected:
    medCompositeDataSetImporterSelectorToolBox *parent(void);

protected:
    medCompositeDataSetImporterAbstractToolBoxPrivate *d;
};


