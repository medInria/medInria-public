#include <iostream>
#include <string>
#include <regex>
#include <chrono>
#include <thread>
#include <QtWidgets>
#include "medBoutiquesSearchToolsWidget.h"

#define BOUTIQUES_CACHE_PATH "/.cache/boutiques"
#define DATABASE_NAME "all-descriptors.json"

medBoutiquesSearchToolsWidget::medBoutiquesSearchToolsWidget(QWidget *parent) : QWidget(parent), toolDatabaseUpdated(false), ignorePPrintError(false)
{
    // Create the search input
    this->searchLineEdit = new QLineEdit();
    this->searchLineEdit->setPlaceholderText("Search a tool in Boutiques...");
    this->button = new QPushButton("Search");

    this->searchGroupBox = new QGroupBox();
    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchLayout->addWidget(this->searchLineEdit);
    searchLayout->addWidget(this->button);
    this->searchGroupBox->setLayout(searchLayout);

    this->loadingLabel = new QLabel("Loading...");
    this->loadingLabel->hide();

    // Create the tool model and the search view
    this->createSearchView();

    // Create the info text area for the tool description
    this->infoLabel = new QLabel("Tool info");
    this->infoLabel->hide();
    this->infoShortDescription = new QLabel("");
    this->infoShortDescription->hide();
    this->infoShortDescription->setWordWrap(true);
    this->info = new QTextEdit();
    this->info->setMinimumHeight(300);
    this->info->setReadOnly(true);
    this->info->hide();
    this->moreInfoButton = new QPushButton("More info");
    this->moreInfoButton->hide();

    // Create the layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(this->searchGroupBox);
    layout->addWidget(this->loadingLabel);
    layout->addWidget(this->searchView);
    layout->addWidget(this->infoLabel);
    layout->addWidget(this->infoShortDescription);
    layout->addWidget(this->info);
    layout->addWidget(this->moreInfoButton);
    this->setLayout(layout);

    // Connect signals
    connect(this->button, &QPushButton::clicked, this, &medBoutiquesSearchToolsWidget::searchBoutiquesTools);
    connect(this->searchLineEdit, &QLineEdit::textChanged, this, &medBoutiquesSearchToolsWidget::searchChanged);
    connect(this->searchLineEdit, &QLineEdit::returnPressed, this, &medBoutiquesSearchToolsWidget::searchBoutiquesTools);
    connect(this->moreInfoButton, &QPushButton::clicked, this, &medBoutiquesSearchToolsWidget::toggleMoreInfo);

    // Create search, pprint and loadToolDatabase processes
    this->createProcesses();
    // Load tool database (if any)
    this->loadToolDatabase();

    // Download the entire tool database from zenodo once at the startup (search and pull the 1000 first boutiques tools on zenodo),
    // so that the database (the file "~/.cache/boutiques/all-descriptors.json") remains up-to-date.
    // This is useful to keep BoutiquesGUI working when the zenodo database is down (we can then rely on the data stored in "all-descriptors.json"),
    // and to get instantaneous search result.
    // This process is executed on the background, and updates the database
    // once all descriptors are pulled from Zenodo.
    this->downloadToolDatabase();
}

ToolDescription *medBoutiquesSearchToolsWidget::getSelectedTool()
{
    // Get selected tool from the search view index (which must be converted from the proxy model index to the tool model index)
    // return the selected tool description, or nullptr if no result correponds to the index
    int currentRow = this->proxyToolModel->mapToSource(this->searchView->currentIndex()).row();
    int currentToolIndex = this->toolModel->index(currentRow, 0).data(Qt::UserRole).toInt();
    return currentToolIndex >= 0 && currentToolIndex < int(this->searchResults.size()) ? &this->searchResults[static_cast<unsigned int>(currentToolIndex)] : nullptr;
}

const QJsonObject medBoutiquesSearchToolsWidget::getSelectedToolDescriptor()
{
    ToolDescription *searchResult = this->getSelectedTool();

    // If the tool is in the database ("~/.cache/boutiques/all-descriptors.json"): return it
    if(this->descriptors.contains(searchResult->id))
    {
        return this->descriptors[searchResult->id].toObject();
    }

    // Otherwise: return the "~/.cache/boutiques/zenodo-TOOL_ID.json" file (if openable, otherwise return an empty QJsonObject)
    return this->getToolDescriptor(searchResult->id);
}

