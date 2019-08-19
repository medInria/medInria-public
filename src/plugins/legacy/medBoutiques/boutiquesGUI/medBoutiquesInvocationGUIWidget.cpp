#include <iostream>
#include <QtWidgets>
#include "medBoutiquesInvocationGUIWidget.h"

medBoutiquesInvocationGUIWidget::medBoutiquesInvocationGUIWidget(QWidget *parent, medBoutiquesSearchToolsWidget *searchToolsWidget, medBoutiquesAbstractFileHandler *medBoutiquesFileHandler) :
    QWidget(parent),
    searchToolsWidget(searchToolsWidget),
    fileHandler(medBoutiquesFileHandler),
    optionalInputGroup(nullptr),
    completeInvocationJSON(nullptr),
    ignoreSignals(false)
{
    this->layout = new QVBoxLayout(this);
    this->setMinimumHeight(750);
//    this->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::MinimumExpanding);
    this->scrollArea = new QScrollArea(this);
//    this->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    this->scrollArea->setWidgetResizable(true);
    this->scrollArea->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);

    this->createSelectCurrentDirectoryGUI();
    this->layout->addWidget(this->scrollArea);
    this->group = nullptr;
    this->setLayout(this->layout);

    this->emitInvocationChangedTimer = new QTimer(this);
    this->emitInvocationChangedTimer->setSingleShot(true);
    connect(this->emitInvocationChangedTimer, &QTimer::timeout, [this](){ emit invocationChanged();});
}

medBoutiquesInvocationGUIWidget::~medBoutiquesInvocationGUIWidget()
{
    if(this->completeInvocationJSON != nullptr)
    {
        delete this->completeInvocationJSON;
    }
}

void medBoutiquesInvocationGUIWidget::createSelectCurrentDirectoryGUI()
{
    this->selectCurrentDirectoryGroupBox = new QGroupBox();
    selectCurrentDirectoryGroupBox->setTitle("Set root directory for all relative input and output file paths");
    selectCurrentDirectoryGroupBox->setCheckable(true);
    selectCurrentDirectoryGroupBox->setChecked(false);
    selectCurrentDirectoryGroupBox->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Maximum);

    QLabel *label = new QLabel("Current working directory:");

    this->selectCurrentDirectoryLineEdit = new QLineEdit();
    selectCurrentDirectoryLineEdit->setPlaceholderText("/Path/To/Current/Working/Directory/ (default is " + QDir::homePath() + ")");
    selectCurrentDirectoryLineEdit->setText(QDir::homePath());

    QPushButton *pushButton = new QPushButton("Select current directory");

    connect(pushButton, &QPushButton::clicked, [this]() { this->selectCurrentDirectoryLineEdit->setText(QFileDialog::getExistingDirectory(this, "Set current working directory")); } );

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(label);
    hLayout->addWidget(selectCurrentDirectoryLineEdit);
    hLayout->addWidget(pushButton);
    selectCurrentDirectoryGroupBox->setLayout(hLayout);

    this->layout->addWidget(selectCurrentDirectoryGroupBox);
}

bool medBoutiquesInvocationGUIWidget::inputGroupIsMutuallyExclusive(const QString &inputId)
{
    auto it = this->idToInputObject.find(inputId);
    if(it == this->idToInputObject.end())
    {
        return false;
    }
    const InputObject &inputObject = it->second;
    return inputObject.group != nullptr && inputObject.group->description["mutually-exclusive"].toBool();
}

void medBoutiquesInvocationGUIWidget::removeMutuallyExclusiveParameters(const QString &inputId)
{
    if(this->inputGroupIsMutuallyExclusive(inputId))
    {
        GroupObject *groupObject = this->idToInputObject.at(inputId).group;
        if(groupObject != nullptr && groupObject->description.contains("members"))
        {
            QJsonArray inputArray = groupObject->description["members"].toArray();
            this->ignoreSignals = true;
            for (int i=0 ; i<inputArray.size() ; ++i)
            {
                const QString &member = inputArray[i].toString();
                this->invocationJSON->remove(member);
            }
            this->ignoreSignals = false;
        }
    }
}

inline QJsonArray medBoutiquesInvocationGUIWidget::stringToArray(const QString &string)
{
    QJsonDocument jsonDocument(QJsonDocument::fromJson(QByteArray::fromStdString("[" + string.toStdString() + "]")));
    return jsonDocument.array();
}

void medBoutiquesInvocationGUIWidget::valueChanged(const QString &inputId)
{
    if(this->ignoreSignals)
    {
        return;
    }
    const InputObject &inputObject = this->idToInputObject.at(inputId);
    const QJsonValue &value = inputObject.getValue();
//    const QString &inputType = inputObject.description["type"].toString();
    bool inputIsList = inputObject.description["list"].toBool();
//    bool inputIsStringButValueIsNullOrEmpty = inputType == "String" && !inputIsList && ( value.toString().isEmpty() || value.toString().isNull() );
    bool inputIsListButValueIsInvalidOrEmpty = inputIsList && (!value.isArray() || value.toArray().isEmpty());
    bool removeInputFromInvocation = inputIsListButValueIsInvalidOrEmpty;
    if(removeInputFromInvocation)
    {
        this->invocationJSON->remove(inputId);
    }
    else
    {
        this->invocationJSON->insert(inputId, value);
    }
    this->emitInvocationChanged();
}

