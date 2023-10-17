/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medQuickAccessMenu.h>
#include <medWorkspaceFactory.h>
#include <medSettingsManager.h>

#ifdef Q_OS_MAC
Qt::Key OSIndependentControlKey = Qt::Key_Meta;
#else
Qt::Key OSIndependentControlKey = Qt::Key_Control;
#endif

int retrieveDefaultWorkSpace()
{
    int homepageDefaultWorkspaceNumber = 1;
    int iRes = homepageDefaultWorkspaceNumber;

    bool bMatch = false;
    medWorkspaceFactory::Details *poDetail = nullptr;
    QList<medWorkspaceFactory::Details*> oListOfWorkspaceDetails = medWorkspaceFactory::instance()->workspaceDetailsSortedByName(true);
    QVariant oStartupWorkspace = medSettingsManager::instance().value("startup", "default_starting_area");

    if (oStartupWorkspace.toString() == "Search")
    {
        iRes = homepageDefaultWorkspaceNumber; // "Search" should not be the startup area. Default is Homepage.
    }
    if (oStartupWorkspace.toString() == "Homepage")
    {
        iRes = homepageDefaultWorkspaceNumber;
    }
    else if (oStartupWorkspace.toString() == "Browser")
    {
        iRes = 2;
    }
    else if (oStartupWorkspace.toString() == "Composer")
    {
        iRes = 3;
    }
    else
    {
        for (int i = 0; i < oListOfWorkspaceDetails.size() && !bMatch; ++i)
        {
            poDetail = oListOfWorkspaceDetails[i];
            bMatch = poDetail->name == oStartupWorkspace.toString();
            if (bMatch)
            {
                iRes = i+4;
            }
        }
    }


    return iRes;
}

/**
 * Constructor, parameter vertical chooses if the layout will be vertical (bottom left menu) or horizontal (alt-tab like menu)
 */
medQuickAccessMenu::medQuickAccessMenu(bool vertical, QWidget *parent, Qt::WindowFlags f) : QWidget(parent, f)
{
    currentSelected = retrieveDefaultWorkSpace();

    if (vertical)
        this->createVerticalQuickAccessMenu();
    else
    {
        this->createHorizontalQuickAccessMenu();
        this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        this->setAttribute(Qt::WA_TranslucentBackground, true);
    }
}

/**
 *  emit the menuHidden() signal when the widget lost the focus
 */
void medQuickAccessMenu::focusOutEvent ( QFocusEvent *event )
{
    QWidget::focusOutEvent ( event );
    emit menuHidden();
}

/**
 * Handles key press events to control selected widget in menu
 */
void medQuickAccessMenu::keyPressEvent ( QKeyEvent *event )
{
    if ((event->key() == Qt::Key_Down)||(event->key() == Qt::Key_Right))
    {
        this->updateCurrentlySelectedRight();
        return;
    }

    if ((event->key() == Qt::Key_Up)||(event->key() == Qt::Key_Left))
    {
        this->updateCurrentlySelectedLeft();
        return;
    }

    if (event->key() == Qt::Key_Return)
    {
        this->switchToCurrentlySelected();
        return;
    }

    if (event->key() == Qt::Key_Escape)
    {
        emit menuHidden();
        return;
    }

    QApplication::sendEvent(this->parentWidget(), event);
}

/**
 * Send back key release event to parent widget that handles key releases
 */
void medQuickAccessMenu::keyReleaseEvent ( QKeyEvent *event )
{
    if (event->key() == OSIndependentControlKey && this->isVisible())
    {
        emit menuHidden();
        switchToCurrentlySelected();
        return;
    }

    if (event->key() == Qt::Key_Space && event->modifiers().testFlag(Qt::ShiftModifier))
    {
        updateCurrentlySelectedLeft();
        return;
    }

    QApplication::sendEvent(this->parentWidget(), event);
}

/**
 * Mouse over implementation to select widget in menu
 */
