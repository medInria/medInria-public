#ifndef MEDBROWSERTOOLBOXSETTINGS_H
#define MEDBROWSERTOOLBOXSETTINGS_H

#include <medGui/medToolBox.h>

class medBrowserToolBoxSettingsPrivate;

/**
 * This ToolBox has been created just to temporary launch the Preference Pane pop-up.
 * It has just one button who does the job and that's it.
 * Later, a welcome page will be added to the project and that is going to be the right
 * place to put the launcher, so this ToolBox will be removed and deleted.
 */
class medBrowserToolBoxSettings : public medToolBox
{
    Q_OBJECT

public:
     medBrowserToolBoxSettings(QWidget *parent = 0);
    ~medBrowserToolBoxSettings(void);

public slots:
	void onClick (void);

signals:

private:
    medBrowserToolBoxSettingsPrivate *d;
};

#endif