void medBoutiquesInvocationGUIWidget::optionalGroupChanged(bool on)
{
    if(on)
    {
        // when check optional parameters:
        //  - if we stored values (when we unchecked optional parameters): restore them
        if(this->completeInvocationJSON != nullptr)
        {
            const QStringList& keys = this->completeInvocationJSON->keys();
            for(const auto& key: keys)
            {
                auto it = this->idToInputObject.find(key);
                if(it != this->idToInputObject.end())
                {
                    const QJsonObject& description = it->second.description;
                    if(description["optional"].toBool())
                    {
                        this->invocationJSON->insert(key, this->completeInvocationJSON->value(key));
                    }
                }
            }
        }
    }
    else
    {
        // when uncheck optional parameters:
        //  - remove all optional parameters from invocationJSON
        //  - store the optional values to be able to restore them later (when re-checking optional parameters)
        if(this->completeInvocationJSON != nullptr)
        {
            delete this->completeInvocationJSON;
        }
        this->completeInvocationJSON = new QJsonObject(*this->invocationJSON);
        const QStringList& keys = this->invocationJSON->keys();
        for(const auto& key: keys)
        {
            auto it = this->idToInputObject.find(key);
            if(it != this->idToInputObject.end())
            {
                const QJsonObject& description = it->second.description;
                if(description["optional"].toBool())
                {
                    this->invocationJSON->remove(key);
                }
            }
        }
    }
    this->emitInvocationChanged();
}

pair<QGroupBox *, QVBoxLayout *> medBoutiquesInvocationGUIWidget::createGroupAndLayout(const QString &name)
{
    QGroupBox *group = new QGroupBox();
    group->setTitle(name);
    QVBoxLayout *layout = new QVBoxLayout(group);
    group->setLayout(layout);
    return pair<QGroupBox *, QVBoxLayout *>(group, layout);
}

void medBoutiquesInvocationGUIWidget::mutuallyExclusiveGroupChanged(GroupObject *groupObject, int itemIndex)
{
    if(groupObject->comboBox->count() == 0)
    {
        return;
    }
    const auto &members= groupObject->description["members"].toArray();
    for(const auto &member: members)
    {
        const QString &memberId = member.toString();
        const auto &it = this->idToInputObject.find(memberId);
        this->invocationJSON->remove(memberId);
        if(it == this->idToInputObject.end() || it->second.widget == nullptr)
        {
            continue;
        }
        it->second.widget->hide();
    }

    const QString &inputId = groupObject->comboBox->itemData(itemIndex).toString();
    InputObject &inputObject = this->idToInputObject.at(inputId);
    if(inputObject.widget != nullptr)
    {
        inputObject.widget->show();
        this->invocationJSON->insert(inputId, inputObject.getValue());
    }
    else
    {
        this->invocationJSON->insert(inputId, QJsonValue(true));
    }

    this->emitInvocationChanged();
}

void medBoutiquesInvocationGUIWidget::emitInvocationChanged()
{
    this->emitInvocationChangedTimer->stop();
    this->emitInvocationChangedTimer->start(250);
}

QWidget *medBoutiquesInvocationGUIWidget::createUnsetGroup(const QString &inputId, QWidget *widget)
{
    QHBoxLayout *hLayout = new QHBoxLayout();
    QWidget *hGroup = new QWidget();
    QPushButton *unsetPushButton = new QPushButton("Unset");
    unsetPushButton->setMaximumWidth(60);
    connect(unsetPushButton, &QPushButton::clicked, [this, inputId]()
    {
        this->invocationJSON->remove(inputId);
        this->emitInvocationChanged();
    } );
    hGroup->setLayout(hLayout);
    hLayout->addWidget(widget);
    hLayout->addWidget(unsetPushButton);
    return hGroup;
}

