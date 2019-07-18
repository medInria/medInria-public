/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medCoreLegacyExport.h"

#include <dtkCoreSupport/dtkPlugin.h>

class MEDCORELEGACY_EXPORT medPluginLegacy : public dtkPlugin
{
    Q_OBJECT

public:
    medPluginLegacy(QObject* parent = 0);

    /**
     * @brief name() fills the header widget in the "i" information pop-up.
     *        Is also used to link a toolbox with a plugin in its plugin() function
     * @return the name of the plugin
     */
    virtual QString name() const = 0;

    /**
     * @brief contact() fills the Contact widget in the "i" information pop-up
     * @return a contact mail adress/phone number/etc.
     */
    virtual QString contact() const;

    /**
     * @brief description() fills the Description widget in the "i" information pop-up
     * @return a description of the plugin
     */
    virtual QString description() const;

    /**
     * @brief authors() fills the Authors widget in the "i" information pop-up
     * @return a list of authors of the plugin
     */
    virtual QStringList authors() const;

    /**
     * @brief contributors() fills the Contributors widget in the "i" information pop-up
     * @return a list of contributors of the plugin
     */
    virtual QStringList contributors() const;

    /**
     * @brief dependencies() fills the Dependencies widget in the "i" information pop-up
     * @return a list of external dependencies needed by the plugin
     */
    virtual QStringList dependencies() const;

    /**
     * @brief tags() is used to add tags to plugins
     * @return a list of tags linked to the plugin
     */
    virtual QStringList tags() const;

    /**
     * @brief <not used>
     * @return <not used>
     */
    virtual bool uninitialize();

    /**
     * @brief identifier() is used to identify the current plugin with a uniq QString
     * @return the identifier of the current plugin
     */
    virtual QString identifier() const;

    /**
     * @brief version() fills the Version widget in the "i" information pop-up
     * @return the version number set in the plugin CMakeLists file
     */
    virtual QString version() const;

    /**
     * @brief types() is used to define the processes/toolboxes types in the plugin
     * @return a list of types defined in the plugin
     */
    virtual QStringList types() const;
};


