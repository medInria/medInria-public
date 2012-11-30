#ifndef MEDVIEWERWORKSPACEREGISTRATION_H
#define MEDVIEWERWORKSPACEREGISTRATION_H


#include <QtCore>

#include <medViewerWorkspace.h>

class medTabbedViewContainers;
class medViewerWorkspaceRegistrationPrivate;

class medViewerWorkspaceRegistration : public medViewerWorkspace
{
    Q_OBJECT

public:
     medViewerWorkspaceRegistration(QWidget *parent = 0);
    ~medViewerWorkspaceRegistration(void);

    virtual QString identifier()  const;
    virtual QString description() const;
    static bool isUsable();
    void setupViewContainerStack ();

public slots:
    void patientChanged(int patientId);

private:
    medViewerWorkspaceRegistrationPrivate *d;
};

#endif // MEDVIEWERWORKSPACEREGISTRATION_H