void medBoutiquesInvocationGUIWidget::parseDescriptor(QJsonObject *invocationJSON)
{
    // Widget structure:

    // By names:
    // this > layout > scrollArea > group > groupLayout > mainInputsGroupAndLayout & optionalInputsGroupAndLayout >
    //        inputWidget or ( inputGroups > inputWidget ) or (mutuallyExclusiveGroup > hWidget > hGroup > inputWidget)

    // With types:
    // this: QWidget > layout: QGroupBox > scrollArea: QScrollArea > group: QWidget > groupLayout: QVBoxLayout >
    //       mainInputsGroupAndLayout & optionalInputsGroupAndLayout: (QGroupBox > QVBoxLayout) > inputWidget: (QLineEdit or QSpinBox or QDoubleSpinBox or QPushButton or QCheckBox)*
    //       or ( inputGroups: (QGroupBox > QVBoxLayout) > inputWidget: * ) or (mutuallyExclusiveGroup: (QGroupBox > QVBoxLayout) > hWidget: QWidget > hGroup: QWidget > inputWidget: *)

    // *inputWidget can be:
    //   String: QLineEdit
    //   Number: QSpinBox or QDoubleSpinBox (depending on the integer property)
    //   File: QPushButton > QFileDialog::getOpenFileName()
    //   Flag: QCheckBox
    // if input is a list: QLineEdit to enter comma seperated values. Warning: File not handled yet!
    // if input group is mutually exclusive:
    //   The inputWidgets will be composed of a horizontal group (QWidget > QHBoxLayout) which then holds the inputWidget

    if(this->completeInvocationJSON != nullptr)
    {
        delete this->completeInvocationJSON;
        this->completeInvocationJSON = nullptr;
    }

    this->invocationJSON = invocationJSON;

    if(this->group != nullptr)
    {
        this->emitInvocationChangedTimer->stop();
        connect(this->group, &QWidget::destroyed, [this](){ this->parseDescriptor(this->invocationJSON); });
        this->scrollArea->takeWidget();
        this->group->deleteLater();
        this->group = nullptr;
        return;
    }

    this->group = new QWidget(this->scrollArea);
    this->scrollArea->setWidget(this->group);

    QVBoxLayout *groupLayout = new QVBoxLayout(this->group);
    this->group->setLayout(groupLayout);
    ToolDescription *searchResult = this->searchToolsWidget->getSelectedTool();

    QString id = QString::fromStdString(searchResult->id.toStdString()); // deep copy the string
    QString descriptorFileName = id.replace(QChar('.'), QChar('-')) + ".json";
    QDir cacheDirectory(QDir::homePath() + "/.cache/boutiques");

    QFile file(cacheDirectory.absoluteFilePath(descriptorFileName));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Could not open descriptor file", "Error while opening descriptor file (" + descriptorFileName + ") from ~/.cache/boutiques ");
        return;
    }

    QJsonDocument jsonDocument(QJsonDocument::fromJson(file.readAll()));

    QJsonObject json = jsonDocument.object();

    auto mainInputsGroupAndLayout = this->createGroupAndLayout("Required parameters");
    auto optionalInputsGroupAndLayout = this->createGroupAndLayout("Optional parameters");

    this->optionalInputGroup = optionalInputsGroupAndLayout.first;
    this->optionalInputGroup->setCheckable(true);
    connect(this->optionalInputGroup, &QGroupBox::toggled, this, &medBoutiquesInvocationGUIWidget::optionalGroupChanged);

    if ( !(json.contains("inputs") && json["inputs"].isArray() && (!json.contains("groups") || json["groups"].isArray()) ) )
    {
        QMessageBox::critical(this, "Error in descriptor file", "No inputs or groups.");
        return;
    }

    this->idToInputObject.clear();
    this->selectedOutputId.clear();

    QJsonArray inputArray = json["inputs"].toArray();
    for (int i = 0 ; i<inputArray.size() ; ++i)
    {
        const QJsonObject &description = inputArray[i].toObject();
        const auto &result = this->idToInputObject.emplace(pair<QString, InputObject>(description["id"].toString(), InputObject(description)));
        if(!result.second)
        {
            QMessageBox::warning(this, "Error in descriptor file", "Input " + result.first->first + "appears twice in descriptor, ignoring one of them...");
        }
        else
        {
            result.first->second.description = description;
        }
    }

    this->outputFiles = json["output-files"].toArray();

    this->groupObjects.clear();
    vector<pair<QGroupBox*, QVBoxLayout*>> destinationLayouts;
    QJsonArray groupArray = json["groups"].toArray();
    this->groupObjects.reserve(static_cast<size_t>(groupArray.size()));
    for (int i = 0 ; i<groupArray.size() ; ++i)
    {
        this->groupObjects.emplace_back();
        GroupObject &groupObject = this->groupObjects.back();
        groupObject.description = groupArray[i].toObject();
        if ( !(groupObject.description.contains("members") && groupObject.description["members"].isArray()) )
        {
            continue;
        }
        auto groupAndLayout = this->createGroupAndLayout(groupObject.description["name"].toString());
        groupObject.groupBox = groupAndLayout.first;
        groupObject.layout = groupAndLayout.second;
        bool groupIsOptional = true;
        QJsonArray memberArray = groupObject.description["members"].toArray();
        for (int j = 0 ; j<memberArray.size() ; ++j)
        {
            auto it = this->idToInputObject.find(memberArray[j].toString());
            if(it == this->idToInputObject.end())
            {
                continue;
            }
            InputObject &inputObject = it->second;
            inputObject.group = &groupObject;
            const QJsonValue &isOptional= inputObject.description["optional"];
            if(isOptional.isNull() || (isOptional.isBool() && !isOptional.toBool()) ) {
                groupIsOptional = false;
            }
        }
        groupObject.optional = groupIsOptional;
        destinationLayouts.push_back(pair<QGroupBox*, QVBoxLayout*>(groupAndLayout.first, groupIsOptional ? optionalInputsGroupAndLayout.second : mainInputsGroupAndLayout.second));
    }

    bool noOptionalInput = true;

    for (auto& idAndInputObject: idToInputObject)
    {
        const QString &inputId = idAndInputObject.first;
        InputObject &inputObject = idAndInputObject.second;
        GroupObject *groupObject = inputObject.group;

        const QString &inputName = inputObject.description["name"].toString();
        const QString &inputType = inputObject.description["type"].toString();
        const QString &inputDescription = inputObject.description["description"].toString();
        const QJsonValue &inputValue = this->invocationJSON->value(inputId);
        bool inputIsOptional = inputObject.description["optional"].toBool();

        noOptionalInput &= !inputIsOptional;

        QWidget *widget = nullptr;
        QLayout *parentLayout = nullptr;

        if(groupObject != nullptr)
        {
            parentLayout = groupObject->layout;
        }
        else if(inputIsOptional)
        {
            parentLayout = optionalInputsGroupAndLayout.second;
        }
        else
        {
            parentLayout = mainInputsGroupAndLayout.second;
        }

        bool inputGroupIsMutuallyExclusive = this->inputGroupIsMutuallyExclusive(inputId);

        //  if input is part of a mutually exclusive group:
        //      create a horizontal layout to put the radio button
        //      along with the corresponding widget
        //      (an open file button, a text field or a spinbox when type is File, String or Number respectively) if necessary (nothing if type is Flag)
        //      the later widget will be a child of this horizontal layout, and the idToGetValue will be set accordingly
//        if(inputGroupIsMutuallyExclusive)
//        {
//            QWidget *hWidget = new QWidget();
//            QHBoxLayout *hLayout = new QHBoxLayout(hWidget);
//            hWidget->setLayout(hLayout);
//            QRadioButton *radioButton = new QRadioButton(inputName, hWidget);
//            radioButton->setChecked(inputValue.toBool());
//            inputObject.radioButton = radioButton;

//            // when the widget value changes, the invocationJSON is updated by calling getValue() (with the help of self.idToGetValue)
//            // getValue will be overriden if input is not a Flag

//            inputObject.getValue = [radioButton]() { return QJsonValue(radioButton->isChecked()); };
//            connect(radioButton, &QRadioButton::toggled, [this, inputId]() { this->valueChanged(inputId); } );
//            radioButton->setToolTip(inputDescription);
//            hLayout->addWidget(radioButton);
//            parentLayout->addWidget(hWidget);
//            parentLayout = hLayout;
//            widget = hWidget;
//        }
        if(inputGroupIsMutuallyExclusive)
        {
            if(groupObject->comboBox == nullptr)
            {
                QComboBox *comboBox = new QComboBox();
                groupObject->comboBox = comboBox;
                parentLayout->addWidget(comboBox);
                connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, groupObject](int itemIndex) { this->mutuallyExclusiveGroupChanged(groupObject, itemIndex); } );
                if(groupObject->optional)
                {
                    groupObject->groupBox->setCheckable(true);
                    connect(groupObject->groupBox, &QGroupBox::toggled, this, [this, groupObject](bool on)
                    {
                        const QString &inputId = groupObject->comboBox->currentData().toString();
                        if(on)
                        {
                            InputObject &inputObject = this->idToInputObject.at(inputId);
                            this->invocationJSON->insert(inputId, inputObject.widget != nullptr ? inputObject.getValue() : QJsonValue(true));
                        }
                        else
                        {
                            this->invocationJSON->remove(inputId);
                        }
                        this->emitInvocationChanged();
                    } );
                }
            }
            groupObject->comboBox->addItem(inputName, QVariant(inputId));
            if(!inputValue.isNull())
            {
                groupObject->comboBox->setCurrentIndex(groupObject->comboBox->count() - 1);
            }
        }

        if(inputType == "String" || inputType == "Number" || inputType == "File")
        {
//            widget = new QGroupBox();
            widget = inputType == "File" ? new medBoutiquesDropWidget() : new QWidget();
            QHBoxLayout *layout = new QHBoxLayout();
            QLabel *label = new QLabel(inputName + ":");
            layout->addWidget(label);

            if(inputObject.description["list"].toBool())
            {
                QLineEdit *lineEdit = new QLineEdit();
                lineEdit->setPlaceholderText(QString("Comma separated ") + (inputType == "String" ? "strings" : inputType == "Number" ? "numbers" : "file paths") + ".");
                QString listString;

                QJsonDocument valueList;
                valueList.setArray(inputValue.toArray());
                lineEdit->setText(QString::fromUtf8(valueList.toJson()).remove('[').remove(']'));

                inputObject.getValue = [this, lineEdit]() { return QJsonValue(this->stringToArray(lineEdit->text())); };
                connect(lineEdit, &QLineEdit::textChanged, [this, inputId](){ this->valueChanged(inputId); });
                layout->addWidget(lineEdit);

                if(inputType == "File")
                {
                    QPushButton *pushButton = new QPushButton("Select files");

                    connect(pushButton, &QPushButton::clicked, [this, inputName, lineEdit]() { lineEdit->setText("\"" + QFileDialog::getOpenFileNames(this, "Select " + inputName).join("\", \"") + "\""); } );
                    layout->addWidget(pushButton);

                    QPushButton *SetCurrentInputPushButton = new QPushButton("Add input");
                    connect(SetCurrentInputPushButton, &QPushButton::clicked, [this, lineEdit]()
                    {
                        QString text = lineEdit->text();
                        text += text.length() > 0 ? ", " : "";
                        const QString &currentInputFilePath = this->fileHandler->createTemporaryInputFileForCurrentInput();
                        if(!currentInputFilePath.isEmpty())
                        {
                            lineEdit->setText(text + "\"" + currentInputFilePath + "\"");
                        }
                    } );

                    layout->addWidget(SetCurrentInputPushButton);
                    connect(static_cast<medBoutiquesDropWidget*>(widget), &medBoutiquesDropWidget::dragEnter, [this](QDragEnterEvent *event) { this->fileHandler->checkAcceptDragEvent(event); });
                    connect(static_cast<medBoutiquesDropWidget*>(widget), &medBoutiquesDropWidget::drop, [this, lineEdit](QDropEvent *event)
                    {
                        const QMimeData *mimeData = event->mimeData();
                        QStringList paths;
                        if(mimeData->hasUrls())
                        {
                            const QList<QUrl> &urls= mimeData->urls();
                            for (int i = 0 ; i < urls.size() ; ++i)
                            {
                                paths.append(urls.at(i).toLocalFile());
                            }
                        }
                        else
                        {
                            QString filePath = this->fileHandler->createTemporaryInputFileForMimeData(mimeData);
                            if(!filePath.isEmpty())
                            {
                                paths.append(filePath);
                            }
                        }

                        QString text = lineEdit->text();
                        for (int i = 0 ; i < paths.size() ; ++i)
                        {
                            text += text.length() > 0 ? ", " : "";
                            text += "\"" + paths[i] + "\"";
                        }
                        lineEdit->setText(text);
                    });
                }
            }
            else
            {
                if(     (inputType == "String" || inputType == "Number") &&
                        inputObject.description.contains("value-choices") &&
                        inputObject.description["value-choices"].isArray()  &&
                        inputObject.description["value-choices"].toArray().size() > 0)
                {
                    const QJsonArray &choices = inputObject.description["value-choices"].toArray();
                    QComboBox *comboBox = new QComboBox();
                    layout->addWidget(comboBox);
                    bool isInt = inputObject.description["integer"].toBool();
                    connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, inputId](){ this->valueChanged(inputId); });
                    inputObject.getValue = [comboBox, inputType, isInt]()
                    {
                        const QString &text = comboBox->currentText();
                        return inputType == "String" ? QJsonValue(text) : isInt ? QJsonValue(text.toInt()) : QJsonValue(text.toDouble());
                    };
                    for(const auto &choice: choices)
                    {
                        const QString &value = inputType == "String" ? choice.toString() : QString::number( isInt ? choice.toInt() : choice.toDouble());
                        comboBox->addItem(value);
                    }
                }
                else if(inputType == "String")
                {
                    QLineEdit *lineEdit = new QLineEdit();
                    lineEdit->setPlaceholderText(inputDescription);
                    lineEdit->setText(inputValue.toString());

                    inputObject.getValue = [lineEdit]() { return lineEdit->text(); };
                    connect(lineEdit, &QLineEdit::textChanged, [this, inputId](){ this->valueChanged(inputId); });
                    layout->addWidget(lineEdit);
                }
                else if(inputType == "Number")
                {
                    if(inputObject.description["integer"].toBool())
                    {
                        QSpinBox *spinBox = new QSpinBox();
                        if(inputObject.description["minimum"].isDouble()) {
                            spinBox->setMinimum(static_cast<int>(inputObject.description["minimum"].toDouble()));
                        }
                        if(inputObject.description["exclusive-minimum"].isDouble()) {
                            spinBox->setMinimum(static_cast<int>(inputObject.description["exclusive-minimum"].toDouble()) + 1);
                        }
                        if(inputObject.description["maximum"].isDouble()) {
                            spinBox->setMaximum(static_cast<int>(inputObject.description["maximum"].toDouble()));
                        }
                        if(inputObject.description["exclusive-maximum"].isDouble()) {
                            spinBox->setMaximum(static_cast<int>(inputObject.description["exclusive-maximum"].toDouble()) - 1);
                        }
                        spinBox->setValue(static_cast<int>(inputValue.toDouble()));
                        inputObject.getValue = [spinBox](){ return QJsonValue(spinBox->value()); };
                        connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this, inputId](){ this->valueChanged(inputId); });
                        layout->addWidget(spinBox);
                    }
                    else
                    {
                        QDoubleSpinBox *spinBox = new QDoubleSpinBox();
                        if(inputObject.description["minimum"].isDouble()) {
                            spinBox->setMinimum(inputObject.description["minimum"].toDouble());
                        }
                        if(inputObject.description["exclusive-minimum"].isDouble()) {
                            spinBox->setMinimum(inputObject.description["exclusive-minimum"].toDouble() + 0.0001);
                        }
                        if(inputObject.description["maximum"].isDouble()) {
                            spinBox->setMaximum(inputObject.description["maximum"].toDouble());
                        }
                        if(inputObject.description["exclusive-maximum"].isDouble()) {
                            spinBox->setMaximum(inputObject.description["exclusive-maximum"].toDouble() - 0.0001);
                        }
                        spinBox->setValue(inputValue.toDouble());
                        spinBox->setDecimals(4);
                        inputObject.getValue = [spinBox](){ return QJsonValue(spinBox->value()); };
                        connect(spinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, inputId](){ this->valueChanged(inputId); });
                        layout->addWidget(spinBox);
                    }
                }
                else if(inputType == "File")
                {
                    QLineEdit *lineEdit = new QLineEdit();
                    lineEdit->setPlaceholderText(inputDescription);
                    lineEdit->setText(inputValue.toString());

                    inputObject.getValue = [lineEdit]() { return lineEdit->text(); };
                    connect(lineEdit, &QLineEdit::textChanged, [this, inputId](){ this->valueChanged(inputId); });
                    layout->addWidget(lineEdit);

                    QPushButton *selectFilePushButton = new QPushButton("Select file");
                    connect(selectFilePushButton, &QPushButton::clicked, [this, inputName, lineEdit]() { lineEdit->setText(QFileDialog::getOpenFileName(this, "Select " + inputName)); } );
                    layout->addWidget(selectFilePushButton);

                    QPushButton *setInputPushButton = new QPushButton("Set input");
                    connect(setInputPushButton, &QPushButton::clicked, [this, lineEdit]() { lineEdit->setText(this->fileHandler->createTemporaryInputFileForCurrentInput()); } );
                    layout->addWidget(setInputPushButton);

                    widget->setAcceptDrops(true);

                    connect(static_cast<medBoutiquesDropWidget*>(widget), &medBoutiquesDropWidget::dragEnter, [this](QDragEnterEvent *event) { this->fileHandler->checkAcceptDragEvent(event); });
                    connect(static_cast<medBoutiquesDropWidget*>(widget), &medBoutiquesDropWidget::drop, [this, lineEdit](QDropEvent *event)
                    {
                        const QMimeData *mimeData = event->mimeData();
                        QString filePath;
                        if(mimeData->hasUrls())
                        {
                            filePath = mimeData->urls().first().toLocalFile();
                        }
                        else
                        {
                            filePath = this->fileHandler->createTemporaryInputFileForMimeData(mimeData);
                        }
                        if(!filePath.isEmpty())
                        {
                            lineEdit->setText(filePath);
                        }
                    });
                }

                if(inputType == "String")
                {
                    bool foundCorrespondingOutput = false;
                    const QString &inputValueKey = inputObject.description["value-key"].toString();
                    for (int i = 0 ; i<this->outputFiles.size() ; ++i)
                    {
                        const QJsonObject &outputFilesDescription = this->outputFiles[i].toObject();
                        const QString &pathTemplate = outputFilesDescription["path-template"].toString();
                        if(pathTemplate.contains(inputValueKey) && this->fileHandler->hasKnownExtension(pathTemplate))
                        {
                            if(this->selectedOutputId.isEmpty())
                            {
                                this->selectedOutputId = outputFilesDescription["id"].toString();
                            }
                            foundCorrespondingOutput = true;
                            break;
                        }
                    }
                    if(foundCorrespondingOutput)
                    {
                        QPushButton *toggleOutputPushButton = new QPushButton("Set output");
                        connect(toggleOutputPushButton, &QPushButton::clicked, [this, toggleOutputPushButton, inputId]() { this->toggleOutputFile(toggleOutputPushButton, inputId); } );
                        layout->addWidget(toggleOutputPushButton);
                    }
                }
            }
            widget->setToolTip(inputDescription);
            widget->setLayout(layout);
        }

        if(inputType == "Flag")
        {
            if(!inputGroupIsMutuallyExclusive)
            {
                QCheckBox *checkBox = new QCheckBox(inputName);
                checkBox->setCheckState(inputValue.toBool() ? Qt::Checked : Qt::Unchecked);

                inputObject.getValue = [checkBox](){ return QJsonValue(checkBox->isChecked()); };
                connect(checkBox, &QCheckBox::stateChanged, [this, inputId](){ this->valueChanged(inputId); });
                checkBox->setToolTip(inputDescription);
                widget = checkBox;
            }
        }
        bool inputCanBeUnset = !inputGroupIsMutuallyExclusive && inputIsOptional;
        bool isFlagInMutuallyExclusiveGroup = inputType == "Flag" && inputGroupIsMutuallyExclusive;

        if(inputCanBeUnset)
        {
            widget = this->createUnsetGroup(inputId, widget);
        }

        if(!isFlagInMutuallyExclusiveGroup)     // Then we must add it
        {
            parentLayout->addWidget(widget);
            inputObject.widget = widget;
        }

        if(inputGroupIsMutuallyExclusive && inputValue.isNull() && widget != nullptr)
        {
            widget->hide();
        }
    }
    for(auto& destinationLayout: destinationLayouts)
    {
        destinationLayout.second->addWidget(destinationLayout.first);
    }
    groupLayout->addWidget(mainInputsGroupAndLayout.first);
    groupLayout->addWidget(optionalInputsGroupAndLayout.first);
    groupLayout->addStretch();
    if(noOptionalInput)
    {
        optionalInputGroup->hide();
    }

    this->emitInvocationChanged();
}

