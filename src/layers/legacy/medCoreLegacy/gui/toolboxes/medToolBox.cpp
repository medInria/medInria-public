/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractData.h>
#include <medAbstractProcessLegacy.h>
#include <medAbstractView.h>
#include <medAbstractWorkspaceLegacy.h>
#include <medButton.h>
#include <medJobManagerL.h>
#include <medMessageController.h>
#include <medToolBox.h>
#include <medToolBoxHeader.h>
#include <medToolBoxBody.h>
#include <medToolBoxTab.h>

#include <dtkCoreSupport/dtkGlobal.h>
#include <dtkCoreSupport/dtkPlugin>
#include <dtkGuiSupport/dtkAboutPlugin.h>

class medToolBoxPrivate
{
public:
    medToolBoxHeader *header;
    medToolBoxBody *body;
    bool isMinimized;

    QStringList validDataTypes;
    bool isContextVisible;
    bool aboutPluginVisibility;
    dtkPlugin* plugin;
    medAbstractWorkspaceLegacy *workspace;

public:
    QVBoxLayout *layout;
};

medToolBox::medToolBox(QWidget *parent) : QWidget(parent), d(new medToolBoxPrivate)
{
    d->workspace = nullptr;

    d->header = new medToolBoxHeader(this);
    d->body = new medToolBoxBody(this);
    d->isContextVisible = false;
    d->aboutPluginVisibility = false;
    d->plugin = nullptr;


    d->layout = new QVBoxLayout(this);
    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(0);
    d->layout->addWidget(d->header);
    d->layout->addWidget(d->body);
    d->isMinimized = false;
    connect(d->header,SIGNAL(triggered()),this,SLOT(switchMinimize()));

    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

medToolBox::~medToolBox(void)
{
    delete d;
    d = nullptr;
}

/**
 * @brief Adds a medToolBoxTab to the toolbox.
 *
 * @param tab
*/
void medToolBox::setTabWidget(medToolBoxTab *tab)
{
    d->body->setTabWidget(tab);
}

/**
 * @brief Adds a widget to the toolbox.
 * This widget will be horizontally or vertically added to the layout given the toolbox's orientation.
 *
 * @param widget
*/
void medToolBox::addWidget(QWidget *widget)
{
    d->body->addWidget(widget);
}

/**
 * @brief Removes a widget from the toolbox.
 *
 * @param widget
*/
void medToolBox::removeWidget(QWidget *widget)
{
    d->body->removeWidget(widget);
}

/**
 * @brief Sets the toolbox's title.
 *
 * @param title
*/
void medToolBox::setTitle(const QString &title)
{
    d->header->setTitle(title);
}

/**
 * @brief Sets the offset of the header's title from upper left corner.
 *
 * @param titleOffset
 */
void medToolBox::setTitleOffset(const QPoint & titleOffset)
{
    d->header->setTitleOffset(titleOffset);
}

/**
 * @brief Gets the Toolbox's header.
 *
 * @return medToolBoxHeader *
*/
medToolBoxHeader *medToolBox::header(void) const
{
    return d->header;
}

/**
 * @brief Gets the toolbox's body.
 *
 * @return medToolBoxBody *
*/
medToolBoxBody *medToolBox::body(void) const
{
    return d->body;
}

/**
 * @brief Clears the toolbox.
 *
 * Resets the parameters within the tolbox,
 * for instance when the current patient changed or the view.
 *
*/
void medToolBox::clear(void)
{
    d->body->clear();
}

/**
 * @brief Switches from a minimized state to an extended one and vice versa.
 *
*/
void medToolBox::switchMinimize()
{
    //isMinimized before switch == wanted body visibility
    d->body->setVisible(d->isMinimized);
    d->isMinimized = !d->isMinimized;
}

/**
 * @brief Show the toolbox, "overloads" the QWidget show
 *
*/
void medToolBox::show()
{
    //JGG dtkDebug()<<"show TB:" << header()->title();
    if(d->validDataTypes.isEmpty() || d->isContextVisible)
    {
        //JGG dtkDebug()<<"actually showing in TB";
        QWidget::show();
    }
}

/**
 * @brief Set toolbox's valid data types.
 *
 * If at least one datatype in the view is contained within the
 * toolBoxValidTypes the toolbox will be visible. If the list is empty,
 * then the toolbox will be context free, and always visible whatever the
 * focused view.
 *
 * @param dataTypes Valid data types of the toolbox.
*/
void medToolBox::setValidDataTypes(const QStringList & dataTypes)
{
    d->validDataTypes = QStringList(dataTypes);

}

/**
 * @brief Get toolbox's valid data types.
 *
 * @see setValidDataTypes
*/
const QStringList medToolBox::ValidDataTypes()
{
   return  d->validDataTypes;

}

/**
 * @brief Add a valid data type to the toolbox's valid data types.
 *
 * @param dataType
*/
void medToolBox::addValidDataType(const QString & dataType)
{
    if (!d->validDataTypes.contains(dataType))
    {
        d->validDataTypes.append(dataType);
    }
}

/**
 * @brief Enables or disable the aboutPlugin button.
 *
 * @param enable true to display the button.
 */
void medToolBox::setAboutPluginVisibility(bool enable)
{
    d->aboutPluginVisibility = enable;
    d->header->setAboutButtonVisibility(enable);
}

/**
 * @brief Returns the aboutPlugin button visibility.
 *
 */
bool medToolBox::aboutPluginVisibility()
{
    return d->aboutPluginVisibility;
}

/**
 * @brief Sets up the plugin this button is refering to.
 *
 * @param plugin The dtkPlugin this button will give info about.
 */
void medToolBox::setAboutPluginButton(dtkPlugin *plugin)
{
    medButton* aboutButton = d->header->aboutButton();
    if (aboutButton)
    {
        aboutButton->disconnect();
        connect(aboutButton,SIGNAL(triggered()),this,SLOT(onAboutButtonClicked()));
        d->plugin = plugin;
    }
    else
    {
        dtkWarn() << "no aboutButton found for toolbox" << d->header->title();
    }
}

/**
 * @brief Raises a dtkAboutPlugin dialog.
 *
 */
void medToolBox::onAboutButtonClicked()
{
    if(d->plugin)
    {
        dtkDebug() << "about plugin" << d->plugin->name();

        QDialog * dial = new QDialog(this);
        QString windowTitle = tr("About ");
        windowTitle += d->plugin->name();
        dial->setWindowTitle(windowTitle);
        dtkAboutPlugin * apWidget = new dtkAboutPlugin(d->plugin,dial);

        QVBoxLayout * layout = new QVBoxLayout(dial);

        QPushButton * okBut = new QPushButton(dial);
        okBut->setText("Ok");
        okBut->setFocusPolicy(Qt::NoFocus);
        QObject::connect(okBut, SIGNAL(clicked()), dial, SLOT(close()));

        QHBoxLayout * butLayout = new QHBoxLayout;
        butLayout->addStretch();
        butLayout->addWidget(okBut);
        butLayout->addStretch();

        layout->addWidget(apWidget);
        layout->addLayout(butLayout);

        dial->setLayout(layout);
        dial->exec();
    }
    else
    {
        dtkWarn() << "No plugin set for toolbox" << d->header->title();
    }
}

medAbstractWorkspaceLegacy* medToolBox::getWorkspace()
{
    return d->workspace;
}

void medToolBox::setWorkspace(medAbstractWorkspaceLegacy *workspace)
{
    d->workspace = workspace;
}

void medToolBox::displayMessageError(QString error)
{
    qDebug() << qPrintable(name() + ": " + error);
    medMessageController::instance()->showError(error, 3000);
}

void medToolBox::handleDisplayError(int error)
{
    // Handle volume(s)/mesh(es) error
    switch (error)
    {
    case medAbstractProcessLegacy::PIXEL_TYPE:
        displayMessageError("Pixel type not yet implemented");
        break;
    case medAbstractProcessLegacy::DIMENSION_3D:
        displayMessageError("This toolbox is designed to be used with 3D volumes");
        break;
    case medAbstractProcessLegacy::DIMENSION_4D:
        displayMessageError("This toolbox is designed to be used with 4D volumes");
        break;
    case medAbstractProcessLegacy::MESH_TYPE:
        displayMessageError("This toolbox is designed to be used with meshes");
        break;
    case medAbstractProcessLegacy::MESH_3D:
        displayMessageError("This toolbox is designed to be used with 3D meshes");
        break;
    case medAbstractProcessLegacy::MESH_4D:
        displayMessageError("This toolbox is designed to be used with 4D meshes");
        break;
    case medAbstractProcessLegacy::NO_MESH:
        displayMessageError("This toolbox is not designed to be used with meshes");
        break;
    case medAbstractProcessLegacy::DATA_SIZE:
        displayMessageError("Inputs must be the same size");
        break;
    case medAbstractProcessLegacy::MISMATCHED_DATA_TYPES:
        displayMessageError("Inputs must be the same type");
        break;
    case medAbstractProcessLegacy::MISMATCHED_DATA_SIZES_ORIGIN_SPACING:
        displayMessageError("Inputs must have the same size, origin, spacing");
        break;
    case medAbstractProcessLegacy::MISMATCHED_DATA_SIZE:
        displayMessageError("Inputs must have the same size");
        break;
    case medAbstractProcessLegacy::MISMATCHED_DATA_ORIGIN:
        displayMessageError("Inputs must have the same origin");
        break;
    case medAbstractProcessLegacy::MISMATCHED_DATA_SPACING:
        displayMessageError("Inputs must have the same spacing");
        break;
    default:
        displayMessageError("This action failed (undefined error)");
        break;
    }
}

void medToolBox::setToolBoxOnWaitStatus()
{
    this->setDisabled(true);
}

void medToolBox::setToolBoxOnWaitStatusForNonRunnableProcess()
{
    setToolBoxOnWaitStatus();

    // Needed for non-medRunnableProcess functions
    this->repaint();
    QApplication::processEvents();
}

void medToolBox::setToolBoxOnReadyToUse()
{
    this->setDisabled(false);
}

medProgressionStack* medToolBox::getProgressionStack()
{
    return getWorkspace()->getProgressionStack();
}

void medToolBox::addConnectionsAndStartJob(medJobItemL *job)
{
    addToolBoxConnections(job);

    getProgressionStack()->addJobItem(job, "Progress "+this->name()+":");

    medJobManagerL::instance()->registerJobItem(job);
    QThreadPool::globalInstance()->start(dynamic_cast<QRunnable*>(job));
}

void medToolBox::addToolBoxConnections(medJobItemL *job)
{
    connect (job, SIGNAL (success   (QObject*)),    this, SIGNAL (success ()));
    connect (job, SIGNAL (failure   (QObject*)),    this, SIGNAL (failure ()));
    connect (job, SIGNAL (cancelled (QObject*)),    this, SIGNAL (failure ()));
    connect (job, SIGNAL (cancelled (QObject*)),    this, SLOT   (setToolBoxOnReadyToUse()));
    connect (job, SIGNAL (success   (QObject*)),    this, SLOT   (setToolBoxOnReadyToUse()));
    connect (job, SIGNAL (failure   (QObject*)),    this, SLOT   (setToolBoxOnReadyToUse()));
    connect (job, SIGNAL (failure   (int)),         this, SLOT   (handleDisplayError(int)));
    connect (job, SIGNAL (activate(QObject*, bool)), getProgressionStack(), SLOT(setActive(QObject*,bool)));
}
