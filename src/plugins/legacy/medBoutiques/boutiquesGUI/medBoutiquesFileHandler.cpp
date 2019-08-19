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

medBoutiquesFileHandler::medBoutiquesFileHandler(medBoutiquesToolBox *toolbox): toolbox(toolbox)
{
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

void medBoutiquesFileHandler::checkAcceptDragEvent(QDragEnterEvent *event)
{
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
    QList<FormatObject> fileFormats;
    QList<QString> allWriters = medAbstractDataFactory::instance()->writers();
    QHash<QString, dtkAbstractDataWriter*> possibleWriters;

    foreach(QString writerType, allWriters) {
        dtkAbstractDataWriter * writer = medAbstractDataFactory::instance()->writer(writerType);
        if (writer->handled().contains(data->identifier()))
            possibleWriters[writerType] = writer;
        else
            delete writer;
    }

    if (possibleWriters.isEmpty()) {
        medMessageController::instance()->showError("Sorry, we have no exporter for this format.");
        return fileFormats;
    }

    // we use allWriters as the list of keys to make sure we traverse possibleWriters
    // in the order specified by the writers priorities.
    foreach(QString type, allWriters) {
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
        formatAndExtension = FormatAndExtension(this->dataTypeToFormatAndExtension[dataType].toArray());
    }
    else
    {
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
            formatAndExtension = this->getFormatForInputFile(dataType, fileFormats);
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
    const FormatAndExtension &formatAndExtension = this->getFormatAndExtensionForData(data);
    return this->createTemporaryInputFile(data, formatAndExtension.type, formatAndExtension.extension);
#endif
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

FormatAndExtension medBoutiquesFileHandler::getFormatForInputFile(const QString &dataType, const QList<FormatObject> &fileFormats)
{
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

    //    QCheckBox *checkBox = new QCheckBox("Always use this file format when possible");
    //    checkBox->setCheckState(Qt::Checked);
    QRadioButton *alwaysWhenPossibleButton = new QRadioButton("Always use this file format when possible");
    QRadioButton *alwaysWidthThisTypeButton = new QRadioButton("Always use this file format for this type of data");
    QRadioButton *askNextTimeButton = new QRadioButton("Ask again next time");

    messageBoxLayout->addWidget(hGroup);
//    messageBoxLayout->addWidget(checkBox);
    messageBoxLayout->addWidget(alwaysWhenPossibleButton);
    messageBoxLayout->addWidget(alwaysWidthThisTypeButton);
    messageBoxLayout->addWidget(askNextTimeButton);
    messageBoxLayout->addWidget(okButton);

    alwaysWhenPossibleButton->setChecked(true);

    messageBox.exec();

    QString chosenExtension = typeComboBox->itemData(typeComboBox->currentIndex(), Qt::UserRole + 1).toString();
    QString chosenType = typeComboBox->itemData(typeComboBox->currentIndex(), Qt::UserRole + 2).toString();

    if(alwaysWhenPossibleButton->isChecked())
    {
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

    if(alwaysWidthThisTypeButton->isChecked())
    {
        this->dataTypeToFormatAndExtension[dataType] = QJsonArray::fromStringList({chosenType, chosenExtension});
        this->savePreferredFormatSettings();
    }

    return FormatAndExtension(chosenType, chosenExtension);
}

QString medBoutiquesFileHandler::createTemporaryInputFile(medAbstractData *data, const QString &chosenType, const QString &chosenExtension)
{
    Q_UNUSED(data)
    QTemporaryFile file("XXXXXX_" + chosenType + chosenExtension);
    if (file.open()) {
        QString absoluteFilePath = QFileInfo(file).absoluteFilePath();
#ifndef BOUTIQUE_GUI_STANDALONE
        medDataManager::instance()->exportDataToPath(data, absoluteFilePath, chosenType);
#endif
        return absoluteFilePath;
    }
    return "";
}


void medBoutiquesFileHandler::savePreferredFormatSettings()
{
    QFile file(BoutiquesPaths::Settings());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }
    QJsonObject preferredFormats;
    preferredFormats["dataTypeToFormatAndExtension"] = this->dataTypeToFormatAndExtension;
    preferredFormats["preferredFormatsAndExtensions"] = this->preferredFormatsAndExtensions;
    preferredFormats["outputExtensions"] = this->outputExtensions;
    QJsonDocument document(preferredFormats);
    file.write(document.toJson());
}