const QJsonObject medBoutiquesSearchToolsWidget::getToolDescriptor(const QString &toolId)
{
    QString id = QString::fromStdString(toolId.toStdString()); // deep copy the string to modify it

    QString descriptorFileName = id.replace(QChar('.'), QChar('-')) + ".json";
    QDir cacheDirectory(QDir::homePath() + "/.cache/boutiques");

    QFile file(cacheDirectory.absoluteFilePath(descriptorFileName));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Could not open descriptor file", "Error while opening descriptor file (" + descriptorFileName + ") from ~" + BOUTIQUES_CACHE_PATH);
        return QJsonObject();
    }

    QJsonDocument jsonDocument(QJsonDocument::fromJson(file.readAll()));

    return jsonDocument.object();
}

void medBoutiquesSearchToolsWidget::createSearchView()
{
    // Create the proxy tool model
    this->proxyToolModel = new QSortFilterProxyModel();
    this->proxyToolModel->setFilterKeyColumn(-1);
    this->proxyToolModel->setSortCaseSensitivity(Qt::CaseInsensitive);

    // Create the search view
    this->searchView = new QTreeView();
    this->searchView->setRootIsDecorated(false);
    this->searchView->setAlternatingRowColors(true);
    this->searchView->setSortingEnabled(true);
    this->searchView->setMinimumHeight(300);
    this->searchView->setUniformRowHeights(true);

    // Create the tool model
    this->toolModel = new QStandardItemModel(0, 4, this);
    this->toolModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Id"));
    this->toolModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Title"));
    this->toolModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Description"));
    this->toolModel->setHeaderData(3, Qt::Horizontal, QObject::tr("Downloads"));

    this->proxyToolModel->setSourceModel(this->toolModel);
    this->searchView->setModel(this->proxyToolModel);

    // Create the view headers
    QHeaderView *viewHeader = this->searchView->header();
    viewHeader->setDefaultSectionSize(100);
    viewHeader->setSectionResizeMode(0, QHeaderView::Interactive);
    viewHeader->setSectionResizeMode(1, QHeaderView::Interactive);
    viewHeader->setSectionResizeMode(2, QHeaderView::Stretch);
    viewHeader->setSectionResizeMode(3, QHeaderView::Fixed);
    viewHeader->setStretchLastSection(false);

    this->searchView->setColumnWidth(0, 50);
    this->searchView->setColumnWidth(1, 250);
    this->searchView->setColumnWidth(3, 80);

    this->searchView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(this->searchView->selectionModel(), &QItemSelectionModel::currentChanged, this, &medBoutiquesSearchToolsWidget::selectionChanged);
}

void medBoutiquesSearchToolsWidget::createProcesses()
{
    // Create the search process, pprint process and toolDatabaseProcess (to search all tools)
    this->searchProcess = new QProcess(this);
    connect(this->searchProcess, &QProcess::started, this, &medBoutiquesSearchToolsWidget::searchProcessStarted);
    connect(this->searchProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &medBoutiquesSearchToolsWidget::searchProcessFinished);
    connect(this->searchProcess, &QProcess::errorOccurred, this, &medBoutiquesSearchToolsWidget::errorOccurred);

    this->pprintProcess = new QProcess(this);
    connect(this->pprintProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &medBoutiquesSearchToolsWidget::pprintProcessFinished);
    connect(this->pprintProcess, &QProcess::errorOccurred, this, &medBoutiquesSearchToolsWidget::errorOccurred);

    this->toolDatabaseProcess = new QProcess(this);
}

void medBoutiquesSearchToolsWidget::downloadToolDatabase()
{
    // Search for the 1000 first boutiques tools in the Zenodo database
    connect(this->toolDatabaseProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &medBoutiquesSearchToolsWidget::createToolDatabase);

    this->toolDatabaseProcess->start(BoutiquesPaths::Python(), {BoutiquesPaths::Bosh(), "search", "-m 1000"});
}

void medBoutiquesSearchToolsWidget::searchChanged()
{
    // Filter the shown tools when the search field changes
    QRegExp::PatternSyntax syntax = QRegExp::PatternSyntax(QRegExp::RegExp);
    Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive;

    QRegExp regExp(this->searchLineEdit->text(), caseSensitivity, syntax);
    this->proxyToolModel->setFilterRegExp(regExp);
}

