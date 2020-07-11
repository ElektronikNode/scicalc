#include "dialoggeneralsettings.h"
#include "ui_dialoggeneralsettings.h"

#include <QSettings>
#include <QColorDialog>
#include <QFontDialog>


DialogGeneralSettings::DialogGeneralSettings(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DialogGeneralSettings)
{
	ui->setupUi(this);
	
	QSettings set;
	ui->spinBox_digits->setValue(set.value("digits", 6).toInt());
	ui->checkBox_zeros->setChecked(set.value("trailingZeros", false).toBool());
	
	backcolor.setNamedColor(set.value("backcolor", "#000000").toString());
	incolor.setNamedColor(set.value("incolor", "#00FF00").toString());
	outcolor.setNamedColor(set.value("outcolor", "#FF0000").toString());
	
	font.fromString(set.value("font", "Courier 10 Pitch").toString());
	
	QPalette pal;
	pal.setColor(QPalette::Window, backcolor);
	ui->label_backcolor->setPalette(pal);
	ui->label_backcolor->setAutoFillBackground(true);
	
	pal.setColor(QPalette::Window, incolor);
	ui->label_incolor->setPalette(pal);
	ui->label_incolor->setAutoFillBackground(true);
	
	pal.setColor(QPalette::Window, outcolor);
	ui->label_outcolor->setPalette(pal);
	ui->label_outcolor->setAutoFillBackground(true);
	
	ui->lineEdit_Font->setText(font.family());
}

DialogGeneralSettings::~DialogGeneralSettings()
{
	delete ui;
}

void DialogGeneralSettings::on_buttonBox_accepted()
{
	QSettings set;
	set.setValue("digits", getDigits());
	set.setValue("trailingZeros", getTrailingZeros());
			
	set.setValue("backcolor", backcolor);
	set.setValue("incolor", incolor);
	set.setValue("outcolor", outcolor);
	
	set.setValue("font", font);
}

int DialogGeneralSettings::getDigits()
{
	return ui->spinBox_digits->value();
}

bool DialogGeneralSettings::getTrailingZeros()
{
	return ui->checkBox_zeros->isChecked();
}

QColor DialogGeneralSettings::getBackColor()
{
	return backcolor;
}

QColor DialogGeneralSettings::getInColor()
{
	return incolor;
}

QColor DialogGeneralSettings::getOutColor()
{
	return outcolor;
}

QFont DialogGeneralSettings::getFont()
{
	return font;
}


void DialogGeneralSettings::on_toolButton_background_clicked()
{
	backcolor=QColorDialog::getColor(backcolor, this, "Pick the background color!");
	QPalette pal;
	pal.setColor(QPalette::Window, backcolor);
	ui->label_backcolor->setPalette(pal);
}

void DialogGeneralSettings::on_toolButton_input_clicked()
{
	incolor=QColorDialog::getColor(incolor, this, "Pick the input color!");
	QPalette pal;
	pal.setColor(QPalette::Window, incolor);
	ui->label_incolor->setPalette(pal);
}

void DialogGeneralSettings::on_toolButton_output_clicked()
{
	outcolor=QColorDialog::getColor(outcolor, this, "Pick the output color!");
	QPalette pal;
	pal.setColor(QPalette::Window, outcolor);
	ui->label_outcolor->setPalette(pal);
}

void DialogGeneralSettings::on_toolButton_Font_clicked()
{
	bool *ok=(bool*)0;
	font=QFontDialog::getFont(ok, font, this, "Pick the text font");
	ui->lineEdit_Font->setText(font.family());
}
