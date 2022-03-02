#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractFactory.h>

#include <QtCore>

#include <medCoreLegacyExport.h>

class medAbstractWorkspaceLegacy;
class medWorkspaceFactoryPrivate;

class MEDCORELEGACY_EXPORT medWorkspaceFactory : public dtkAbstractFactory
{
    Q_OBJECT

public:
    typedef medAbstractWorkspaceLegacy *(*medWorkspaceCreator)(QWidget* parent, void* argument);
    typedef bool (*medWorkspaceIsUsable)();

    struct Details
    {
        QString identifier;
        QString name;
        QString description;
        QString category;
        medWorkspaceCreator creator;
        void* creatorArgument;
        medWorkspaceIsUsable isUsable;
        bool isActive;
        Details(QString id_,
                QString name_,
                QString description_,
                QString category_,
                medWorkspaceCreator creator_,
                void* creatorArgument_,
                medWorkspaceIsUsable isUsable_ = nullptr,
                bool isActive_ = true)
            : identifier(id_)
            , name(name_)
            , description(description_)
            , category(category_)
            , creator(creator_)
            , creatorArgument(creatorArgument_)
            , isUsable(isUsable_)
            , isActive(isActive_)
        {}

    };

public:
    static medWorkspaceFactory* instance();

    QList<QString> workspaces();

    /**
     * @brief Registers a medAbstractWorkspaceLegacy type with the factory.
     *
     *
     * This method is templated with the workspaceType.
     * This is a convience method.
     * The constructor of the workspace - with a (QWidget *parent) signature -
     * is used to create a function pointer to allocate memory.
     * The workspace source code doesn't need to contain any such
     * function itself.
     *
     * @param identifier Identifier of the type.
     * @param name Human readable name (Potentially localised).
     * @param description short description (Potentially localised).
     */
    template <typename workspaceType>
    bool registerWorkspace(bool isActive = true)
    {
        //we must keep the templated part in the .h file for library users
        medWorkspaceCreator creator = create<workspaceType>;
        return registerWorkspace(workspaceType::staticIdentifier(),
                                 workspaceType::staticName(),
                                 workspaceType::staticDescription(),
                                 workspaceType::staticCategory(),
                                 creator,
                                 nullptr,
                                 workspaceType::isUsable,
                                 isActive);
    }

    /**
     * @brief Registers a medAbstractWorkspaceLegacy type with the factory.
     *
     * This method requires the developer to provide his own function pointer
     * to allocate the workspace memory.
     *
     * @param identifier Identifier of the type.
     * @param name Human readable name (Potentially localised).
     * @param description short description, mainly used for tooltips
     * (Potentially localised).
     * @param creator function pointer allocating memory for the toolbox.
     */
    bool registerWorkspace(QString identifier,
                           QString name,
                           QString description,
                           QString category,
                           medWorkspaceCreator creator,
                           void* creatorArgument = nullptr,
                           medWorkspaceIsUsable isUsable = nullptr, bool isActive = true);


    QHash<QString, medWorkspaceFactory::Details *> workspaceDetails() const;
    QList<medWorkspaceFactory::Details *> workspaceDetailsSortedByName(bool activeOnly = false) const;
    medWorkspaceFactory::Details * workspaceDetailsFromId(QString identifier) const;

    bool isUsable(QString identifier) const;

    QList<QString> workspacesFromCategory(const QString& category) const;

public slots:

    medAbstractWorkspaceLegacy* createWorkspace(QString type,QWidget* parent=0);

protected:

     medWorkspaceFactory();
    virtual ~medWorkspaceFactory();

private:
    static medWorkspaceFactory *s_instance;
    /**
     * @brief Templated method returning a pointer to an allocated workspace.
     * @see template<class workspaceType> registerWorkspace
     * @warning keep it static if you don't want to freeze your brain (solution in http://www.parashift.com/c++-faq-lite/pointers-to-members.html#faq-33.5 for those interested)
     */
    template < typename T >
    static medAbstractWorkspaceLegacy* create(QWidget* parent, void* argument) {
    return ( new T(parent) );
    }

private:
    medWorkspaceFactoryPrivate *d;
};
