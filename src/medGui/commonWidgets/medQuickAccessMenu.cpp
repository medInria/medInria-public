#include "medQuickAccessMenu.h"
#include <medViewerWorkspaceFactory.h>

medQuickAccessMenu::medQuickAccessMenu ( bool vertical, QWidget* parent, Qt::WindowFlags f ) : QWidget ( parent, f )
{
    currentSelected = 0;
    if (vertical)
        this->createVerticalQuickAccessMenu();
    else
    {
        this->createHorizontalQuickAccessMenu();
        this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        this->setAttribute(Qt::WA_TranslucentBackground, true);
    }
}

void medQuickAccessMenu::focusOutEvent ( QFocusEvent* event )
{
    QWidget::focusOutEvent ( event );
    emit hideMenu();
}

void medQuickAccessMenu::keyPressEvent ( QKeyEvent * event )
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
    
    QApplication::sendEvent(this->parentWidget(),event);
}

void medQuickAccessMenu::keyReleaseEvent ( QKeyEvent * event )
{
    QApplication::sendEvent(this->parentWidget(),event);
}

void medQuickAccessMenu::mouseMoveEvent (QMouseEvent *event)
{
    for (unsigned int i = 0;i < buttonsList.size();++i)
    {
        QRect widgetRect = buttonsList[i]->geometry();
        if (widgetRect.contains(event->pos()))
        {
            this->onMouseMovedInWidget(i);
            break;
        }
    }
    
    QWidget::mouseMoveEvent(event);
}

void medQuickAccessMenu::switchToCurrentlySelected ( void )
{
    if (currentSelected < 0)
    {
        emit hideMenu();
        return;
    }
    
    if (currentSelected == 0)
    {
        emit switchToHomepageArea();
        return;
    }
    
    if (currentSelected == 1)
    {
        emit switchToBrowserArea();
        return;
    }

    if (currentSelected >= 2)
    {
        emit showWorkspace(buttonsList[currentSelected]->identifier());
        return;
    }
}

void medQuickAccessMenu::updateCurrentlySelectedLeft ( void )
{
    if (currentSelected <= 0)
    {
        if (currentSelected == 0)
        {
            buttonsList[currentSelected]->setSelected(false);
            buttonsList[currentSelected]->style()->unpolish(buttonsList[currentSelected]);
            buttonsList[currentSelected]->style()->polish(buttonsList[currentSelected]);
        }
        
        currentSelected = buttonsList.size() - 1;
        buttonsList[currentSelected]->setSelected(true);
        buttonsList[currentSelected]->style()->unpolish(buttonsList[currentSelected]);
        buttonsList[currentSelected]->style()->polish(buttonsList[currentSelected]);
        
        return;
    }
    
    buttonsList[currentSelected]->setSelected(false);
    buttonsList[currentSelected]->style()->unpolish(buttonsList[currentSelected]);
    buttonsList[currentSelected]->style()->polish(buttonsList[currentSelected]);
    currentSelected--;
    buttonsList[currentSelected]->setSelected(true);
    buttonsList[currentSelected]->style()->unpolish(buttonsList[currentSelected]);
    buttonsList[currentSelected]->style()->polish(buttonsList[currentSelected]);
}

void medQuickAccessMenu::updateCurrentlySelectedRight ( void )
{
    if (currentSelected < 0)
    {
        currentSelected = 0;
        buttonsList[currentSelected]->setSelected(true);
        buttonsList[currentSelected]->style()->unpolish(buttonsList[currentSelected]);
        buttonsList[currentSelected]->style()->polish(buttonsList[currentSelected]);        
        return;
    }

    buttonsList[currentSelected]->setSelected(false);
    buttonsList[currentSelected]->style()->unpolish(buttonsList[currentSelected]);
    buttonsList[currentSelected]->style()->polish(buttonsList[currentSelected]);

    currentSelected++;
    if (currentSelected >= buttonsList.size())
        currentSelected = 0;
    
    buttonsList[currentSelected]->setSelected(true);
    buttonsList[currentSelected]->style()->unpolish(buttonsList[currentSelected]);
    buttonsList[currentSelected]->style()->polish(buttonsList[currentSelected]);
}