void medBoutiquesSearchToolsWidget::selectionChanged()
{
    // Display a short description when a tool is selected
    ToolDescription *tool = this->getSelectedTool();
    if(tool == nullptr)
    {
        return;
    }
    const QJsonObject &jsonObject = this->getToolDescriptor(tool->id);
    const QString &description = jsonObject["description"].toString();
    const QString &name = jsonObject["name"].toString();
    const QJsonObject &tags = jsonObject["tags"].toObject();
    QString tagsString;

    // Parse tags
    const QStringList &tagNames = tags.keys();
    for(int i=0 ; i<tagNames.size() ; i++)
    {
        const QJsonValue &tag = tags.value(tagNames[i]);
        tagsString += tagNames[i] + ": ";
        if(tag.isString())
        {
            tagsString += tag.toString();
        }
        else if(tag.isArray())
        {
            const QJsonArray &tagArray = tag.toArray();
            for(int i=0 ; i<tagArray.size() ; i++)
            {
                tagsString += tagArray[i].toString() + ( i < tagArray.size() - 1 ? ", " : "");
            }
        }
        else if(tag.isObject())
        {
            QJsonDocument tagDocument;
            tagDocument.setObject(tag.toObject());
            tagsString += tagDocument.toJson();
        }
        else
        {
            tagsString += tag.toString();
        }
        if(i < tags.size() - 1)
        {
            tagsString += "\n\t\t";
        }
    }

    QString shortDescription = "Name: \t\t" + name + "\n";
    shortDescription += "Description: \t" + description + "\n";
    shortDescription += "Tags: \t\t" + tagsString + "\n";
    this->infoShortDescription->setText(shortDescription);
    this->moreInfoButton->setText("More info");
    this->info->clear();
    this->info->hide();
    this->infoLabel->show();
    this->infoShortDescription->show();
    this->moreInfoButton->show();

    emit toolSelected();
}

void medBoutiquesSearchToolsWidget::toggleMoreInfo()
{
    if(this->moreInfoButton->text() == "More info")
    {
        if(this->info->toPlainText().isEmpty())
        {
            this->prettyPrint();
        }
        else
        {
            this->moreInfoButton->setText("Less info");
            this->moreInfoButton->show();
            this->infoShortDescription->hide();
            this->info->show();
        }
    }
    else
    {
        this->moreInfoButton->setText("More info");
        this->info->hide();
        this->infoShortDescription->show();
        this->moreInfoButton->show();
    }
}

void medBoutiquesSearchToolsWidget::prettyPrint()
{
    // Pretty print the tool help when a tool is selected
    ToolDescription *tool = this->getSelectedTool();
    if(tool == nullptr)
    {
        return;
    }

    // If pprint is already runnning: kill it and ignore the next pprint error, then retry prettyPrint()
    if(this->pprintProcess->state() != QProcess::NotRunning)
    {
        this->ignorePPrintError = true;
        this->pprintProcess->kill();
        QTimer::singleShot(100, this, &medBoutiquesSearchToolsWidget::prettyPrint);
        return;
    }

    // Start "bosh pprint"
    this->ignorePPrintError = false;
    this->loadingLabel->setText("Getting tool help...");
    this->pprintProcess->start(BoutiquesPaths::Python(), {BoutiquesPaths::Bosh(), "pprint", tool->id});
}

void medBoutiquesSearchToolsWidget::pprintProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)

    // Once pprint process is finished: emit toolSelected() so that other widgets update (invocationGUIWidget creates a GUI accordingly)

    if(exitCode == 0)
    {
        // If pprint terminated successfully: display output
        QByteArray result = this->pprintProcess->readAllStandardOutput();
        this->info->setText(QString::fromUtf8(result));
    }
    else
    {
        // If pprint encountered an error: display error
        if(!this->ignorePPrintError)
        {
            this->info->setText("Error while reading the tool description.");
        }
    }

    // Show info and emit toolSelected()
    this->infoShortDescription->hide();
    this->moreInfoButton->setText("Less info");
    this->info->show();
}

