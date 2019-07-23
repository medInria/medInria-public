#ifndef MEDBOUTIQUESINVOCATION_H
#define MEDBOUTIQUESINVOCATION_H

#include <QWidget>
#include "medBoutiquesSearchTools.h"
#include "medBoutiquesInvocationGUI.h"

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

class medBoutiquesInvocation : public QWidget
{
    Q_OBJECT
public:
    medBoutiquesInvocationGUI *invocationGUIWidget;
    QTextEdit *invocationEditor;

private:
    medBoutiquesSearchTools *searchToolsWidget;
    QVBoxLayout *layout;
    QPushButton *openInvocationButton;
    QPushButton *saveInvocationButton;
    QJsonObject invocationJSON;
    QProcess *generateInvocationProcess;

public:
    explicit medBoutiquesInvocation(QWidget *parent = nullptr, medBoutiquesSearchTools *searchToolsWidget = nullptr);

private:
    void generateInvocationFile();

signals:

public slots:
    void generateInvocationProcessFinished();
    void openInvocationFile();
    void saveInvocationFile();
    void invocationChanged();
    void toolSelected();
    void toolDeselected();
};

#endif // MEDBOUTIQUESINVOCATION_H
