#include <iostream>
#include <QtWidgets>
#include "medBoutiquesInvocationGUIWidget.h"

medBoutiquesInvocationGUIWidget::medBoutiquesInvocationGUIWidget(QWidget *parent, medBoutiquesSearchToolsWidget *searchToolsWidget, medBoutiquesAbstractFileHandler *medBoutiquesFileHandler) :
    QWidget(parent),
    searchToolsWidget(searchToolsWidget),
    fileHandler(medBoutiquesFileHandler),
    optionalInputGroup(nullptr),
    completeInvocationJSON(nullptr)
{
    // Create the main layout and scroll area
    this->layout = new QVBoxLayout(this);
    this->setMinimumHeight(750);
//    this->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::MinimumExpanding);
    this->scrollArea = new QScrollArea(this);
//    this->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    this->scrollArea->setWidgetResizable(true);
    this->scrollArea->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);

    // Create a GUI to select the working directory (from which relative paths will be defined)
    this->createSelectCurrentDirectoryGUI();
    this->layout->addWidget(this->scrollArea);
    this->group = nullptr;
    this->setLayout(this->layout);

    // Create the timer to emit invocationChanged() with a delay when the user changes parameters
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
    // The GUI to select the current directory is composed of a checkable QGroupBox containing a QLineEdit to enter a path, and a QPushButton to select the path form a file dialog
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

inline QJsonArray medBoutiquesInvocationGUIWidget::stringToArray(const QString &string)
{
    QJsonDocument jsonDocument(QJsonDocument::fromJson(QByteArray::fromStdString("[" + string.toStdString() + "]")));
    return jsonDocument.array();
}

void medBoutiquesInvocationGUIWidget::valueChanged(const QString &inputId)
{
    // Called when a parameter changes in the invocation GUI
    // updates invocationJSON and emit invocationChanged() with a delay
    const InputObject &inputObject = this->idToInputObject.at(inputId);
    const QJsonValue &value = inputObject.getValue();
    bool inputIsList = inputObject.description["list"].toBool();
    bool inputIsListButValueIsInvalidOrEmpty = inputIsList && (!value.isArray() || value.toArray().isEmpty());
    // Invalid or empty List parameters must be removed from the invocation file
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
        //  - store the optional values to be able to restore them later (when re-checking optional parameters) in completeInvocationJSON
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
    // Create a group box and its layout
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

    // Remove all group members from invocationJSON and hide the parameter widget if any (= if the parameter is not of type "Flag")
    const auto &members = groupObject->description["members"].toArray();
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

    // Insert the value of the newly selected group member in invocationJSON
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

    // Emit invocationChanged with a delay
    this->emitInvocationChanged();
}

void medBoutiquesInvocationGUIWidget::emitInvocationChanged()
{
    // Emit invocationChanged with a delay
    this->emitInvocationChangedTimer->stop();
    this->emitInvocationChangedTimer->start(250);
}

