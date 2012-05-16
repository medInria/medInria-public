#ifndef MEDTOOLBOXCUSTOM_H
#define MEDTOOLBOXCUSTOM_H

#include "medGuiExport.h"

#include <medToolBox.h>

class medToolBoxCustomPrivate;


/**
 * @brief A toolbox that helps an other more generic toolbox.
 *
 *
 */
class MEDGUI_EXPORT medToolBoxCustom : public medToolBox
{
public:

    medToolBoxCustom(QWidget* parentWidget = 0);
    /**
     * @note considering setting this method the only constructor.
     */
    medToolBoxCustom(medToolBox* parentToolbox, QWidget* parentWidget = 0);
    ~medToolBoxCustom();

    /**
     * @brief
     *
     */
    virtual void setParentToolBox(medToolBox * toolbox);


    /**
     * @brief Returns the plugin this custom Toolbox comes from.
     *
     * Used to fill the aboutPlugin button in the parent toolbox, if needed.
     *
     * @note: not Sure I want to keep this method pure abstract, may not be
     * needed by everyone implementation.
     */
    virtual dtkPlugin *plugin( void )const = 0;

    /**
     *
     */
    static bool registered(void);

    /**
     * @note Forces the addition of a description, to be displayed in comboBoxes
     * and others.
     */
    virtual QString description (void) const = 0;
protected:
    medToolBox *parentToolBox(void);

    medToolBoxCustomPrivate *d;
};

#endif // MEDTOOLBOXCUSTOM_H
