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


#ifndef medCompositeDataSetImporterAbstractToolBox_H
#define medCompositeDataSetImporterAbstractToolBox_H

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
    virtual ~medCompositeDataSetImporterAbstractToolBox();
    virtual void setCompositeDataSetImporterToolBox(medCompositeDataSetImporterSelectorToolBox *toolbox);

    virtual QString description() const = 0;

    virtual dtkAbstractData *output() const;

public slots:
    virtual bool import() = 0;
    virtual void reset()  {};
    virtual void load() {};

protected:
    medCompositeDataSetImporterSelectorToolBox *parent();

protected:
    medCompositeDataSetImporterAbstractToolBoxPrivate *d;
};

#endif // medCompositeDataSetImporterAbstractToolBox_H
