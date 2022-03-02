#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.

 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.

=========================================================================*/

#include <QDomDocument>
#include <QDomElement>
#include <QtGui>
#include <QtWidgets>

#include <medAbstractWorkspaceLegacy.h>
#include <medCoreLegacyExport.h>
#include <medJobItemL.h>
#include <medProgressionStack.h>

class dtkAbstractView;
class medAbstractData;

class medToolBoxTab;
class medToolBoxPrivate;
class medToolBoxBody;
class medToolBoxHeader;
class dtkPlugin;

/**
 * @brief Toolbox that includes a title bar and a widget container.
 *
 * The toolboxes are displayed in the medToolBoxContainer.
 * They can be minimized, hidden, updated when a view is selected, etc...
 *
*/
class MEDCORELEGACY_EXPORT medToolBox : public QWidget
{
    Q_OBJECT

public:
    medToolBox(QWidget *parent = nullptr);
    virtual ~medToolBox();

    void addWidget(QWidget *widget);
    void removeWidget(QWidget *widget);
    void setTabWidget (medToolBoxTab* tab);
    void setTitle(const QString& title);
    void setTitleOffset(const QPoint & titleOffset);

    medToolBoxHeader *header() const;
    medToolBoxBody   *body()   const;

    void setAboutPluginVisibility(bool enable);
    bool aboutPluginVisibility();

    void setAboutPluginButton(dtkPlugin * plugin);

    virtual QString identifier() const {return QString();}
    virtual QString name() const {return QString();}
    virtual QString description() const {return QString();}
    virtual QStringList categories() const {return QStringList();}

    virtual void setWorkspace(medAbstractWorkspaceLegacy* workspace);
    medAbstractWorkspaceLegacy* getWorkspace();

    //! Display a qDebug and a medMessageController
    void displayMessageError(QString error);

    //! Get back progress bar from workspace
    medProgressionStack *getProgressionStack();

    //! enable or disable the output automatic import after a process success
    void enableOnProcessSuccessImportOutput(medJobItemL *job, bool enable);

    virtual void toXMLNode(QDomDocument *doc, QDomElement *currentNode);

    virtual QVariant getValue(QString name) const;
    virtual bool setValue(QString name, QVariant value);

    template<class VALUE_TYPE>
    VALUE_TYPE getValue(QString name) const
    {
        return getValue(name).value<VALUE_TYPE>();
    }

    template<class VALUE_TYPE>
    bool setValue(QString name, VALUE_TYPE value)
    {
        return setValue(name, QVariant::fromValue<VALUE_TYPE>(value));
    }

    virtual QObject* getComponent(QString name);

    template<class TYPE>
    TYPE getComponent(QString name)
    {
        return dynamic_cast<TYPE>(getComponent(name));
    }

signals:
    /**
     * @brief Emitted when an action from the toolbox succeeded.
     * Typically used when a dtkProcess returned.
     *
    */
    void success();

    /**
     * @brief Emitted when an action from the toolbox failed.
     *
     * Typically used when a dtkProcess returned.
    */
    void failure();

public slots:
    virtual void clear(){}
    void switchMinimize();
    void setValidDataTypes(const QStringList & dataTypes);
    const QStringList ValidDataTypes();
    void addValidDataType(const QString & dataType);
    void show();

    //! Switch between errors
    void handleDisplayError(int);

    //! Deactivate toolbox
    void setToolBoxOnWaitStatus();

    //! Deactivate toolbox in function non using medRunnableProcess
    void setToolBoxOnWaitStatusForNonRunnableProcess();

    //! Enable toolbox
    void setToolBoxOnReadyToUse();

    //! Add default connection and start a process
    void addConnectionsAndStartJob(medJobItemL *job);

    //! Default connections between a toolbox and a process (success, failure, etc)
    void addToolBoxConnections(medJobItemL *job);

    virtual void updateView(){}

protected slots:
    void onAboutButtonClicked();

protected:
    template<class COMPONENT_TYPE, class VALUE_TYPE>
    bool getComponentValue(QObject* component, VALUE_TYPE* value) const;

    template<class COMPONENT_TYPE, class VALUE_TYPE>
    bool setComponentValue(QObject* component, VALUE_TYPE value);

private:
    medToolBoxPrivate *d;
};

#define MED_TOOLBOX_INTERFACE(_name,_desc,_categories) \
public:\
    static QString staticIdentifier() {return QString(staticMetaObject.className());}\
    static QString staticName() {return QString::fromUtf8(_name);}\
    static QString staticDescription() {return QString::fromUtf8(_desc);}\
    static QStringList staticCategories() {return QStringList() _categories;}\
    virtual QString identifier() const {return staticIdentifier();}\
    virtual QString name() const {return staticName();}\
    virtual QString description() const {return staticDescription();}\
    virtual QStringList categories() const {return staticCategories();}

