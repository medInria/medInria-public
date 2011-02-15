#ifndef MEDSETTINGSWIDGETFACTORY_H
#define MEDSETTINGSWIDGETFACTORY_H

#include "medGuiExport.h"
#include <dtkCore/dtkAbstractFactory.h>
#include <QtCore>

class medSettingsWidget;
class medSettingsWidgetFactoryPrivate;

/**
 * @brief This factory creates Widgets that are pages in the medSettingsEditor widget.
 *
*/
class MEDGUI_EXPORT medSettingsWidgetFactory : public dtkAbstractFactory
{
  Q_OBJECT

public:
  /**
   * @brief This function pointer designates functions allocating memory
   *
  */
  typedef medSettingsWidget*(*medSettingsWidgetCreator)(QWidget *);

  /**
   * @brief Type designating the internal has table containing the creator functions.
   *
  */
  typedef QHash<QString, medSettingsWidgetCreator> medSettingsWidgetCreatorHash;

public:
  /**
   * @brief Gets an instance of the factory.
   *
   * @param void
   * @return medSettingsWidgetFactory * Factory instance
  */
  static medSettingsWidgetFactory * instance(void);
  /**
   * @brief Registers a new widget type, and its creator function.
   *
   * @param type name of the widget type.
   * @param func creator function
   * @return bool true if type was not registered already, false if it exists.
  */
  bool registerSettingsWidget (const QString& type,
                               medSettingsWidgetCreator func);
  /**
   * @brief Gets a list of registered type names.
   *
   * @return QList<QString>
  */
  QList<QString> settingsWidgets();

public slots:
  /**
   * @brief Creates a new widget.
   *
   * @param type the type to instanciate
   * @return medSettingsWidget * the newly allocated widget.
  */
  medSettingsWidget * createSettingsWidget(QString type,
                                           QWidget * parent);

protected:
  /**
   * @brief Constructor, not to be used by users.
   *
   * Use the instance() method instead to get a singleton.
   *
   * @param void
  */
  medSettingsWidgetFactory(void);

  /**
   * @brief Destructor, not to be used by users.
   *
   * The singleton will be deleted at appplication closing time.
   *
   * @param void
  */
  ~medSettingsWidgetFactory(void);

private:
  static medSettingsWidgetFactory * s_instance; /**< TODO */

private:
  medSettingsWidgetFactoryPrivate * d; /**< TODO */
};

#endif // MEDSETTINGSWIDGETFACTORY_H