bool medBoutiquesInvocationGUIWidget::generateCompleteInvocation()
{
    return this->optionalInputGroup != nullptr && this->optionalInputGroup->isChecked();
}

void medBoutiquesInvocationGUIWidget::populateDirectoriesAndSetOutputFileName(QJsonObject &invocationJSON, QStringList &directories)
{
    QDir::setCurrent(QDir::homePath());

    bool hasChangedCurrentDirectory = this->selectCurrentDirectoryGroupBox->isChecked();
    if(hasChangedCurrentDirectory)
    {
        const QString &newCurrentPath = this->selectCurrentDirectoryLineEdit->text();
        if(QDir(newCurrentPath).exists())
        {
            QDir::setCurrent(newCurrentPath);
        }
        else
        {
            QMessageBox::warning(this, "Could not set current working directory", "Could not set current working directory to \"" + newCurrentPath + "\".\n\nThe current directory is \"" + QDir::currentPath() + "\".\n\nYou will be asked to choose another one if needed.");
            hasChangedCurrentDirectory = false;
        }
    }
    this->populateInputDirectories(invocationJSON, directories, hasChangedCurrentDirectory);
    this->populateOutputDirectoriesAndSetOutputFileName(invocationJSON, directories, hasChangedCurrentDirectory);

}