void medQuickAccessMenu::mouseMoveEvent (QMouseEvent *event)
{
    for (int i = 0; i < buttonsList.size(); ++i)
    {
        QRect widgetRect = buttonsList[i]->geometry();
        if (widgetRect.contains(event->pos()))
        {
            this->mouseSelectWidget(i);
            break;
        }
    }

    QWidget::mouseMoveEvent(event);
}

/**
 * Updates the currently selected item when current workspace was changed from somewhere else
 */
void medQuickAccessMenu::updateSelected (QString workspace)
{
    currentSelected = 0;

    for (int i = 0; i < buttonsList.size(); ++i)
    {
        if (buttonsList[i]->identifier() == workspace)
        {
            currentSelected = i;
            break;
        }
    }
}

/**
 * Actually emits signal to switch to selected workspace
 */
void medQuickAccessMenu::switchToCurrentlySelected()
{
    if (currentSelected < 0)
    {
        emit menuHidden();
        return;
    }

    if (currentSelected == 0)
    {
        emit searchSelected();
        return;
    }

    if (currentSelected == 1)
    {
        emit homepageSelected();
        return;
    }

    if (currentSelected == 2)
    {
        emit browserSelected();
        return;
    }

    if (currentSelected == 3)
    {
        emit composerSelected();
        return;
    }

    if (currentSelected >= 4)
    {
        emit workspaceSelected(buttonsList[currentSelected]->identifier());
        return;
    }
}

/**
 * Move selected widget one position to the left
 */
void medQuickAccessMenu::updateCurrentlySelectedLeft()
{
    if (currentSelected <= 0)
    {
        if (currentSelected == 0)
            buttonsList[currentSelected]->setSelected(false);

        currentSelected = buttonsList.size() - 1;
        buttonsList[currentSelected]->setSelected(true);

        return;
    }

    buttonsList[currentSelected]->setSelected(false);
    currentSelected--;
    buttonsList[currentSelected]->setSelected(true);
}

/**
 * Move selected widget one position to the right
 */
void medQuickAccessMenu::updateCurrentlySelectedRight()
{
    if (currentSelected < 0)
    {
        currentSelected = 0;
        buttonsList[currentSelected]->setSelected(true);
        return;
    }

    buttonsList[currentSelected]->setSelected(false);

    currentSelected++;
    if (currentSelected >= buttonsList.size())
        currentSelected = 0;

    buttonsList[currentSelected]->setSelected(true);
}

/**
 * Resets the menu when shown, optionally updates the layout to window size
 */
void medQuickAccessMenu::reset(bool optimizeLayout)
{
    for (int i = 0; i < buttonsList.size(); ++i)
    {
        if (currentSelected == i)
            buttonsList[i]->setSelected(true);
        else
            buttonsList[i]->setSelected(false);
    }

    if ((optimizeLayout)&&(backgroundFrame))
    {
        unsigned int width = ((QWidget*)this->parent())->size().width();
        unsigned int numberOfWidgetsPerLine = floor((width - 40.0) / 180.0);
        unsigned int optimalSizeLayout = ceil((float)buttonsList.size() / numberOfWidgetsPerLine);

        if (backgroundFrame->layout())
            delete backgroundFrame->layout();

        if (optimalSizeLayout > 1)
        {
            QGridLayout *layout = new QGridLayout;
            unsigned int totalAdded = 0;
            for (unsigned int i = 0; i < optimalSizeLayout; ++i)
            {
                for (unsigned int j = 0; j < numberOfWidgetsPerLine; ++j)
                {
                    layout->addWidget(buttonsList[totalAdded], i, j);
                    ++totalAdded;

                    if (totalAdded == (unsigned int)buttonsList.size())
                        break;
                }

                if (totalAdded == (unsigned int)buttonsList.size())
                    break;
            }

            backgroundFrame->setLayout(layout);
            backgroundFrame->setFixedWidth ( 40 + 180 * numberOfWidgetsPerLine );
            backgroundFrame->setFixedHeight ( 130 * optimalSizeLayout );
            this->setFixedWidth ( 40 + 180 * numberOfWidgetsPerLine );
            this->setFixedHeight ( 130 * optimalSizeLayout );
        }
        else
        {
            QHBoxLayout *layout = new QHBoxLayout;
            for (int i = 0; i < buttonsList.size(); ++i)
            {
                layout->addWidget(buttonsList[i]);
            }

            backgroundFrame->setLayout(layout);
            backgroundFrame->setFixedWidth ( 40 + 180 * buttonsList.size() );
            backgroundFrame->setFixedHeight ( 130 );

            this->setFixedWidth ( 40 + 180 * buttonsList.size() );
            this->setFixedHeight ( 130 );
        }
    }
}

