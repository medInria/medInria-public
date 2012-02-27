/* medToolBoxCompositeDataSetImporterCustom.h ---
 *
 * Author: Nicolas Toussaint
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */


#ifndef MEDTOOLBOXCOMPOSITEDATASETIMPORTERCUSTOM_H
#define MEDTOOLBOXCOMPOSITEDATASETIMPORTERCUSTOM_H

#include "medToolBox.h"
#include "medGuiExport.h"

class medToolBoxCompositeDataSetImporter;
class medToolBoxCompositeDataSetImporterCustomPrivate;
class dtkAbstractProcess;
class dtkAbstractData;

class MEDGUI_EXPORT medToolBoxCompositeDataSetImporterCustom : public medToolBox
{
    Q_OBJECT
public:
             medToolBoxCompositeDataSetImporterCustom(QWidget *parent = 0);
    virtual ~medToolBoxCompositeDataSetImporterCustom(void);
    virtual void setCompositeDataSetImporterToolBox(medToolBoxCompositeDataSetImporter *toolbox);

    virtual QString description (void) const = 0;

    virtual dtkAbstractData *output (void) const;

public slots:    
    virtual bool import() = 0;
    virtual void reset()  {};
    virtual void load() {};
    
protected:
    medToolBoxCompositeDataSetImporter *parent(void);

protected:
    medToolBoxCompositeDataSetImporterCustomPrivate *d;
};

#endif // MEDTOOLBOXCOMPOSITEDATASETIMPORTERCUSTOM_H
