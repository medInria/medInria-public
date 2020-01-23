#include "medBoutiquesFileHandler.h"

#include <iostream>
#include <QtWidgets>

#ifndef BOUTIQUE_GUI_STANDALONE

#include "medDataIndex.h"
#include "medDataManager.h"
#include "medAbstractDataFactory.h"
#include "medMessageController.h"
#include "medBoutiquesToolBox.h"

#endif

medBoutiquesFileHandler::medBoutiquesFileHandler(medBoutiquesToolBox *toolbox):medBoutiquesAbstractFileHandler(), toolbox(toolbox)
{
    // Read dataTypeToFormatAndExtension, preferredFormatsAndExtensions and outputExtensions from the settings

    QFile file(BoutiquesPaths::Settings());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QJsonDocument jsonDocument(QJsonDocument::fromJson(file.readAll()));

    QJsonObject json = jsonDocument.object();
    if(json.contains("dataTypeToFormatAndExtension") && json["dataTypeToFormatAndExtension"].isObject())
    {
        this->dataTypeToFormatAndExtension = json["dataTypeToFormatAndExtension"].toObject();
    }

    this->preferredFormatsAndExtensions = json["preferredFormatsAndExtensions"].toArray();

    this->outputExtensions = json["outputExtensions"].toArray();
}

medBoutiquesFileHandler::~medBoutiquesFileHandler()
{
    this->deleteTemporaryFiles();
}

void medBoutiquesFileHandler::checkAcceptDragEvent(QDragEnterEvent *event)
{
    // See description in filehandler.h
#ifdef BOUTIQUE_GUI_STANDALONE
    event->acceptProposedAction();
#else
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls())
    {
        event->acceptProposedAction();
    }
    medDataIndex index = medDataIndex::readMimeData(mimeData);
    if (index.isValidForSeries())
    {
        event->acceptProposedAction();
    }
#endif
}

QList<FormatObject> medBoutiquesFileHandler::getFileFormatsForData(medAbstractData *data)
{
#ifdef BOUTIQUE_GUI_STANDALONE
    Q_UNUSED(data)
    QList<FormatObject> fileFormats;
    fileFormats.push_back(FormatObject("Type 1", "Description 1", {".ext1", ".ext2"}));
    fileFormats.push_back(FormatObject("Type 2", "Description 2", {".ext3", ".ext4"}));
    return fileFormats;
#else
    // Get all compatible writers
    QList<FormatObject> fileFormats;
    QList<QString> allWriters = medAbstractDataFactory::instance()->writers();
    QHash<QString, dtkAbstractDataWriter*> possibleWriters;

    foreach(QString writerType, allWriters)
    {
        dtkAbstractDataWriter * writer = medAbstractDataFactory::instance()->writer(writerType);
        if (writer->handled().contains(data->identifier()))
            possibleWriters[writerType] = writer;
        else
            delete writer;
    }

    // Warn user if no compatible writer, and return an empty list
    if (possibleWriters.isEmpty())
    {
        medMessageController::instance()->showError("Sorry, we have no exporter for this format.");
        return fileFormats;
    }

    // We use allWriters as the list of keys to make sure we traverse possibleWriters
    // in the order specified by the writers priorities.
    foreach(QString type, allWriters)
    {
        if (!possibleWriters.contains(type))
            continue;

        QStringList extensions = possibleWriters[type]->supportedFileExtensions();
        fileFormats.append(FormatObject(type, possibleWriters[type]->description(), extensions));
    }

    return fileFormats;
#endif
}

FormatAndExtension medBoutiquesFileHandler::getFormatAndExtensionForData(medAbstractData *data)
{
#ifdef BOUTIQUE_GUI_STANDALONE
    Q_UNUSED(data)
    FormatAndExtension formatAndExtension("Type 1", ".ext1");
    return formatAndExtension;
#else

    FormatAndExtension formatAndExtension;
    const QString &dataType = data->identifier();
    if(this->dataTypeToFormatAndExtension.contains(dataType))
    {
        // If the data type is in dataTypeToFormatAndExtension: return the associated format and extension
        formatAndExtension = FormatAndExtension(this->dataTypeToFormatAndExtension[dataType].toArray());
    }
    else
    {
        // Otherwise: for all formats compatible with the data type: return the first one in preferredFormatsAndExtensions if any
        const QList<FormatObject> &fileFormats = this->getFileFormatsForData(data);
        bool foundPreferredFormat = false;
        for(const auto &formatObject : fileFormats)
        {
            for(int i = 0 ; i<this->preferredFormatsAndExtensions.size() ; ++i)
            {
                const QJsonArray &formatAndExtensionArray = this->preferredFormatsAndExtensions.at(i).toArray();
                if(formatAndExtensionArray.size() == 2 && formatObject.type == formatAndExtensionArray[0].toString())
                {
                    formatAndExtension = FormatAndExtension(formatAndExtensionArray);
                    foundPreferredFormat = true;
                    break;
                }
            }
            if(foundPreferredFormat)
            {
                break;
            }
        }
        if(!foundPreferredFormat)
        {
            // If no format was found: ask the user to select one
            formatAndExtension = this->askFormatAndExtensionForData(dataType, fileFormats);
        }
    }
    return formatAndExtension;
#endif
}