QString medBoutiquesInvocationGUIWidget::getOutputFileName()
{
    return this->outputFileName;
}

void medBoutiquesInvocationGUIWidget::askChangeCurrentDirectory()
{
    QString currentPath = QDir::currentPath();
    QMessageBox messageBox;
    messageBox.setText("One or more file paths are relative");
    messageBox.setInformativeText("One or more file paths are relative, do you want to change the current working directory?\n\nThe current working directory is \"" + currentPath + "\".");
    messageBox.setStandardButtons(QMessageBox::Ok | QMessageBox::No);
    messageBox.setDefaultButton(QMessageBox::Ok);
    int returnCode = messageBox.exec();
    if(returnCode == QMessageBox::Ok)
    {
        const QString &newCurrentPath = QFileDialog::getExistingDirectory(this, tr("Select root directory"), QDir::home().absolutePath(), QFileDialog::ShowDirsOnly);
        QDir::setCurrent(newCurrentPath);
    }
}

void medBoutiquesInvocationGUIWidget::populateAbsolutePath(const QJsonValue &fileNameValue, QStringList &directories, bool &hasChangedCurrentDirectory)
{
//    QString fileName = fileNameValue.toString();
//    if(fileName == "[CURRENT INPUT]")
//    {

//    }
    QFileInfo fileInfo(fileNameValue.toString());
    if(fileInfo.isRelative())
    {
        if(!hasChangedCurrentDirectory)
        {
            this->askChangeCurrentDirectory();
            hasChangedCurrentDirectory = true;
        }
        return;
    }
    const QString &path = fileInfo.absolutePath();
    if(!directories.contains(path))
    {
        directories.append(path);
    }
}

