#ifndef MEDVIEWERWORKSPACEFACTORY_H
#define MEDVIEWERWORKSPACEFACTORY_H

#include "medGuiExport.h"

#include <dtkCore/dtkAbstractFactory.h>

#include <QtCore>

class medViewerWorkspace;
class medViewerWorkspaceFactoryPrivate;
struct medViewerWorkspaceDetails;

class MEDGUI_EXPORT medViewerWorkspaceFactory : public dtkAbstractFactory
{
    Q_OBJECT

public:
    typedef medViewerWorkspace *(*medViewerWorkspaceCreator)(QWidget* parent);
    typedef bool (*medViewerWorkspaceIsUsable)();

public:
    static medViewerWorkspaceFactory *instance(void);

    QList<QString> workspaces(void);

    /**
     * @brief Registers a medWorkspace type with the factory.
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
    bool registerWorkspace(QString identifier,
                         QString name,
                         QString description){
        //we must keep the templated part in the .h file for library users
        medViewerWorkspaceCreator creator = create<workspaceType>;
        return registerWorkspace(identifier,name,description,creator,workspaceType::isUsable);
    }

    /**
     * @brief Registers a medWorkspace type with the factory.
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
                         medViewerWorkspaceCreator creator,
                         medViewerWorkspaceIsUsable isUsable=NULL);

    /**
     * @brief Gives the details of all workspaces.
     *
     */
    QHash<QString, medViewerWorkspaceDetails *> workspaceDetails() const;

    /**
     * @brief Gives the details of one workspace.
     *
     */
    medViewerWorkspaceDetails * workspaceDetailsFromId(QString identifier) const;

    bool isUsable(QString identifier) const;

public slots:
    /**
     * @brief allocates the memory for a medViewerWorkspace.
     * @param type identifier for the Workspace type.
     * @param parent the parentWidget for all the Widget created in the workspace, even if the workspace is not a widget, its children can be destroyed by the qobject hierarchy.
     */
    medViewerWorkspace *createWorkspace(QString type,QWidget* parent=0);

protected:
     medViewerWorkspaceFactory(void);
    ~medViewerWorkspaceFactory(void);

private:
    static medViewerWorkspaceFactory *s_instance;
    /**
     * @brief Templated method returning a pointer to an allocated workspace.
     * @see template<class workspaceType> registerWorkspace
     * @warning keep it static if you don't want to freeze your brain (solution in http://www.parashift.com/c++-faq-lite/pointers-to-members.html#faq-33.5 for those interested)
     */
    template < typename T >
    static medViewerWorkspace* create ( QWidget* parent ) {
    return ( new T(parent) );
    }

private:
    medViewerWorkspaceFactoryPrivate *d;
};

/**
 * @brief stores the details for a particular workspace,
 * and a function to allocate memory.
 *
 */
struct MEDGUI_EXPORT medViewerWorkspaceDetails{
    QString name; /** Readable name*/
    QString description; /** (tooltip) short description of the workspace */
    medViewerWorkspaceFactory::medViewerWorkspaceCreator creator; /** function pointer allocating memory for the workspace*/
    medViewerWorkspaceFactory::medViewerWorkspaceIsUsable isUsable;
    medViewerWorkspaceDetails(QString name,QString description,medViewerWorkspaceFactory::medViewerWorkspaceCreator creator, medViewerWorkspaceFactory::medViewerWorkspaceIsUsable isUsable = NULL):
        name(name),description(description),creator(creator),isUsable(isUsable){}
};

#endif // MEDVIEWERWORKSPACEFACTORY_H