void medBoutiquesSearchToolsWidget::searchBoutiquesTools()
{
    // When user launch a new search (with enter or the "search" button):
    //  - update the tool database if necessary (toolDatabaseUpdated)
    //  - emit toolDeselected() to hide unecessary widgets
    //  - if this->descriptors is not empty: the tool database was loaded, all tools are added in the model and filtered while typing: do nothing when user hits enter.
    //  - otherwise: the launch a regular "bosh search" to query boutiques tool in the zenodo database

    if(this->toolDatabaseUpdated)
    {
        this->loadToolDatabase();
        this->toolDatabaseUpdated = false;
    }

    emit toolDeselected();

    if(!this->descriptors.isEmpty())
    {
        // If descriptors are not empty: the tool database was loaded, all tools are added in the model and filtered while typing: do nothing when user hits enter.
        return;
    }

    // If the search process is already runnning: kill it and retry searchBoutiquesTools()
    if(this->searchProcess->state() != QProcess::NotRunning)
    {
        this->searchProcess->kill();
        QTimer::singleShot(100, this, &medBoutiquesSearchToolsWidget::searchBoutiquesTools);
        return;
    }

    // Hide unused widgets
    this->loadingLabel->show();
    this->infoLabel->hide();
    this->info->hide();

    // Launch "bosh search" to query the Zenodo database
    QString searchQuery = this->searchLineEdit->text();

    this->loadingLabel->setText("Search launched...");
    this->searchResults.clear();
    this->searchProcess->start(BoutiquesPaths::Python(), {BoutiquesPaths::Bosh(), "search", "-m 50", searchQuery});
}

void medBoutiquesSearchToolsWidget::errorOccurred(QProcess::ProcessError error)
{
    Q_UNUSED(error)
    this->loadingLabel->setText("An error occurred while searching or reading the tool.");
}

void medBoutiquesSearchToolsWidget::searchProcessStarted()
{
    this->loadingLabel->setText("Searching for tools...");
}

QStringList medBoutiquesSearchToolsWidget::readSearchResults(QString &output)
{
    // Remove the color characeters from the search results
    QString outputClean = output.replace(QRegExp("\x1b\[[0-9;]*[mGKF]"), "");

    // Extract each line from output
    QTextStream outputSream(&outputClean);
    QStringList lines;
    while (!outputSream.atEnd())
    {
       lines.push_back(outputSream.readLine());
    }
    return lines;
}

void medBoutiquesSearchToolsWidget::parseSearchResults(const QStringList &lines, vector<ToolDescription> &searchResults)
{
    // Extract ID, Title, Description and number of Downloads from search results
    QString line = lines[1];
    int idIndex = line.indexOf("ID");
    int titleIndex = line.indexOf("TITLE");
    int descriptionIndex = line.indexOf("DESCRIPTION");
    int nDownloadsIndex = line.indexOf("DOWNLOADS");

    for(int i=2 ; i<lines.size() ; i++){
        QString line = lines[i];

        searchResults.emplace_back();
        ToolDescription& searchResult = searchResults.back();
        searchResult.id = line.mid(idIndex, titleIndex - idIndex).trimmed();
        searchResult.title = line.mid(titleIndex, descriptionIndex - titleIndex).trimmed();
        searchResult.description = line.mid(descriptionIndex, nDownloadsIndex - descriptionIndex).trimmed();
        searchResult.nDownloads = line.mid(nDownloadsIndex, line.size() - 1).trimmed().toInt();
    }
}

void medBoutiquesSearchToolsWidget::addSearchResult(const ToolDescription &toolDescription, const unsigned int toolDescriptionIndex)
{
    // Add search result in the tool model
    this->toolModel->insertRow(0);
    this->toolModel->setData(this->toolModel->index(0, 0), toolDescription.id);
    this->toolModel->setData(this->toolModel->index(0, 1), toolDescription.title);
    this->toolModel->setData(this->toolModel->index(0, 2), toolDescription.description);
    this->toolModel->setData(this->toolModel->index(0, 3), toolDescription.nDownloads);
    this->toolModel->setData(this->toolModel->index(0, 0), QSize(50, 50), Qt::SizeHintRole);
    this->toolModel->setData(this->toolModel->index(0, 0), toolDescriptionIndex, Qt::UserRole);
}

void medBoutiquesSearchToolsWidget::displaySearchResults()
{
    // Add all search results to the tool model (the proxy model will be filtered according to the search input field)
    this->loadingLabel->hide();
    this->searchView->show();

    this->toolModel->removeRows(0, this->toolModel->rowCount());
    for(unsigned int i=0 ; i<this->searchResults.size() ; i++){
        const ToolDescription &searchResult = this->searchResults[i];
        this->addSearchResult(searchResult, i);
    }
}