void medBoutiquesInvocationGUIWidget::populateInputDirectories(const QJsonObject &invocationJSON, QStringList &directories, bool &hasChangedCurrentDirectory)
{
   for(auto input = invocationJSON.begin() ; input != invocationJSON.end() ; input++)
   {
       const QString &inputId = input.key();
       const QJsonValue &value = input.value();
       auto it = this->idToInputObject.find(inputId);
       if(it == this->idToInputObject.end())
       {
           continue;
       }
       const InputObject &inputObject = it->second;
       if(inputObject.description["type"].toString() == "File")
       {
           if(inputObject.description["list"].toBool())
           {
               const QJsonArray &paths = value.toArray();
               for(auto path: paths)
               {
                    this->populateAbsolutePath(path, directories, hasChangedCurrentDirectory);
               }
           }
           else
           {
               this->populateAbsolutePath(value, directories, hasChangedCurrentDirectory);
           }
       }
   }
}

void medBoutiquesInvocationGUIWidget::populateOutputDirectoriesAndSetOutputFileName(const QJsonObject &invocationJSON, QStringList &directories, bool &hasChangedCurrentDirectory)
{
    for (auto& idAndInputObject: idToInputObject)
    {
        const QString &inputId = idAndInputObject.first;
        InputObject &inputObject = idAndInputObject.second;
        const QString &inputType = inputObject.description["type"].toString();

        if((inputType == "File" || inputType == "String") && invocationJSON.contains(inputId))
        {
            QString fileName = invocationJSON[inputId].toString();
            for (int i = 0 ; i<this->outputFiles.size() ; ++i)
            {
                const QJsonObject &outputFilesDescription = this->outputFiles[i].toObject();
                QString pathTemplate = outputFilesDescription["path-template"].toString();
                if(inputType == "File" && outputFilesDescription.contains("path-template-stripped-extensions"))
                {
                    const QJsonArray &pathTemplateStrippedExtensions = outputFilesDescription["path-template-stripped-extensions"].toArray();

                    for (int j = 0 ; j<pathTemplateStrippedExtensions.size() ; ++j)
                    {
                        const QString &pathTemplateStrippedExtension = pathTemplateStrippedExtensions[j].toString();
                        fileName.remove(pathTemplateStrippedExtension);
                    }
                }
                const QString &valueKey = inputObject.description["value-key"].toString();
                if(pathTemplate.contains(valueKey))
                {
                    pathTemplate.replace(valueKey, fileName);

                    QFileInfo fileInfo(pathTemplate);
                    if(fileInfo.isRelative())
                    {
                        if(!hasChangedCurrentDirectory)
                        {
                            this->askChangeCurrentDirectory();
                            hasChangedCurrentDirectory = true;
                        }
                    }
                    else
                    {
                        const QString &absolutePath = fileInfo.absolutePath();
                        if(!directories.contains(absolutePath))
                        {
                            directories.append(absolutePath);
                        }
                    }
                    if(outputFilesDescription["id"].toString() == this->selectedOutputId)
                    {
                        this->outputFileName = fileInfo.absoluteFilePath();
                    }
                }
            }
        }
    }
}

