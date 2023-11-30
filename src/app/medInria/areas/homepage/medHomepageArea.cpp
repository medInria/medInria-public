/*
 * medInria
 * Copyright (c) INRIA 2013. All rights reserved.
 * 
 * medInria is under BSD-2-Clause license. See LICENSE.txt for details in the root of the sources or:
 * https://github.com/medInria/medInria-public/blob/master/LICENSE.txt
 * 
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <medHomepageArea.h>

// #include <medDatabaseSettingsWidget.h>
#include <medHomepageButton.h>
#include <medMainWindow.h>
#include <medPluginWidget.h>
#include <medStartupSettingsWidget.h>
#include <medWorkspaceFactory.h>

#include<medApplicationContext.h>

class medHomepageAreaPrivate
{
public:
    QWidget *navigationWidget;
    QWidget *descriptionWidget;
    QLabel *applicationLabel;
    QTextEdit *textEdit;
};

medHomepageArea::medHomepageArea ( QWidget * parent ) : QWidget ( parent ), d ( new medHomepageAreaPrivate )
{
    // Setup the description widget: application logo and description
    d->descriptionWidget = new QWidget(this);
    d->descriptionWidget->setProperty("pos", QPoint(10, this->height()/7));

    QHBoxLayout *descriptionLayout = new QHBoxLayout(d->descriptionWidget);

    d->applicationLabel = new QLabel(this);
    QPixmap applicationLogo( ":pixmaps/medInria-logo-homepage.png" );
    d->applicationLabel->setPixmap(applicationLogo);
    descriptionLayout->addWidget(d->applicationLabel);
    descriptionLayout->addStretch();

    d->textEdit = new QTextEdit(this);
    QFile descriptionFile(":DESCRIPTION.txt");
    descriptionFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream descriptionStream(&descriptionFile);
    descriptionStream.setCodec("UTF-8");
    d->textEdit->setHtml(descriptionStream.readAll());
    d->textEdit->setReadOnly(true);
    d->textEdit->setFocusPolicy(Qt::NoFocus);
    d->textEdit->setMinimumWidth(500);
    d->textEdit->setMinimumHeight(150);
    d->textEdit->setMaximumHeight(150);
    descriptionLayout->addWidget(d->textEdit);

    // Setup the navigation widget with buttons to access workspaces
    d->navigationWidget = new QWidget(this);
    d->navigationWidget->setProperty("pos", QPoint(20, this->height()/4));
}

medHomepageArea::~medHomepageArea()
{
    delete d;
    d = nullptr;
}

void medHomepageArea::resizeEvent ( QResizeEvent * event )
{
    Q_UNUSED(event);

    // Recompute the widget position when the window is resized
    d->navigationWidget->setProperty("pos", QPoint(20, height()/4));

    // The description text is resized when the window is resized:
    // Total width of the app, minus the logo and the spacings
    int newTextSize = width() - d->applicationLabel->pixmap()->width() - 40;
    d->textEdit->setMinimumWidth(newTextSize);
    d->textEdit->setMaximumWidth(newTextSize);
    d->descriptionWidget->resize(width() - 30, d->applicationLabel->pixmap()->height());
}

void medHomepageArea::initPage()
{
    //Initialization of the navigation widget with available workspaces
    QList<medWorkspaceFactory::Details*> workspaceDetails = medWorkspaceFactory::instance()->workspaceDetailsSortedByName(true);

    //--- Basic grid
    int spacingBetweenHeaderAndColumn = 10;
    int spacingBetweenColumnButtons = 10;

    QVBoxLayout * workspaceButtonsLayoutBasic = new QVBoxLayout;
    workspaceButtonsLayoutBasic->setSpacing(spacingBetweenHeaderAndColumn);
    QGridLayout * workspaceButtonsLayoutBasicGrid = new QGridLayout;
    workspaceButtonsLayoutBasicGrid->setSpacing(spacingBetweenColumnButtons);
    QLabel * workspaceLabelBasic = new QLabel ( "<b>Basic Area</b>" );
    workspaceLabelBasic->setTextFormat(Qt::RichText);
    workspaceLabelBasic->setAlignment(Qt::AlignLeft);
    workspaceButtonsLayoutBasic->addWidget(workspaceLabelBasic);
    workspaceButtonsLayoutBasic->addLayout(workspaceButtonsLayoutBasicGrid);

    medHomepageButton * browserButton = new medHomepageButton ( this );
    browserButton->setToolButtonStyle ( Qt::ToolButtonTextUnderIcon );
    browserButton->setIcon ( QIcon ( ":/icons/folder.png" ) );
    browserButton->setText ( "Import/export files" );
    browserButton->setMinimumHeight ( 40 );
    browserButton->setMaximumWidth ( 250 );
    browserButton->setMinimumWidth ( 250 );
    browserButton->setFocusPolicy ( Qt::NoFocus );
    browserButton->setToolTip("Area to manage and import data");
    workspaceButtonsLayoutBasicGrid->addWidget(browserButton, 0, 0);
    QObject::connect ( browserButton, SIGNAL ( clicked() ),this, SLOT ( onShowBrowser() ) );

    //--- Workspace grids

    QVBoxLayout * workspaceButtonsLayoutMethodology = new QVBoxLayout;
    workspaceButtonsLayoutMethodology->setSpacing(spacingBetweenHeaderAndColumn);
    QGridLayout * workspaceButtonsLayoutMethodologyGrid = new QGridLayout;
    workspaceButtonsLayoutMethodologyGrid->setSpacing(spacingBetweenColumnButtons);
    QLabel * workspaceLabelMethodology = new QLabel ( "<b>Methodology</b>" );
    workspaceLabelMethodology->setTextFormat(Qt::RichText);
    workspaceLabelMethodology->setAlignment(Qt::AlignLeft);
    workspaceButtonsLayoutMethodology->addWidget(workspaceLabelMethodology);
    workspaceButtonsLayoutMethodology->addLayout(workspaceButtonsLayoutMethodologyGrid);

    QVBoxLayout * workspaceButtonsLayoutClinical = new QVBoxLayout;
    workspaceButtonsLayoutClinical->setSpacing(spacingBetweenHeaderAndColumn);
    QGridLayout * workspaceButtonsLayoutClinicalGrid = new QGridLayout;
    workspaceButtonsLayoutClinicalGrid->setSpacing(spacingBetweenColumnButtons);
    QLabel * workspaceLabelClinical = new QLabel ( "<b>Clinical</b>" );
    workspaceLabelClinical->setTextFormat(Qt::RichText);
    workspaceLabelClinical->setAlignment(Qt::AlignLeft);
    workspaceButtonsLayoutClinical->addWidget(workspaceLabelClinical);
    workspaceButtonsLayoutClinical->addLayout(workspaceButtonsLayoutClinicalGrid);

    QVBoxLayout * workspaceButtonsLayoutOther = new QVBoxLayout;
    workspaceButtonsLayoutOther->setSpacing(spacingBetweenHeaderAndColumn);
    QGridLayout * workspaceButtonsLayoutOtherGrid = new QGridLayout;
    workspaceButtonsLayoutOtherGrid->setSpacing(spacingBetweenColumnButtons);
    QLabel * workspaceLabelOther = new QLabel ( "<b>Other</b>" );
    workspaceLabelOther->setTextFormat(Qt::RichText);
    workspaceLabelOther->setAlignment(Qt::AlignLeft);
    workspaceButtonsLayoutOther->addWidget(workspaceLabelOther);
    workspaceButtonsLayoutOther->addLayout(workspaceButtonsLayoutOtherGrid);

    // If there are too many buttons in a category (more than maximumButtonsPerColumn), split them in several columns
    int maximumButtonsPerColumn = 7;

    for( medWorkspaceFactory::Details* detail : workspaceDetails)
    {
        medHomepageButton * button = new medHomepageButton ( this );
        button->setText ( detail->name );
        button->setFocusPolicy ( Qt::NoFocus );
        button->setMinimumHeight ( 40 );
        button->setMaximumWidth ( 250 );
        button->setMinimumWidth ( 250 );
        button->setToolTip(detail->description);
        button->setIdentifier(detail->identifier);
        QObject::connect ( button, SIGNAL ( clicked ( QString ) ),this, SLOT ( onShowWorkspace ( QString ) ) );
        if (!(medWorkspaceFactory::instance()->isUsable(detail->identifier)))
        {
            button->setDisabled(true);
            button->setToolTip("No useful plugin has been found for this workspace.");
        }
        if(!detail->category.compare("Basic")) 
        {
            // Workspaces as "Visualization"
            int row    = workspaceButtonsLayoutBasicGrid->count() % maximumButtonsPerColumn;
            int column = std::ceil(workspaceButtonsLayoutBasicGrid->count() / maximumButtonsPerColumn);
            workspaceButtonsLayoutBasicGrid->addWidget(button, row, column);
        }
        else if(!detail->category.compare("Methodology")) 
        {
            int row    = workspaceButtonsLayoutMethodologyGrid->count() % maximumButtonsPerColumn;
            int column = std::ceil(workspaceButtonsLayoutMethodologyGrid->count() / maximumButtonsPerColumn);
            workspaceButtonsLayoutMethodologyGrid->addWidget(button, row, column);
        }
        else if(!detail->category.compare("Clinical")) 
        {
            int row    = workspaceButtonsLayoutClinicalGrid->count() % maximumButtonsPerColumn;
            int column = std::ceil(workspaceButtonsLayoutClinicalGrid->count() / maximumButtonsPerColumn);
            workspaceButtonsLayoutClinicalGrid->addWidget(button, row, column);
        }
        else
        {
            int row    = workspaceButtonsLayoutOtherGrid->count() % maximumButtonsPerColumn;
            int column = std::ceil(workspaceButtonsLayoutOtherGrid->count() / maximumButtonsPerColumn);
            workspaceButtonsLayoutOtherGrid->addWidget(button, row, column);
        }
    }
    workspaceButtonsLayoutBasic->addStretch();
    workspaceButtonsLayoutMethodology->addStretch();
    workspaceButtonsLayoutClinical->addStretch();
    workspaceButtonsLayoutOther->addStretch();

    QGridLayout* workspaceButtonsLayout = new QGridLayout;
    workspaceButtonsLayout->setSpacing(40); // Spacing between categories

    // Hide the empty categories 
    std::vector<QLayout*> oLayoutVect;
    if (workspaceButtonsLayoutBasicGrid->count() > 0)
    {
        oLayoutVect.push_back(workspaceButtonsLayoutBasic);
    }
    if (workspaceButtonsLayoutMethodologyGrid->count() > 0)
    {
        oLayoutVect.push_back(workspaceButtonsLayoutMethodology);
    }
    if (workspaceButtonsLayoutClinicalGrid->count() > 0)
    {
        oLayoutVect.push_back(workspaceButtonsLayoutClinical);
    }
    if (workspaceButtonsLayoutOtherGrid->count() > 0)
    {
        oLayoutVect.push_back(workspaceButtonsLayoutOther);
    }
    for (int i = 0; i < static_cast<int>(oLayoutVect.size()); ++i)
    {
        workspaceButtonsLayout->addLayout(oLayoutVect[i], 0, i);
    }

    d->navigationWidget->setLayout(workspaceButtonsLayout);
}



void medHomepageArea::onShowBrowser()
{
    emit showBrowser();
}
 
void medHomepageArea::onShowWorkspace(QString workspace)
{
    emit showWorkspace(workspace);
}
