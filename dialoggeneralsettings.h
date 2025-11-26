#ifndef DIALOGGENERALSETTINGS_H
#define DIALOGGENERALSETTINGS_H

#include <QDialog>
#include <QColor>

namespace Ui {
class DialogGeneralSettings;
}

class DialogGeneralSettings : public QDialog
{
	Q_OBJECT
	
public:
	explicit DialogGeneralSettings(QWidget *parent = 0);
	~DialogGeneralSettings();
	
	int getDigits();
	bool getTrailingZeros();
	bool getAccountingMode();
	
	QColor getBackColor();
	QColor getInColor();
	QColor getOutColor();
	
	QFont getFont();
	
private slots:
	void on_buttonBox_accepted();
	
	void on_toolButton_background_clicked();
	void on_toolButton_input_clicked();
	void on_toolButton_output_clicked();
	
	void on_toolButton_Font_clicked();
	
private:
	Ui::DialogGeneralSettings *ui;
	
	
	QColor backcolor, incolor, outcolor;
	QFont font;
};

#endif // DIALOGGENERALSETTINGS_H