void medBoutiquesInvocationGUIWidget::toggleOutputFile(QPushButton *toggleOutputPushButton, const QString &inputId)
{
    if(toggleOutputPushButton->text() == "Unset output")
    {
        this->selectedOutputId.clear();
        toggleOutputPushButton->setText("Set output");
        return;
    }
    const InputObject &inputObject = this->idToInputObject.at(inputId);
    const QString &inputValueKey = inputObject.description["value-key"].toString();

    struct OutputFile {
        QString name;
        QString description;
        QString id;
        OutputFile(const QString &name, const QString &description, const QString &id): name(name), description(description), id(id) {}
    };

    vector<OutputFile> outputFiles;
    for (int i = 0 ; i<this->outputFiles.size() ; ++i)
    {
        const QJsonObject &outputFilesDescription = this->outputFiles[i].toObject();
        const QString &pathTemplate = outputFilesDescription["path-template"].toString();
        if(pathTemplate.contains(inputValueKey) && this->fileHandler->hasKnownExtension(pathTemplate))
        {
            const QString &outputName = outputFilesDescription["name"].toString();
            const QString &outputDescription = outputFilesDescription["description"].toString();
            outputFiles.push_back(OutputFile(outputName + "(" + pathTemplate + ")", outputDescription, outputFilesDescription["id"].toString()));
        }
    }

    if(outputFiles.size() <= 0)
    {
        return;
    }
    else if (outputFiles.size() == 1)
    {
        this->selectedOutputId = outputFiles[0].id;
        toggleOutputPushButton->setText("Unset output");
    }
    else
    {
        QDialog messageBox;

        QVBoxLayout *messageBoxLayout = new QVBoxLayout();
        messageBox.setLayout(messageBoxLayout);

        QLabel *title = new QLabel("<b>Please select an output file type</b>");
        QLabel *message = new QLabel("There are multiple output files associated with this input string, please select the one which will be displayed as output.");
        QLabel *descriptionLabel = new QLabel("");
        QPushButton *okButton = new QPushButton("Ok");

        connect(okButton, &QPushButton::clicked, [&messageBox]() { messageBox.close(); } );

        messageBoxLayout->addWidget(title);
        messageBoxLayout->addWidget(message);

        QComboBox* typeComboBox = new QComboBox();

        for(const auto &outputFile : outputFiles)
        {
            typeComboBox->addItem(outputFile.name, outputFile.description);
            typeComboBox->setItemData(typeComboBox->count() - 1, outputFile.id, Qt::UserRole + 1);
        }
        connect(typeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [typeComboBox, descriptionLabel](int itemIndex) { descriptionLabel->setText(typeComboBox->itemData(itemIndex).toString()); } );

        messageBoxLayout->addWidget(typeComboBox);
        messageBoxLayout->addWidget(descriptionLabel);
        messageBoxLayout->addWidget(okButton);

        messageBox.exec();

        this->selectedOutputId = typeComboBox->itemData(typeComboBox->currentIndex(), Qt::UserRole + 1).toString();
        toggleOutputPushButton->setText("Unset output");
    }
}