/**
 * Mouse over slot to select widget in menu
 */
void medQuickAccessMenu::mouseSelectWidget(unsigned int identifier)
{
    unsigned int newSelection = identifier;

    if (newSelection == (unsigned int)currentSelected)
        return;

    if (currentSelected >= 0)
        buttonsList[currentSelected]->setSelected(false);

    buttonsList[newSelection]->setSelected(true);
    currentSelected = newSelection;
}

/**
 * Vertical menu layout creation method
 */
void medQuickAccessMenu::createVerticalQuickAccessMenu()
{
    buttonsList.clear();

    QVBoxLayout *workspaceButtonsLayout = new QVBoxLayout;
    workspaceButtonsLayout->setMargin(0);
    workspaceButtonsLayout->setSpacing ( 0 );

    //Setup quick access menu title for toolbox tools
    QLabel *toolboxeSearchLabel = new QLabel ( tr("<b>Search a toolbox</b>") );
    toolboxeSearchLabel->setMaximumWidth(300);
    toolboxeSearchLabel->setFixedHeight(25);
    toolboxeSearchLabel->setAlignment(Qt::AlignCenter);
    toolboxeSearchLabel->setTextFormat(Qt::RichText);
    toolboxeSearchLabel->setObjectName("quickAccessMenuHeader"); // See qss file, dedicated format
    workspaceButtonsLayout->addWidget(toolboxeSearchLabel);

    //Find a toolbox
    medHomepagePushButton *searchButton = new medHomepagePushButton ( this );
    searchButton->setText("Search");
    searchButton->setIdentifier("Search");
    searchButton->setIcon(QIcon(":icons/magnifier.png"));
    searchButton->setFixedHeight(40);
    searchButton->setMaximumWidth(250);
    searchButton->setMinimumWidth(250);
    searchButton->setStyleSheet("border: 0px;");
    searchButton->setFocusPolicy(Qt::NoFocus);
    searchButton->setCursor(Qt::PointingHandCursor);
    workspaceButtonsLayout->addWidget(searchButton);
    QObject::connect ( searchButton, SIGNAL ( clicked() ), this, SIGNAL ( searchSelected() ) );
    buttonsList.push_back(searchButton);

    //Setup quick access menu title for workspace
    QLabel *workspaceLabel = new QLabel ( tr("<b>Switch to workspaces</b>") );
    workspaceLabel->setMaximumWidth(300);
    workspaceLabel->setFixedHeight(25);
    workspaceLabel->setAlignment(Qt::AlignCenter);
    workspaceLabel->setTextFormat(Qt::RichText);
    workspaceLabel->setObjectName("quickAccessMenuHeader");
    workspaceButtonsLayout->addWidget(workspaceLabel);

    //Setup homepage access button
    medHomepagePushButton *homeButton = new medHomepagePushButton ( this );
    homeButton->setText("Home");
    homeButton->setIdentifier("Homepage");
    homeButton->setIcon ( QIcon ( ":icons/home.png" ) );
    homeButton->setFixedHeight ( 40 );
    homeButton->setMaximumWidth ( 250 );
    homeButton->setMinimumWidth ( 250 );
    homeButton->setStyleSheet("border: 0px;");
    homeButton->setFocusPolicy ( Qt::NoFocus );
    homeButton->setCursor(Qt::PointingHandCursor);
    workspaceButtonsLayout->addWidget ( homeButton );
    QObject::connect ( homeButton, SIGNAL ( clicked() ), this, SIGNAL ( homepageSelected() ) );
    buttonsList.push_back(homeButton);

    //Setup browser access button
    medHomepagePushButton *browserButton = new medHomepagePushButton(this);
    browserButton->setCursor(Qt::PointingHandCursor);
    browserButton->setStyleSheet("border: 0px;");
    browserButton->setIcon(QIcon(":/icons/folder.png"));
    browserButton->setText("Browser");
    browserButton->setFixedHeight(40);
    browserButton->setMaximumWidth(250);
    browserButton->setMinimumWidth(250);
    browserButton->setIdentifier("Browser");
    browserButton->setFocusPolicy(Qt::NoFocus);
    workspaceButtonsLayout->addWidget(browserButton);
    QObject::connect(browserButton, SIGNAL(clicked()), this, SIGNAL(browserSelected()));
    buttonsList.push_back(browserButton);

    //Setup composer access button
    medHomepagePushButton *composerButton = new medHomepagePushButton(this);
    composerButton->setCursor(Qt::PointingHandCursor);
    composerButton->setStyleSheet("border: 0px;");
    composerButton->setIcon(QIcon(":/icons/composer.png"));
    composerButton->setText("Composer");
    composerButton->setFixedHeight(40);
    composerButton->setMaximumWidth(250);
    composerButton->setMinimumWidth(250);
    composerButton->setIdentifier("Composer");
    composerButton->setFocusPolicy(Qt::NoFocus);
    workspaceButtonsLayout->addWidget(composerButton);
    QObject::connect(composerButton, SIGNAL(clicked()), this, SIGNAL(composerSelected()));
    buttonsList.push_back(composerButton);

    //Dynamically setup workspaces access button
    QList<medWorkspaceFactory::Details*> workspaceDetails = medWorkspaceFactory::instance()->workspaceDetailsSortedByName();
    unsigned int numActiveWorkspaces = 0;
    for( medWorkspaceFactory::Details* detail : workspaceDetails )
    {
        if (!detail->isActive)
            continue;

        ++numActiveWorkspaces;
        medHomepagePushButton *button = new medHomepagePushButton ( this );
        button->setText ( detail->name );
        button->setFocusPolicy ( Qt::NoFocus );
        button->setCursor(Qt::PointingHandCursor);
        button->setStyleSheet("border: 0px;");
        button->setFixedHeight ( 40 );
        button->setMaximumWidth ( 250 );
        button->setMinimumWidth ( 250 );
        button->setToolTip( detail->description);
        button->setIdentifier(detail->identifier );
        workspaceButtonsLayout->addWidget ( button );
        QObject::connect ( button, SIGNAL ( clicked ( QString ) ), this, SIGNAL ( workspaceSelected ( QString ) ) );
        buttonsList.push_back(button);
    }
    workspaceButtonsLayout->addStretch();
    this->setMinimumHeight ( 20 + 40 * ( 3 + numActiveWorkspaces ) );
    this->setLayout(workspaceButtonsLayout);
}