void medQuickAccessMenu::reset(bool optimizeLayout)
{
    for (unsigned int i = 0;i < buttonsList.size();++i)
    {
        if (currentSelected == i)
            buttonsList[i]->setSelected(true);
        else
            buttonsList[i]->setSelected(false);
        buttonsList[i]->style()->unpolish(buttonsList[i]);
        buttonsList[i]->style()->polish(buttonsList[i]);
    }
    
    if ((optimizeLayout)&&(backgroundFrame))
    {
        unsigned int width = ((QWidget *)this->parent())->size().width();
        unsigned int numberOfWidgetsPerLine = floor((width - 40.0) / 180.0);
        unsigned int optimalSizeLayout = ceil((float)buttonsList.size() / numberOfWidgetsPerLine);
        //numberOfWidgetsPerLine = ceil((float)buttonsList.size() / optimalSizeLayout);
        
        if (backgroundFrame->layout())
            delete backgroundFrame->layout();
        
        if (optimalSizeLayout > 1)
        {
            QGridLayout *layout = new QGridLayout;
            unsigned int totalAdded = 0;
            for (unsigned int i = 0;i < optimalSizeLayout;++i)
            {
                for (unsigned int j = 0;j < numberOfWidgetsPerLine;++j)
                {
                    layout->addWidget(buttonsList[totalAdded],i,j);
                    ++totalAdded;
                    
                    if (totalAdded == buttonsList.size())
                        break;
                }
                
                if (totalAdded == buttonsList.size())
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
            unsigned int totalAdded = 0;
            for (unsigned int i = 0;i < buttonsList.size();++i)
                layout->addWidget(buttonsList[i]);
            
            backgroundFrame->setLayout(layout);
            backgroundFrame->setFixedWidth ( 40 + 180 * buttonsList.size() );
            backgroundFrame->setFixedHeight ( 130 );
            
            this->setFixedWidth ( 40 + 180 * buttonsList.size() );
            this->setFixedHeight ( 130 );
        }
    }
}

void medQuickAccessMenu::onMouseMovedInWidget(unsigned int identifier)
{
    unsigned int newSelection = identifier;
    
    if (newSelection == currentSelected)
        return;
    
    if (currentSelected >= 0)
    {
        buttonsList[currentSelected]->setSelected(false);
        buttonsList[currentSelected]->style()->unpolish(buttonsList[currentSelected]);
        buttonsList[currentSelected]->style()->polish(buttonsList[currentSelected]);
    }
    
    buttonsList[newSelection]->setSelected(true);
    buttonsList[newSelection]->style()->unpolish(buttonsList[newSelection]);
    buttonsList[newSelection]->style()->polish(buttonsList[newSelection]);
    
    currentSelected = newSelection;
}

void medQuickAccessMenu::createVerticalQuickAccessMenu ( void )
{
    buttonsList.clear();
    QHash<QString,medViewerWorkspaceDetails*> workspaceDetails = medViewerWorkspaceFactory::instance()->workspaceDetails();

    QVBoxLayout * workspaceButtonsLayout = new QVBoxLayout;
    workspaceButtonsLayout->setMargin(0);
    workspaceButtonsLayout->setSpacing ( 0 );
    
    //Setup quick access menu title
    QLabel * workspaceLabel = new QLabel ( tr("<b>Switch to workspaces</b>") );
    workspaceLabel->setMaximumWidth(300);
    workspaceLabel->setFixedHeight(25);
    workspaceLabel->setAlignment(Qt::AlignHCenter);
    workspaceLabel->setTextFormat(Qt::RichText);
    //It's more easy to set the stylesheet here than in the qss file
    workspaceLabel->setStyleSheet("border-image: url(:/pixmaps/toolbox-header.png) 16 16 0 16 repeat-x;\
                                  border-left-width: 0px;\
                                  border-right-width: 0px;\
                                  border-top-width: 8px;\
                                  border-bottom-width: 0px;");
    workspaceButtonsLayout->addWidget ( workspaceLabel );
    
    //Setup homepage access button
    medHomepagePushButton * homeButton = new medHomepagePushButton ( this );
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
    QObject::connect ( homeButton, SIGNAL ( clicked() ), this, SIGNAL ( switchToHomepageArea() ) );
    buttonsList.push_back(homeButton);
    
    //Setup browser access button
    medHomepagePushButton * browserButton = new medHomepagePushButton ( this );
    browserButton->setCursor(Qt::PointingHandCursor);
    browserButton->setStyleSheet("border: 0px;");
    browserButton->setIcon ( QIcon ( ":/icons/folder.png" ) );
    browserButton->setText ( "Browser" );
    browserButton->setFixedHeight ( 40 );
    browserButton->setMaximumWidth ( 250 );
    browserButton->setMinimumWidth ( 250 );
    browserButton->setIdentifier("Browser");
    browserButton->setFocusPolicy ( Qt::NoFocus );
    workspaceButtonsLayout->addWidget ( browserButton );
    QObject::connect ( browserButton, SIGNAL ( clicked() ),this, SIGNAL ( switchToBrowserArea()) );
    buttonsList.push_back(browserButton);
    
    //Dynamically setup workspaces access button
    foreach ( QString id, workspaceDetails.keys() )
    {
        medHomepagePushButton * button = new medHomepagePushButton ( this );
        medViewerWorkspaceDetails* detail = workspaceDetails.value(id);
        button->setText ( detail->name );
        button->setFocusPolicy ( Qt::NoFocus );
        button->setCursor(Qt::PointingHandCursor);
        button->setStyleSheet("border: 0px;");
        button->setFixedHeight ( 40 );
        button->setMaximumWidth ( 250 );
        button->setMinimumWidth ( 250 );
        button->setToolTip( detail->description);
        button->setIdentifier(id);
        workspaceButtonsLayout->addWidget ( button );
        QObject::connect ( button, SIGNAL ( clicked ( QString ) ),this, SIGNAL ( showWorkspace ( QString ) ) );
        buttonsList.push_back(button);
    }
    workspaceButtonsLayout->addStretch();
    this->setMinimumHeight ( 20 + 40 * ( 2 + workspaceDetails.size() ) );
    this->setLayout(workspaceButtonsLayout);
}

void medQuickAccessMenu::createHorizontalQuickAccessMenu ( void )
{
    buttonsList.clear();
    QHash<QString,medViewerWorkspaceDetails*> workspaceDetails = medViewerWorkspaceFactory::instance()->workspaceDetails();

    backgroundFrame = new QFrame(this);
    backgroundFrame->setStyleSheet("border-radius: 10px;background-color: rgba(200,200,200,150);");
    QHBoxLayout *mainWidgetLayout = new QHBoxLayout;
    mainWidgetLayout->setMargin(0);
    mainWidgetLayout->setSpacing ( 0 );
    
    QHBoxLayout * shortcutAccessLayout = new QHBoxLayout;
    
    medHomepagePushButton * smallHomeButton = new medHomepagePushButton ( this );
    smallHomeButton->setFixedHeight ( 100 );
    smallHomeButton->setFixedWidth ( 160 );
    smallHomeButton->setStyleSheet("border-radius: 5px;font-size:12px;background-image: url(:icons/home_sc.png) no-repeat;");
    smallHomeButton->setFocusPolicy ( Qt::NoFocus );
    smallHomeButton->setCursor(Qt::PointingHandCursor);
    smallHomeButton->setIdentifier("Homepage");
    smallHomeButton->setText("Homepage");
    smallHomeButton->setSelected(true);
    shortcutAccessLayout->addWidget ( smallHomeButton );
    QObject::connect ( smallHomeButton, SIGNAL ( clicked() ), this, SIGNAL ( switchToHomepageArea() ) );
    buttonsList.push_back(smallHomeButton);
    
    //Setup browser access button
    medHomepagePushButton * smallBrowserButton = new medHomepagePushButton ( this );
    smallBrowserButton->setCursor(Qt::PointingHandCursor);
    smallBrowserButton->setStyleSheet("border-radius: 5px;font-size:12px;background-image: url(:icons/database.png) no-repeat;");
    smallBrowserButton->setFixedHeight ( 100 );
    smallBrowserButton->setFixedWidth ( 160 );
    smallBrowserButton->setFocusPolicy ( Qt::NoFocus );
    smallBrowserButton->setText("Browser");
    smallBrowserButton->setIdentifier("Browser");
    shortcutAccessLayout->addWidget ( smallBrowserButton );
    QObject::connect ( smallBrowserButton, SIGNAL ( clicked() ),this, SIGNAL ( switchToBrowserArea()) );
    buttonsList.push_back(smallBrowserButton);
    
    foreach ( QString id, workspaceDetails.keys() )
    {
        medHomepagePushButton * button = new medHomepagePushButton ( this );
        medViewerWorkspaceDetails* detail = workspaceDetails.value(id);
        button->setText ( detail->name );
        button->setFocusPolicy ( Qt::NoFocus );
        button->setCursor(Qt::PointingHandCursor);
        button->setFixedHeight ( 100 );
        button->setFixedWidth ( 160 );
        button->setStyleSheet("border-radius: 5px;font-size:12px;background-image: url(:icons/workspace_" + detail->name + ".png) no-repeat;");
        button->setIdentifier(id);
        shortcutAccessLayout->addWidget ( button );
        QObject::connect ( button, SIGNAL ( clicked ( QString ) ),this, SIGNAL ( showWorkspace ( QString ) ) );
        buttonsList.push_back(button);
    }
    
    backgroundFrame->setLayout(shortcutAccessLayout);
    backgroundFrame->setFixedWidth ( 40 + 180 * ( 2 + workspaceDetails.size() ) );
    backgroundFrame->setFixedHeight ( 130 );
    mainWidgetLayout->addWidget(backgroundFrame);
    this->setFixedWidth ( 40 + 180 * ( 2 + workspaceDetails.size() ) );
    this->setFixedHeight ( 130 );
    this->setLayout(mainWidgetLayout);
}