QString medBoutiquesFileHandler::createTemporaryInputFileForMimeData(const QMimeData *mimeData)
{
#ifdef BOUTIQUE_GUI_STANDALONE
    Q_UNUSED(mimeData)
    return this->createTemporaryInputFile(nullptr, "Type 2", ".ext2");
#else
    medDataIndex index = medDataIndex::readMimeData(mimeData);
    if (index.isValidForSeries())
    {
        medAbstractData *data = medDataManager::instance()->retrieveData(index);

        const FormatAndExtension &formatAndExtension = this->getFormatAndExtensionForData(data);
        return this->createTemporaryInputFile(data, formatAndExtension.type, formatAndExtension.extension);
    }
    return "";
#endif
}

QString medBoutiquesFileHandler::createTemporaryInputFileForCurrentInput()
{
#ifdef BOUTIQUE_GUI_STANDALONE
    return this->createTemporaryInputFile(nullptr, "Type 1", ".ext1");
#else
    medAbstractData *data = toolbox->getInput();
    if(data == nullptr) {
        QMessageBox::warning(nullptr, "No input selected", "There is no file opened as input filter. Select an input file for the filter (on the left panel), it will then be usable as an input for the Boutiques tool.");
        return "";
    }
    const FormatAndExtension &formatAndExtension = this->getFormatAndExtensionForData(data);
    return this->createTemporaryInputFile(data, formatAndExtension.type, formatAndExtension.extension);
#endif
}

void medBoutiquesFileHandler::deleteTemporaryFiles()
{
    for(QString &fileName : this->temporaryFiles)
    {
        QFile file(fileName);
        file.remove();
    }
    this->temporaryFiles.clear();
}

bool medBoutiquesFileHandler::hasKnownExtension(const QString &fileName)
{
    for (int i = 0 ; i<this->outputExtensions.size() ; ++i)
    {
        const QString &outputExtension = this->outputExtensions[i].toString();
        if(fileName.endsWith(outputExtension))
        {
            return true;
        }
    }
    return false;
}

QString medBoutiquesFileHandler::normalizePath(const QString &path)
{
    QRegularExpression regExp("^[A-Z]:");
    if(regExp.match(path).hasMatch())
    {
        QString newPath = QString(path).replace("\\", "/");
        return "/" + QString(newPath[0].toLower()) + newPath.mid(2);
    }
    return path;
}

QStringList medBoutiquesFileHandler::normalizePaths(const QStringList &paths)
{
    QStringList normalizedPaths;
    for(const QString &file:paths)
    {
        normalizedPaths.append(this->normalizePath(file));
    }
    return normalizedPaths;
}

