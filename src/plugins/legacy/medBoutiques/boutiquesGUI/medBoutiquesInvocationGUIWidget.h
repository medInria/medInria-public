#ifndef MEDBOUTIQUESINVOCATIONGUIWIDGET_H
#define MEDBOUTIQUESINVOCATIONGUIWIDGET_H

#include <map>
#include <functional>
#include <QWidget>
#include <QJsonObject>
#include <QJsonArray>
#include "medBoutiquesConfiguration.h"
#include "medBoutiquesSearchToolsWidget.h"
#include "medBoutiquesAbstractFileHandler.h"
#include "medBoutiquesDropWidget.h"

QT_BEGIN_NAMESPACE
class QScrollArea;
class QJsonObject;
class QDoubleSpinBox;
class QRadioButton;
class QComboBox;
class QTimer;
QT_END_NAMESPACE

struct GroupObject {
    QJsonObject description;
    QLayout *layout = nullptr;
    QGroupBox *groupBox = nullptr;
    QComboBox *comboBox = nullptr;
    bool optional;
};

struct InputObject {
    QJsonObject description;
    QWidget *widget = nullptr;
    GroupObject *group = nullptr;
    std::function<QJsonValue()> getValue;
    InputObject(const QJsonObject &description): description(description) {}
};

class medBoutiquesInvocationGUIWidget : public QWidget
{
    Q_OBJECT
private:
    medBoutiquesSearchToolsWidget *searchToolsWidget;
    medBoutiquesAbstractFileHandler *fileHandler;
    QVBoxLayout *layout;
    QGroupBox *selectCurrentDirectoryGroupBox;
    QLineEdit *selectCurrentDirectoryLineEdit;
    QScrollArea *scrollArea;
    QWidget *group;
    QGroupBox *optionalInputGroup;
    QTimer *emitInvocationChangedTimer;
    QJsonObject *invocationJSON;
    QJsonObject *completeInvocationJSON;
    QJsonArray outputFiles;
    map<QString, InputObject> idToInputObject;
    vector<GroupObject> groupObjects;
    QString selectedOutputId;
    QString outputFileName;
    bool ignoreSignals;

public:
    explicit medBoutiquesInvocationGUIWidget(QWidget *parent = nullptr, medBoutiquesSearchToolsWidget *searchToolsWidget = nullptr, medBoutiquesAbstractFileHandler *toolBox = nullptr);
    ~medBoutiquesInvocationGUIWidget();
    void parseDescriptor(QJsonObject *invocationJSON);
    bool generateCompleteInvocation();
    // Check all input and output paths. Change [CURRENT INPUT] to actual temporary file created for this purpose in QDir::tempPath().
    // If one or more relative paths: open a dialog for the user to select the root directory ; relative paths will be converted to absolute path from this directory
    // Return the list of used directories to mount them (bosh -v option), to make them accessible to docker
    void populateDirectoriesAndSetOutputFileName(QJsonObject &invocationJSON, QStringList &directories);
    QString getOutputFileName();

private:
    bool inputGroupIsMutuallyExclusive(const QString &inputId);
    pair<QGroupBox *, QVBoxLayout *> createGroupAndLayout(const QString &name);
    void valueChanged(const QString &inputId);
    void removeMutuallyExclusiveParameters(const QString &inputId);
    QJsonArray stringToArray(const QString &string);
    QWidget *createUnsetGroup(const QString &inputId, QWidget *widget);
    void askChangeCurrentDirectory();
    void populateAbsolutePath(const QJsonValue &fileNameValue, QStringList &directories, bool &hasChangedCurrentDirectory);
    void populateInputDirectories(const QJsonObject &invocationJSON, QStringList &directories, bool &hasChangedCurrentDirectory);
    void populateOutputDirectoriesAndSetOutputFileName(const QJsonObject &invocationJSON, QStringList &directories, bool &hasChangedCurrentDirectory);

    void createSelectCurrentDirectoryGUI();
    void toggleOutputFile(QPushButton *toggleOutputPushButton, const QString &inputId);

signals:
    void invocationChanged();

public slots:
private slots:
    void optionalGroupChanged(bool on);
    void mutuallyExclusiveGroupChanged(GroupObject *groupObject, int itemIndex);
    void emitInvocationChanged();
};

#endif // MEDBOUTIQUESINVOCATIONGUIWIDGET_H