/**
 * Horizontal menu layout creation method
 */
void medQuickAccessMenu::createHorizontalQuickAccessMenu()
{
    buttonsList.clear();

    backgroundFrame = new QFrame(this);
    backgroundFrame->setStyleSheet("border-radius: 10px;background-color: rgba(200,200,200,150);");
    QHBoxLayout *mainWidgetLayout = new QHBoxLayout;
    mainWidgetLayout->setMargin(0);
    mainWidgetLayout->setSpacing ( 0 );

    QHBoxLayout *shortcutAccessLayout = new QHBoxLayout;

    medHomepagePushButton *smallHomeButton = new medHomepagePushButton ( this );
    smallHomeButton->setFixedHeight ( 100 );
    smallHomeButton->setFixedWidth ( 160 );
    smallHomeButton->setStyleSheet("border-radius: 5px;font-size:12px;color: #ffffff;background-image: url(:icons/home_sc.png) no-repeat;");
    smallHomeButton->setFocusPolicy ( Qt::NoFocus );
    smallHomeButton->setCursor(Qt::PointingHandCursor);
    smallHomeButton->setIdentifier("Homepage");
    smallHomeButton->setText("Homepage");
    smallHomeButton->setSelected(true);
    shortcutAccessLayout->addWidget (smallHomeButton);
    QObject::connect ( smallHomeButton, SIGNAL ( clicked() ), this, SIGNAL ( homepageSelected() ) );
    buttonsList.push_back(smallHomeButton);

    //Setup browser access button
    medHomepagePushButton *smallBrowserButton = new medHomepagePushButton ( this );
    smallBrowserButton->setCursor(Qt::PointingHandCursor);
    smallBrowserButton->setStyleSheet("border-radius: 5px;font-size:12px;color: #ffffff;background-image: url(:icons/database.png) no-repeat;");
    smallBrowserButton->setFixedHeight ( 100 );
    smallBrowserButton->setFixedWidth ( 160 );
    smallBrowserButton->setFocusPolicy ( Qt::NoFocus );
    smallBrowserButton->setText("Browser");
    smallBrowserButton->setIdentifier("Browser");
    shortcutAccessLayout->addWidget ( smallBrowserButton );
    QObject::connect ( smallBrowserButton, SIGNAL ( clicked() ), this, SIGNAL ( browserSelected()) );
    buttonsList.push_back(smallBrowserButton);

    //Setup composer access button
    medHomepagePushButton *smallComposerButton = new medHomepagePushButton(this);
    smallComposerButton->setCursor(Qt::PointingHandCursor);
    smallComposerButton->setStyleSheet("border-radius: 5px;font-size:12px;color: #ffffff;background-image: url(:icons/composer_sc.png) no-repeat;");
    smallComposerButton->setFixedHeight(100);
    smallComposerButton->setFixedWidth(160);
    smallComposerButton->setFocusPolicy(Qt::NoFocus);
    smallComposerButton->setText("Composer");
    smallComposerButton->setIdentifier("Composer");
    shortcutAccessLayout->addWidget(smallComposerButton);
    QObject::connect(smallComposerButton, SIGNAL(clicked()), this, SIGNAL(composerSelected()));
    buttonsList.push_back(smallComposerButton);

    QList<medWorkspaceFactory::Details*> workspaceDetails = medWorkspaceFactory::instance()->workspaceDetailsSortedByName();
    unsigned int numActiveWorkspaces = 0;
    for( medWorkspaceFactory::Details* detail : workspaceDetails )
    {
        if (!detail->isActive)
            continue;

        ++numActiveWorkspaces;
        medHomepagePushButton *button = new medHomepagePushButton ( this );
        button->setText ( detail->name );
        button->setFocusPolicy ( Qt::NoFocus );
        button->setCursor(Qt::PointingHandCursor);
        button->setFixedHeight ( 100 );
        button->setFixedWidth ( 160 );
        button->setStyleSheet("border-radius: 5px;font-size:12px;color: #ffffff;background-image: url(:icons/workspace_" + detail->name + ".png) no-repeat;");
        button->setIdentifier(detail->identifier);
        shortcutAccessLayout->addWidget ( button );
        QObject::connect ( button, SIGNAL ( clicked ( QString ) ), this, SIGNAL ( workspaceSelected ( QString ) ) );
        buttonsList.push_back(button);
    }

    backgroundFrame->setLayout(shortcutAccessLayout);
    backgroundFrame->setFixedWidth ( 40 + 180 * ( 2 + numActiveWorkspaces ) );
    backgroundFrame->setFixedHeight ( 240 );
    backgroundFrame->setMouseTracking(true);
    mainWidgetLayout->addWidget(backgroundFrame);
    this->setFixedWidth ( 40 + 180 * ( 2 + numActiveWorkspaces ) );
    this->setFixedHeight ( 240 );
    this->setLayout(mainWidgetLayout);
}

void medQuickAccessMenu::manuallyClickOnWorkspaceButton(QString workspaceName)
{
    for (int i = 0;i < buttonsList.size();++i)
    {
        if (buttonsList[i]->text() == workspaceName)
        {
            buttonsList[i]->click();
            break;
        }
    }
}