FormatAndExtension medBoutiquesFileHandler::askFormatAndExtensionForData(const QString &dataType, const QList<FormatObject> &fileFormats)
{
    // Create a dialog with a combo box to select the format and extension for the given format
    QDialog messageBox;

    QVBoxLayout *messageBoxLayout = new QVBoxLayout();
    messageBox.setLayout(messageBoxLayout);

    QWidget *hGroup = new QWidget();
    QHBoxLayout *hLayout = new QHBoxLayout();

    QLabel *title = new QLabel("<b>Please select a file type</b>");
    QLabel *message = new QLabel("This input data must be saved as a temporary file to be used by Boutiques tools.\n Please choose a file format for the temporary file.");
    QPushButton *okButton = new QPushButton("Ok");

    connect(okButton, &QPushButton::clicked, [&messageBox]() { messageBox.close(); } );

    messageBoxLayout->addWidget(title);
    messageBoxLayout->addWidget(message);

    // Create the combo box and populate it with the compatible formats
    QComboBox* typeComboBox = new QComboBox();

    QStandardItemModel * model = new QStandardItemModel();

    typeComboBox->setModel(model);
    for(const FormatObject &formatObject : fileFormats)
    {
        QStandardItem* item = new QStandardItem( formatObject.description );
        item->setData(formatObject.extensions.first(), Qt::UserRole + 0);
        item->setData(formatObject.type, Qt::UserRole + 1);
        QFont font = item->font();
        font.setBold( true );
        item->setFont( font );
        item->setSelectable(false);
        item->setEnabled(true);
        model->appendRow( item );
        for(const QString &extension: formatObject.extensions)
        {
            typeComboBox->addItem(extension);
            typeComboBox->setItemData(typeComboBox->count() - 1, extension, Qt::UserRole + 0);
            typeComboBox->setItemData(typeComboBox->count() - 1, formatObject.type, Qt::UserRole + 1);
        }
    }

    hLayout->addWidget(new QLabel("File format:", hGroup));
    hLayout->addWidget(typeComboBox);
    hGroup->setLayout(hLayout);

    // The user has three choices:
    //  - Always use this file format when possible: add the chosen value to preferredFormatsAndExtensions
    //  - Always use this file format for this type of data: add the chosen value to dataTypeToFormatAndExtension
    //  - Ask again next time: pretty explicit
    QRadioButton *alwaysWithThisTypeButton = new QRadioButton("Always use this file format for this type of data");
    QRadioButton *alwaysWhenPossibleButton = new QRadioButton("Always use this file format when possible (and no format is associated with this type of data)");
    QRadioButton *askNextTimeButton = new QRadioButton("Ask again next time");

    messageBoxLayout->addWidget(hGroup);
    messageBoxLayout->addWidget(alwaysWithThisTypeButton);
    messageBoxLayout->addWidget(alwaysWhenPossibleButton);
    messageBoxLayout->addWidget(askNextTimeButton);
    messageBoxLayout->addWidget(okButton);

    alwaysWhenPossibleButton->setChecked(true);

    messageBox.exec();

    // Get chosen type and extension
    QString chosenExtension = typeComboBox->itemData(typeComboBox->currentIndex(), Qt::UserRole + 1).toString();
    QString chosenType = typeComboBox->itemData(typeComboBox->currentIndex(), Qt::UserRole + 2).toString();

    // Update preferredFormatsAndExtensions if alwaysWhenPossible
    if(alwaysWhenPossibleButton->isChecked())
    {
        // Check that type and extension is not already in preferredFormatsAndExtensions before adding it
        bool containsChoice = false;
        for(int i = 0 ; i<this->preferredFormatsAndExtensions.size() ; ++i)
        {
            const QJsonArray &formatAndExtensionArray = this->preferredFormatsAndExtensions.at(i).toArray();
            if(formatAndExtensionArray.size() == 2 && chosenType == formatAndExtensionArray[0].toString() && chosenExtension == formatAndExtensionArray[1].toString())
            {
                containsChoice = true;
                break;
            }
        }
        if(!containsChoice)
        {
            this->preferredFormatsAndExtensions.push_back(QJsonArray::fromStringList({chosenType, chosenExtension}));
            this->savePreferredFormatSettings();
        }
    }

    // Update dataTypeToFormatAndExtension if alwaysWithThisType
    if(alwaysWithThisTypeButton->isChecked())
    {
        this->dataTypeToFormatAndExtension[dataType] = QJsonArray::fromStringList({chosenType, chosenExtension});
        this->savePreferredFormatSettings();
    }

    return FormatAndExtension(chosenType, chosenExtension);
}

QString medBoutiquesFileHandler::createTemporaryInputFile(medAbstractData *data, const QString &chosenType, const QString &chosenExtension)
{
    Q_UNUSED(data)
    QTemporaryFile file(BoutiquesPaths::BoutiquesTemp().absoluteFilePath("XXXXXX_" + chosenType + chosenExtension));
    if (!chosenType.isEmpty() && file.open())
    {
        temporaryFiles.push_back(QDir::temp().absoluteFilePath(file.fileName()));
        QString absoluteFilePath = QFileInfo(file).absoluteFilePath();
#ifndef BOUTIQUE_GUI_STANDALONE
        medDataManager::instance()->exportDataToPath(data, absoluteFilePath, chosenType);
#endif
        return this->normalizePath(absoluteFilePath);
    }
    return "";
}


void medBoutiquesFileHandler::savePreferredFormatSettings()
{
    // Read settings from "boutiques-gui-settings.json"
    QFile readFile(BoutiquesPaths::Settings());
    QJsonObject settings;
    if (readFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QJsonDocument jsonDocument(QJsonDocument::fromJson(readFile.readAll()));
        settings = jsonDocument.object();
        readFile.close();
    }

    // Overwrite dataTypeToFormatAndExtension, preferredFormatsAndExtensions and outputExtensions and save settings file
    QFile writeFile(BoutiquesPaths::Settings());
    if (!writeFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }
    settings["dataTypeToFormatAndExtension"] = this->dataTypeToFormatAndExtension;
    settings["preferredFormatsAndExtensions"] = this->preferredFormatsAndExtensions;
    settings["outputExtensions"] = this->outputExtensions;
    QJsonDocument document(settings);
    writeFile.write(document.toJson());
}