void medBoutiquesSearchToolsWidget::searchProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)

    // Extract and display search results, display an error if the search process returns less than two lines
    this->loadingLabel->hide();

    QString output = QString::fromUtf8(this->searchProcess->readAll());
    QStringList lines = this->readSearchResults(output);

    if(lines.size() < 2)
    {
        if(output.contains("Error"))
        {
            QMessageBox::warning(this, "Error while searching in Zenodo database.", "An error occured while searching the zenodo database.");
        }
        return;
    }

    searchResults.clear();

    this->parseSearchResults(lines, this->searchResults);
    this->displaySearchResults();
}

void medBoutiquesSearchToolsWidget::createToolDatabase(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)
    if(exitStatus != QProcess::ExitStatus::NormalExit)
    {
        return;
    }

    // Extract tools from output of the tool database process
    // Then pull all tools from zenodo database

    QString output = QString::fromUtf8(this->toolDatabaseProcess->readAll());
    QStringList lines = this->readSearchResults(output);

    if(lines.size() < 2)
    {
        return;
    }

    this->allTools.clear();
    this->parseSearchResults(lines, this->allTools);

    QStringList args;
    args.push_back(BoutiquesPaths::Bosh());
    args.push_back("pull");
    for(unsigned int i=0 ; i<this->allTools.size() ; i++){
        const ToolDescription &tool = this->allTools[i];
        args.push_back(tool.id);
    }
    disconnect(this->toolDatabaseProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &medBoutiquesSearchToolsWidget::createToolDatabase);
    connect(this->toolDatabaseProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &medBoutiquesSearchToolsWidget::pullProcessFinished);

    this->toolDatabaseProcess->start(BoutiquesPaths::Python(), args);
}

void medBoutiquesSearchToolsWidget::pullProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)

    // Create the database file "~/.cache/boutiques/all-descriptors.json" containing all descriptors
    // "all-descriptors.json" is just an simple (unordered) list of the descriptors

    QDir cacheDirectory(QDir::homePath() + BOUTIQUES_CACHE_PATH);

    // Extract all descriptors from all zenodo-TOOL_ID.json files in ~/.cache/boutiques

    QJsonObject descriptors;
    for(const ToolDescription &toolDescription: this->allTools)
    {
        QJsonObject descriptorObject = this->getToolDescriptor(toolDescription.id);
        descriptorObject["id"] = toolDescription.id;
        descriptorObject["nDownloads"] = toolDescription.nDownloads;
        descriptors[toolDescription.id] = descriptorObject;
    }

    // Use those descriptors to write the "all-descriptors.json" file

    QFile descriptorsFile(cacheDirectory.absoluteFilePath(DATABASE_NAME));
    if (!descriptorsFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Could not open descriptors file", "Error while opening descriptors file.");
        return;
    }
    QJsonDocument descriptorsDocument(descriptors);
    descriptorsFile.write(descriptorsDocument.toJson());

    // Do not loadToolDatabase() right away since it is not safe to modify the model while the user is searching a tool
    // loadToolDatabase() will be called when the user finishes his search (hit enter or press search), at this point it will be safe to update descriptors and searchResults
    this->toolDatabaseUpdated = true;
}

void medBoutiquesSearchToolsWidget::loadToolDatabase()
{
    // Load tool database from the file "~/.cache/boutiques/all-descriptors.json" containing all tool descriptors
    // "all-descriptors.json" is just an simple (unordered) list of the descriptors

    QDir cacheDirectory(QDir::homePath() + BOUTIQUES_CACHE_PATH);

    QFile file(cacheDirectory.absoluteFilePath(DATABASE_NAME));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // Fail silently if database file is not found
        return;
    }

    // Populate searchResults with all tools and display search results
    QJsonDocument descriptorsDocument(QJsonDocument::fromJson(file.readAll()));
    this->descriptors = descriptorsDocument.object();

    searchResults.clear();

    const QStringList &descriptorIds = this->descriptors.keys();
    for(const QString& id:descriptorIds)
    {
        const QJsonObject &descriptor = this->descriptors.value(id).toObject();
        this->searchResults.emplace_back();
        ToolDescription& searchResult = this->searchResults.back();
        searchResult.title = descriptor["name"].toString();
        searchResult.description = descriptor["description"].toString();
        searchResult.id = descriptor["id"].toString();
        searchResult.nDownloads = descriptor["nDownloads"].toInt();
    }

    this->displaySearchResults();
}
