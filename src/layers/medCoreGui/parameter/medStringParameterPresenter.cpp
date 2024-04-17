/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medStringParameter.h>
#include <medStringParameterPresenter.h>

#include <QWidget>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QDateEdit>
#include <QPushButton>
#include <QValidator>
#include <QFileDialog>
#include <QDir>

class medStringParameterPresenterPrivate
{
public:
    medStringParameter* parameter;
};

medStringParameterPresenter::medStringParameterPresenter(medStringParameter* parameter)
    :medAbstractParameterPresenter(parameter), d(new medStringParameterPresenterPrivate)
{
    d->parameter = parameter;
}

medStringParameterPresenter::medStringParameterPresenter(QString const& newParameterId)
    : medStringParameterPresenter(new medStringParameter(newParameterId))
{

}

medStringParameterPresenter::~medStringParameterPresenter()
{

}

medStringParameter* medStringParameterPresenter::parameter() const
{
    return d->parameter;
}

QWidget* medStringParameterPresenter::buildWidget()
{
    QWidget *poWidgetRes = nullptr;
    switch (d->parameter->defaultRepresentation())
    {
    case 1:
        poWidgetRes = this->buildDateEdit(); break;
    case 2:
        poWidgetRes = this->buildLineEditOnFinish(); break;
    case 3:
        poWidgetRes = this->buildLineEditPassword(); break;
    case 4:
        poWidgetRes = this->buildLineEditPasswordEyes(); break;
    case 5:
        poWidgetRes = this->buildLineEditFileImport(); break;
    case 0:
    default:
        poWidgetRes = this->buildLineEdit(); break;
    }
    return poWidgetRes;
}

QLineEdit* medStringParameterPresenter::buildLineEdit()
{
    QLineEdit *lineEdit = new QLineEdit;

    lineEdit->setToolTip(d->parameter->description());
    lineEdit->setText(d->parameter->value());
    lineEdit->setValidator(d->parameter->getValidator());

    this->_connectWidget(lineEdit);
    connect(d->parameter, &medStringParameter::validatorChanged, lineEdit, &QLineEdit::setValidator);
    connect(d->parameter, &medStringParameter::valueChanged, lineEdit, &QLineEdit::setText);
    connect(lineEdit, &QLineEdit::textEdited, d->parameter, &medStringParameter::setValue);
    connect(lineEdit, &QLineEdit::editingFinished, d->parameter, &medStringParameter::edit);

    return lineEdit;
}

QLineEdit* medStringParameterPresenter::buildLineEditOnFinish()
{
    QLineEdit *lineEdit = new QLineEdit;

    lineEdit->setToolTip(d->parameter->description());
    lineEdit->setText(d->parameter->value());
    lineEdit->setValidator(d->parameter->getValidator());

    this->_connectWidget(lineEdit);
    connect(d->parameter, &medStringParameter::validatorChanged, lineEdit, &QLineEdit::setValidator);
    connect(d->parameter, &medStringParameter::valueChanged, lineEdit, &QLineEdit::setText);
    auto *pParam = d->parameter;
    connect(lineEdit, &QLineEdit::editingFinished, [=]() {pParam->setValue(lineEdit->text()); });

    return lineEdit;
}

QDateEdit* medStringParameterPresenter::buildDateEdit()
{
    QDateEdit *dateEdit = new QDateEdit;

    dateEdit->setToolTip(d->parameter->description());
    dateEdit->setCalendarPopup(true);
    QDate date = QDate::fromString( d->parameter->value(), "yyyyMMdd");
    if (!date.isValid())
    {
        date = QDate::currentDate();
    }
    dateEdit->setDate(date);

    this->_connectWidget(dateEdit);
    connect(dateEdit, &QDateEdit::dateChanged, [&](QDate date){ 
        emit d->parameter->setValue(date.toString("yyyyMMdd"));
    });

    connect(d->parameter, &medStringParameter::valueChanged, [=](QString value){
        dateEdit->setDate(QDate::fromString(value, "yyyyMMdd"));
    });

    return dateEdit;
}

QLineEdit * medStringParameterPresenter::buildLineEditPassword()
{
	auto * pLineEdit = buildLineEdit();
	pLineEdit->setEchoMode(QLineEdit::Password);

	return pLineEdit;
}
#include <QIcon>
QWidget * medStringParameterPresenter::buildLineEditPasswordEyes()
{
	QWidget * pWidgetRes = new QWidget();

	auto * pLayout = new QHBoxLayout();
	auto * pLineEdit = buildLineEditPassword();
	auto * pEyeButton = new QPushButton();

	pLayout->addWidget(pLineEdit);
	pLayout->addWidget(pEyeButton);
	pWidgetRes->setLayout(pLayout);

	pEyeButton->setCheckable(true);
	pEyeButton->setIcon(QIcon(":/icons/eye_closed.png"));
	
	connect(pEyeButton, &QPushButton::toggled, [=](bool toggle) 
	{
		if (toggle)
		{
			pLineEdit->setEchoMode(QLineEdit::Normal);
			pEyeButton->setIcon(QIcon(":/icons/eye_open.png"));
		}
		else
		{
			pLineEdit->setEchoMode(QLineEdit::Password);
			pEyeButton->setIcon(QIcon(":/icons/eye_closed.png"));
		}
	});


	return pWidgetRes;
}

QWidget * medStringParameterPresenter::buildLineEditFileImport()
{
    QWidget *widget = new QWidget();
    QHBoxLayout *searchFilePath = new QHBoxLayout();

    QLineEdit *displayPath = buildLineEdit();
    QPushButton *searchButton = new QPushButton("...");
    this->_connectWidget(searchButton);

    auto *pParam = d->parameter;
    connect(searchButton, &QPushButton::clicked, [=]()
    {
        QString sourceFile = QFileDialog::getOpenFileName(nullptr, pParam->caption(), QDir::homePath(), tr("JSON file (*.json)"));
        if(!sourceFile.isEmpty()){
            displayPath->setText(QDir::toNativeSeparators(sourceFile));
            pParam->setValue(displayPath->text());
        }
    });

    searchFilePath->addWidget(displayPath);
    searchFilePath->addWidget(searchButton);
    widget->setLayout(searchFilePath);

    return widget;
}
