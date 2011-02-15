#ifndef MEDSETTINGSWIDGET_H
#define MEDSETTINGSWIDGET_H

#include <QWidget>

class medSettingsWidgetPrivate;

/**
 * @brief AbstractClass for all Settings widgets.
 *
 * All the pages in the medSettingsEditor must inherit from this class.
 *
 *
*/
class medSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief Should not be used directly, the factory will instanciate all the settingsWidgets.
     *
     * @param parent Parent widget. Should be the medSettingsEditor
    */
    explicit medSettingsWidget(QWidget *parent = 0);

    /**
     * @brief Save the form into the QSettings.
     * First calls validate() and if validate() fails, emit a showError signal.
     *
     * @param void
     * @return bool true if saving worked, false otherwise.
    */
    virtual bool save(void);
    /**
     * @brief Gets the section name of this Widget.
     * This is the tab title, as well as the section name in the QSettings.
     *
     * @return const QString &
    */
    const QString& section()const;

signals:
    /**
     * @brief Emits an error message for the medMessageController to display.
     *
     * @param sender Should be the current Object itself.
     * @param text The error message.
     * @param timeout The timeout before the message disapears.
    */
    void showError(QObject *sender, const QString& text,unsigned int timeout=0);

public slots:
    /**
     * @brief Reads its data from the QSettings.
     *
    */
    virtual void read()=0;
protected:
    /**
     * @brief Validates the form before saving the data.
     *
     * @param void
     * @return bool true is succeeded, false otherwise.
    */
    virtual bool validate(void);
    /**
     * @brief Sets the section name. MUST be used by inherited classes.
     *
     * @param section
    */
    void setSection(QString section);

private:
    medSettingsWidgetPrivate *d; /**< TODO */
};

#endif // MEDSETTINGSWIDGET_H