QWidget *medBoutiquesInvocationGUIWidget::createUnsetGroup(const QString &inputId, QWidget *widget)
{
    // Create an unset group (a widget with a horizontal layout, which will contain the parameters widget, and an "Unset" push button)
    // The "Unset" button is added to all optional parameters, to unset them
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
        // If there is an existing GUI: destroy it and recall parseDescriptor() once it is deleted
        this->emitInvocationChangedTimer->stop();
        connect(this->group, &QWidget::destroyed, [this](){ this->parseDescriptor(this->invocationJSON); });
        this->scrollArea->takeWidget();
        this->group->deleteLater();
        this->group = nullptr;
        return;
    }
    // Here we are sure that there is no invocation GUI anymore

    // Create a new group widget, child of the scroll area, which will contain the entire invocation GUI
    this->group = new QWidget(this->scrollArea);

    QVBoxLayout *groupLayout = new QVBoxLayout(this->group);
    this->group->setLayout(groupLayout);

    // Get the tool descriptor
    const QJsonObject descriptor = this->searchToolsWidget->getSelectedToolDescriptor();

    if(descriptor.isEmpty())
    {
        // If the descriptor is empty: there was a problem while opening the zenodo-TOOL_ID.json file: just give up
        return;
    }

    // Create main inputs and optional inputs groups (the optional inputs group will be hidden if it is not required)
    auto mainInputsGroupAndLayout = this->createGroupAndLayout("Required parameters");
    auto optionalInputsGroupAndLayout = this->createGroupAndLayout("Optional parameters");

    // Optional inputs group can be disabled, only the mandatory parameters will be used and BoutiquesGUI will generate a minimal invocation file
    this->optionalInputGroup = optionalInputsGroupAndLayout.first;
    this->optionalInputGroup->setCheckable(true);
    connect(this->optionalInputGroup, &QGroupBox::toggled, this, &medBoutiquesInvocationGUIWidget::optionalGroupChanged);

    if ( !(descriptor.contains("inputs") && descriptor["inputs"].isArray() && (!descriptor.contains("groups") || descriptor["groups"].isArray()) ) )
    {
        QMessageBox::critical(this, "Error in descriptor file", "No inputs or groups.");
        return;
    }

    this->idToInputObject.clear();
    this->selectedOutputId.clear();

    // Populate idToInputObject from the descriptor inputs
    QJsonArray inputArray = descriptor["inputs"].toArray();
    for (int i = 0 ; i<inputArray.size() ; ++i)
    {
        const QJsonObject &description = inputArray[i].toObject();
        const auto &result = this->idToInputObject.emplace(pair<QString, InputObject>(description["id"].toString(), InputObject(description)));
        // result.first is an iterator: first: id, second: inputObject
        // result.second tells if emplace was successfull
        if(!result.second)
        {
            // If emplace() replaced the value at the key "description["id"]": warn the user of the error in the descriptor
            QMessageBox::warning(this, "Error in descriptor file", "Input " + result.first->first + "appears twice in descriptor, only the first one will be considered...");
        }
        else
        {
            result.first->second.description = description;
        }
    }

    this->outputFiles = descriptor["output-files"].toArray();
    this->groupObjects.clear();

    // Create input groups
    // For each group:
    // - create the GroupObject (containing the widget, layout and if it is optional)
    // - create a QGroupBox and its layout
    // - check if group is optional (group is optional if one of its member is optional)
    // - add the group widget to the layout accordingly (main inputs or optional inputs)
    vector<pair<QGroupBox*, QVBoxLayout*>> destinationLayouts;
    QJsonArray groupArray = descriptor["groups"].toArray();
    this->groupObjects.reserve(static_cast<size_t>(groupArray.size()));
    for (int i = 0 ; i<groupArray.size() ; ++i)
    {
        // Create the GroupObject
        this->groupObjects.emplace_back();
        GroupObject &groupObject = this->groupObjects.back();
        groupObject.description = groupArray[i].toObject();
        if ( !(groupObject.description.contains("members") && groupObject.description["members"].isArray()) )
        {
            continue;
        }
        // Create a QGroupBox and its layout
        auto groupAndLayout = this->createGroupAndLayout(groupObject.description["name"].toString());
        groupObject.groupBox = groupAndLayout.first;
        groupObject.layout = groupAndLayout.second;
        // Check if the group is optional (group is optional if one of its member is optional)
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
            const QJsonValue &isOptional = inputObject.description["optional"];
            if(isOptional.isNull() || (isOptional.isBool() && !isOptional.toBool()) )
            {
                groupIsOptional = false;
            }
        }
        groupObject.optional = groupIsOptional;
        // Add the group widget to the layout accordingly (main inputs or optional inputs)
        destinationLayouts.push_back(pair<QGroupBox*, QVBoxLayout*>(groupAndLayout.first, groupIsOptional ? optionalInputsGroupAndLayout.second : mainInputsGroupAndLayout.second));
    }

    bool noOptionalInput = true;        // Hide the optional inputs group if there are no optional inputs

    // Create the parameter widgets according to the descriptor
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

        // Determine the widget parent: groupObject (if input is part of a group), optional inputs group or main inputs group
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

        // Mutually exclusive groups are controlled with a comboBox (drop down menu) containing the group parameters (the user must choose one parameter among the group members)
        // Flag parameters have no additional widget
        // Other parameters have an additional widget (A QLineEdit for strings, QLineEdit + QPushButton for files, etc.)
        // The widgets of the other parameters are all hidden, except the one which is selected (if any)
        bool inputGroupIsMutuallyExclusive = this->inputGroupIsMutuallyExclusive(inputId);
        if(inputGroupIsMutuallyExclusive)
        {
            if(groupObject->comboBox == nullptr)
            {
                // If the mutually exclusive group is not yet created: create the combo box
                QComboBox *comboBox = new QComboBox();
                groupObject->comboBox = comboBox;
                parentLayout->addWidget(comboBox);
                connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, groupObject](int itemIndex) { this->mutuallyExclusiveGroupChanged(groupObject, itemIndex); } );
                if(groupObject->optional)
                {
                    // If the group is optional: the group box is checkable to unset the parameter in invocationJSON
                    groupObject->groupBox->setCheckable(true);
                    connect(groupObject->groupBox, &QGroupBox::toggled, this, [this, groupObject](bool on)
                    {
                        // Add or remove the value in invocationJSON, and emit invocationChanged with a delay
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
            // If the invocationJSON (describing the current invocation values) has a value for this parameter: set the combo box to this value
            if(!inputValue.isNull())
            {
                groupObject->comboBox->setCurrentIndex(groupObject->comboBox->count() - 1);
            }
        }

        // If the input parameter is a String, a Number of a File: create a horizontal group which contains the label and the input field (each type will differ slightly)
        if(inputType == "String" || inputType == "Number" || inputType == "File")
        {
            widget = inputType == "File" ? new medBoutiquesDropWidget() : new QWidget();        // File input are droppable, meaning the user can drop a widget on them (use in medInria to drop data taken from the database)
            QHBoxLayout *layout = new QHBoxLayout();
            QLabel *label = new QLabel(inputName + ":");
            layout->addWidget(label);

            // If the parameter is a list: the corresponding widget will be a QLineEdit to enter comma separated values
            if(inputObject.description["list"].toBool())
            {
                QLineEdit *lineEdit = new QLineEdit();
                lineEdit->setPlaceholderText(QString("Comma separated ") + (inputType == "String" ? "strings" : inputType == "Number" ? "numbers" : "file paths") + ".");
                QString listString;

                // Set the lineEdit to the proper input value (from invocationJSON)
                QJsonDocument valueList;
                valueList.setArray(inputValue.toArray());
                lineEdit->setText(QString::fromUtf8(valueList.toJson()).remove('[').remove(']'));

                // The value of the parameter is the content of the line edit (it should be comma separated values) enclosed in brackets
                inputObject.getValue = [this, lineEdit]() { return QJsonValue(this->stringToArray(lineEdit->text())); };
                connect(lineEdit, &QLineEdit::textChanged, [this, inputId](){ this->valueChanged(inputId); });
                layout->addWidget(lineEdit);

                if(inputType == "File")
                {
                    // If input is a list of files: a "Select files" push button opens a dialog to select multiple files
                    QPushButton *pushButton = new QPushButton("Select files");

                    connect(pushButton, &QPushButton::clicked, [this, inputName, lineEdit]()
                    {
                        lineEdit->setText("\"" + this->fileHandler->normalizePaths( QFileDialog::getOpenFileNames(this, "Select " + inputName) ).join("\", \"") + "\"");
                    });

                    layout->addWidget(pushButton);

                    // Create the button to add the current input data to the list (save the current input data to a temporary file, and add the resulting path)
                    QPushButton *setCurrentInputPushButton = new QPushButton("Add input");
                    connect(setCurrentInputPushButton, &QPushButton::clicked, [this, lineEdit]()
                    {
                        QString text = lineEdit->text();
                        text += text.length() > 0 ? ", " : "";
                        const QString &currentInputFilePath = this->fileHandler->createTemporaryInputFileForCurrentInput();
                        if(!currentInputFilePath.isEmpty())
                        {
                            lineEdit->setText(text + "\"" + currentInputFilePath + "\"");
                        }
                    } );

                    // Create the drag and drop callbacks:
                    // On drop: add the current dragged objects to the list
                    //  - if the objects are files: just add the file paths to the list,
                    //  - if the objects are data from the database: save the data to temporary files, and add the resulting paths.
                    layout->addWidget(setCurrentInputPushButton);
                    connect(static_cast<medBoutiquesDropWidget*>(widget), &medBoutiquesDropWidget::dragEnter, [this](QDragEnterEvent *event) { this->fileHandler->checkAcceptDragEvent(event); });
                    connect(static_cast<medBoutiquesDropWidget*>(widget), &medBoutiquesDropWidget::drop, [this, lineEdit](QDropEvent *event)
                    {
                        const QMimeData *mimeData = event->mimeData();
                        QStringList paths;
                        // On drop: add the current object to the list
                        if(mimeData->hasUrls())
                        {
                            // if the objects are files: just add the file paths to the list
                            const QList<QUrl> &urls = mimeData->urls();
                            for (int i = 0 ; i < urls.size() ; ++i)
                            {
                                paths.append(urls.at(i).toLocalFile());
                            }
                        }
                        else
                        {
                            // if the objects are data from the database: save the current input data to temporary files, and add the resulting paths.
                            QString filePath = this->fileHandler->createTemporaryInputFileForMimeData(mimeData);
                            if(!filePath.isEmpty())
                            {
                                paths.append(filePath);
                            }
                        }

                        // Add the comma separated paths to the list in the line edit
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
                // Parameter is not a list

                if(     (inputType == "String" || inputType == "Number") &&
                        inputObject.description.contains("value-choices") &&
                        inputObject.description["value-choices"].isArray()  &&
                        inputObject.description["value-choices"].toArray().size() > 0)
                {
                    // If the parameter has "value-choices": create a combo box to be able to select one of those choices
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
                    // If type is String: just create a line edit
                    QLineEdit *lineEdit = new QLineEdit();
                    lineEdit->setPlaceholderText(inputDescription);
                    lineEdit->setText(inputValue.toString());

                    inputObject.getValue = [lineEdit]() { return lineEdit->text(); };
                    connect(lineEdit, &QLineEdit::textChanged, [this, inputId](){ this->valueChanged(inputId); });
                    layout->addWidget(lineEdit);
                }
                else if(inputType == "Number")
                {
                    // If type is Number: create a spin box and set min, max and exclusive min and max
                    // Integer must be handled separately from double (eventhough the code is almost the same) because QDoubleSpinBox != QSpinBox, toInt() != toDouble(), etc.
                    if(inputObject.description["integer"].toBool())
                    {
                        // Set the minimum and maximum, set value and connect to valueChanged signal
                        QSpinBox *spinBox = new QSpinBox();
                        if(inputObject.description["minimum"].isDouble())
                        {
                            int exclusiveOffset = inputObject.description["exclusive-minimum"].toBool() ? 1 : 0;
                            spinBox->setMinimum(inputObject.description["minimum"].toInt() + exclusiveOffset);
                        }
                        if(inputObject.description["maximum"].isDouble())
                        {
                            int exclusiveOffset = inputObject.description["exclusive-maximum"].toBool() ? 1 : 0;
                            spinBox->setMaximum(inputObject.description["maximum"].toInt() - exclusiveOffset);
                        }
                        spinBox->setValue(inputValue.toInt());
                        inputObject.getValue = [spinBox](){ return QJsonValue(spinBox->value()); };
                        connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this, inputId](){ this->valueChanged(inputId); });
                        layout->addWidget(spinBox);
                    }
                    else
                    {
                        // Set the minimum and maximum, set value and connect to valueChanged signal
                        QDoubleSpinBox *spinBox = new QDoubleSpinBox();
                        if(inputObject.description["minimum"].isDouble())
                        {
                            double exclusiveOffset = inputObject.description["exclusive-minimum"].toBool() ? 0.0001 : 0.0;
                            spinBox->setMinimum(inputObject.description["minimum"].toDouble() + exclusiveOffset);
                        }
                        if(inputObject.description["maximum"].isDouble())
                        {
                            double exclusiveOffset = inputObject.description["exclusive-maximum"].toBool() ? 0.0001 : 0.0;
                            spinBox->setMaximum(inputObject.description["maximum"].toDouble() - exclusiveOffset);
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
                    // If parameter is File: create a line edit (for the file path), a "Select file" button, and a "Set input" button
                    QLineEdit *lineEdit = new QLineEdit();
                    lineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
                    lineEdit->setPlaceholderText(inputDescription);
                    lineEdit->setText(inputValue.toString());

                    inputObject.getValue = [lineEdit]() { return lineEdit->text(); };
                    connect(lineEdit, &QLineEdit::textChanged, [this, inputId](){ this->valueChanged(inputId); });
                    layout->addWidget(lineEdit);

                    QPushButton *selectFilePushButton = new QPushButton("Select file");
                    connect(selectFilePushButton, &QPushButton::clicked, [this, inputName, lineEdit]()
                    {
                        lineEdit->setText(this->fileHandler->normalizePath(QFileDialog::getOpenFileName(this, "Select " + inputName)));
                    } );
                    layout->addWidget(selectFilePushButton);

                    QPushButton *setInputPushButton = new QPushButton("Set input");
                    connect(setInputPushButton, &QPushButton::clicked, [this, lineEdit]() { lineEdit->setText(this->fileHandler->createTemporaryInputFileForCurrentInput()); } );
                    layout->addWidget(setInputPushButton);

                    widget->setAcceptDrops(true);

                    // Handle the drop on the widget: just set the line edit to the file path of the dropped object (create a file if necessary)
                    connect(static_cast<medBoutiquesDropWidget*>(widget), &medBoutiquesDropWidget::dragEnter, [this](QDragEnterEvent *event) { this->fileHandler->checkAcceptDragEvent(event); });
                    connect(static_cast<medBoutiquesDropWidget*>(widget), &medBoutiquesDropWidget::drop, [this, lineEdit](QDropEvent *event)
                    {
                        const QMimeData *mimeData = event->mimeData();
                        QString filePath;

                        if(mimeData->hasUrls())
                        {
                            // If dropping a file on the widget: just set the line edit to the file path
                            filePath = mimeData->urls().first().toLocalFile();
                        }
                        else
                        {
                            // If dropping some data on the widget: create a temporary file from the data and set the line edit to the resulting file path
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
                    // If input is a string: check if there is a corresponding output file,
                    // if there is one: create a "Set output" button to automatically open the output file in medInria when the process ends

                    // Check if there is a corresponding output file (if the "value-key" of the parameter exists in the "path-template" values of the "output-files" in the descriptor)
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
                        // Create the "Set output" button
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
            // If the parameter is a Flag: just create a checkbox
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
        // Input can be unset if it is optional but not from a mutually exclusive group (in the latter case, the group box can be uncheck to unset the parameters)
        bool inputCanBeUnset = !inputGroupIsMutuallyExclusive && inputIsOptional;

        if(inputCanBeUnset)
        {
            // Add the "Unset" button at the end of the parameter widget (to remove the parameter from the generated invocation)
            widget = this->createUnsetGroup(inputId, widget);
        }

        // The InputObject.widget will be used to show() / hide() widgets in a mutually exclusive group: ignore Flag parameters from mutually exclusive groups
        bool isFlagInMutuallyExclusiveGroup = inputType == "Flag" && inputGroupIsMutuallyExclusive;
        if(!isFlagInMutuallyExclusiveGroup)     // Then we must add it
        {
            parentLayout->addWidget(widget);
            inputObject.widget = widget;
        }

        // Hide widget from mutually exclusive group which are not declared in the current invocationJSON
        if(inputGroupIsMutuallyExclusive && inputValue.isNull() && widget != nullptr)
        {
            widget->hide();
        }
    }
    // Add widgets in layouts
    for(auto& destinationLayout: destinationLayouts)
    {
        destinationLayout.second->addWidget(destinationLayout.first);
    }
    groupLayout->addWidget(mainInputsGroupAndLayout.first);
    groupLayout->addWidget(optionalInputsGroupAndLayout.first);
    groupLayout->addStretch();
    if(noOptionalInput) // Hide optional inputs group if there are no optional inputs
    {
        optionalInputGroup->hide();
    }

    this->scrollArea->setWidget(this->group);

    // emit invocation changed with a delay
    this->emitInvocationChanged();
}

bool medBoutiquesInvocationGUIWidget::mustGenerateCompleteInvocation()
{
    return this->optionalInputGroup != nullptr && this->optionalInputGroup->isChecked();
}

void medBoutiquesInvocationGUIWidget::populateDirectoriesAndSetOutputFileName(QJsonObject &invocationJSON, QStringList &directories)
{
    QDir::setCurrent(QDir::homePath());

    // Change the current directory to the given path (if any, and if the path is valid)
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

    // See description in the header at medBoutiquesInvocationGUIWidget::populateDirectoriesAndSetOutputFileName()
    this->populateInputDirectories(invocationJSON, directories, hasChangedCurrentDirectory);
    this->populateOutputDirectoriesAndSetOutputFileName(invocationJSON, directories, hasChangedCurrentDirectory);

}

QString medBoutiquesInvocationGUIWidget::getOutputFileName()
{
    return this->outputFileName;
}

void medBoutiquesInvocationGUIWidget::askChangeCurrentDirectory()
{
    // Ask the user to set the current directory
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
    QFileInfo fileInfo(fileNameValue.toString());
    if(fileInfo.isRelative())
    {
        if(!hasChangedCurrentDirectory)
        {
            // If the path is relative and the user did not set any working directory: ask him to do so (only once)
            this->askChangeCurrentDirectory();
            hasChangedCurrentDirectory = true;
        }
        return;
    }
    // Add the resulting absolute path to the current directories (to mount them later)
    const QString &path = this->fileHandler->normalizePath(fileInfo.path());
    if(!directories.contains(path))
    {
        directories.append(path);
    }
}

void medBoutiquesInvocationGUIWidget::populateInputDirectories(const QJsonObject &invocationJSON, QStringList &directories, bool &hasChangedCurrentDirectory)
{
   // For all file inputs: populate the directories list with the absolute file path
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
           if(inputObject.description["list"].toBool())             // If parameter is a list: add all file paths
           {
               const QJsonArray &paths = value.toArray();
               for(auto path: paths)
               {
                    this->populateAbsolutePath(path, directories, hasChangedCurrentDirectory);
               }
           }
           else                                                     // If parameter is a single file: just add it
           {
               this->populateAbsolutePath(value, directories, hasChangedCurrentDirectory);
           }
       }
   }
}

void medBoutiquesInvocationGUIWidget::populateOutputDirectoriesAndSetOutputFileName(const QJsonObject &invocationJSON, QStringList &directories, bool &hasChangedCurrentDirectory)
{
    // See the description of how the output files are generated: "Boutiques: a flexible framework for automatedapplication integration in computing platforms"
    //                                                            https://arxiv.org/pdf/1711.09713.pdf

    // For all inputs: if the parameter is a File or a String and has a "value-key":
    //      check if an "output-file" has a "path-template" containing this "value-key",
    //      remove all "path-template-stripped-extensions" from the input parameter value (which is a file name),
    //      then replace this "value-key" in the "path-template" with the file name (= the input parameter value)
    for (auto& idAndInputObject: idToInputObject)
    {
        const QString &inputId = idAndInputObject.first;
        InputObject &inputObject = idAndInputObject.second;
        const QString &inputType = inputObject.description["type"].toString();

        if((inputType == "File" || inputType == "String") && invocationJSON.contains(inputId))
        {
            // For all output files: check if one has "path-template" containing the "value-key" of the current input
            QString fileName = invocationJSON[inputId].toString();
            for (int i = 0 ; i<this->outputFiles.size() ; ++i)
            {
                const QJsonObject &outputFilesDescription = this->outputFiles[i].toObject();
                QString pathTemplate = outputFilesDescription["path-template"].toString();

                // If the input is a File, remove the "path-template-stripped-extensions"
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
                    // If the current output file has a "path-template" containing the current input "value-key": replace the "value-key" by the file name (!input value)
                    pathTemplate.replace(valueKey, fileName);

                    // Make sure the path is absolute (ask user to set the current directory if necessary)
                    QFileInfo fileInfo(pathTemplate);
                    if(fileInfo.isRelative())
                    {
                        if(!hasChangedCurrentDirectory)
                        {
                            this->askChangeCurrentDirectory();
                            hasChangedCurrentDirectory = true;
                        }
                    }

                    // Add the absolute path to the list of directories to mount
                    const QString &absolutePath = fileInfo.isRelative() ? fileInfo.absolutePath() : fileInfo.path();
                    const QString &normalizedPath = this->fileHandler->normalizePath(absolutePath);
                    if(!directories.contains(normalizedPath))
                    {
                        directories.append(normalizedPath);
                    }

                    // Set the output file name (to automatically open it in medInria when the process is over)
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
        // If unset the output: just clear selectedOutputId (which will be used to set the outputFileName in populateOutputDirectoriesAndSetOutputFileName() )
        this->selectedOutputId.clear();
        toggleOutputPushButton->setText("Set output");
        return;
    }

    // If set the output: retrieve the output id of all output files and set selectedOutputId
    // If there is an ambiguity ( = mutliple output files are associated with this input parameter): as the user to select one output file
    const InputObject &inputObject = this->idToInputObject.at(inputId);
    const QString &inputValueKey = inputObject.description["value-key"].toString();

    struct OutputFile {
        QString name;
        QString description;
        QString id;
        OutputFile(const QString &name, const QString &description, const QString &id): name(name), description(description), id(id) {}
    };

    // Populate all output files from the descriptors, only consider output that have a known extension which can be open in medInria
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
        // If no compatible output file: return
        return;
    }
    else if (outputFiles.size() == 1)
    {
        // If a single output file: just set selectedOutputId
        this->selectedOutputId = outputFiles[0].id;
        toggleOutputPushButton->setText("Unset output");
    }
    else
    {
        // If a multiple output files: ask the user to select a single one
        QDialog messageBox;

        // Create a dialog with a combo box (drop down menu) for the user to choose the output file
        QVBoxLayout *messageBoxLayout = new QVBoxLayout();
        messageBox.setLayout(messageBoxLayout);

        QLabel *title = new QLabel("<b>Please select an output file type</b>");
        QLabel *message = new QLabel("There are multiple output files associated with this input, please select the one which will be displayed as output.");
        QLabel *descriptionLabel = new QLabel("");
        QPushButton *okButton = new QPushButton("Ok");

        connect(okButton, &QPushButton::clicked, [&messageBox]() { messageBox.close(); } );

        messageBoxLayout->addWidget(title);
        messageBoxLayout->addWidget(message);

        // Create the combo box with all compatible output files
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

        // Set selectedOutputId to the chosen output file
        this->selectedOutputId = typeComboBox->itemData(typeComboBox->currentIndex(), Qt::UserRole + 1).toString();
        toggleOutputPushButton->setText("Unset output");
    }
}
