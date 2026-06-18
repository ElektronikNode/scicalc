#ifndef SCICALC_H
#define SCICALC_H

#include <QMainWindow>
#include <QDateTime>
#include <QStringList>
#include "dialoggeneralsettings.h"
#include "scicalcedit.h"

namespace Ui {
class scicalc;
}

class scicalc : public QMainWindow
{
	Q_OBJECT
	
public:
	explicit scicalc(QMainWindow *parent = 0);
	~scicalc();
	
	static scicalc* app();
	static QString getVersion();

	void load(QString fileName);
	void save(QString fileName);
	
	DialogGeneralSettings* dialogGeneralSettings;
	
		int getDigitsSetting() const;
		bool getTrailingZerosSetting() const;
		bool getAccountingModeSetting() const;
		bool setTemporaryDigits(int digits);
		bool setTemporaryTrailingZeros(bool enabled);
		bool setTemporaryAccounting(bool enabled);
		void resetTemporarySettings();
	
private slots:	
	void on_actionNew_triggered();
	void on_actionOpen_triggered();
	void on_actionSave_triggered();
	void on_actionSave_as_triggered();
	
	void on_actionRefresh_triggered();
	void inputChanged();
	void settingsChanged();
	
	void on_actionReadme_triggered();
	void on_actionConstants_triggered();
	void on_actionFunctions_triggered();
	void on_actionGeneral_settings_triggered();
	
	void on_actionAbout_scicalc_triggered();
	
	void on_actionChangelog_triggered();
	void tabChanged(int index);
	void closeTabRequested(int index);

private:
	bool eventFilter(QObject *watched, QEvent *event);
	void closeEvent(QCloseEvent *event);
	bool askForSave(ScicalcEdit *edit);
	bool isSaved(ScicalcEdit *edit) const;
	void setSaved(bool saved);
	void setSaved(ScicalcEdit *edit, bool saved);
	void showTextFile(QString filename);
	ScicalcEdit *currentEdit() const;
	ScicalcEdit *createTab(QString text=QString(), QString fileName=QString(), QDateTime createdAt=QDateTime(), bool saved=true);
	void connectEditor(ScicalcEdit *edit);
	void refreshEditor(ScicalcEdit *edit);
	void closeTab(int index);
	bool tabCanClose(int index);
	bool isEditorEmpty(ScicalcEdit *edit) const;
	QString tabFileName(ScicalcEdit *edit) const;
	void setTabFileName(ScicalcEdit *edit, QString fileName);
	QDateTime tabCreatedAt(ScicalcEdit *edit) const;
	QString tabDefaultFileName(ScicalcEdit *edit) const;
	QString tabTitle(ScicalcEdit *edit) const;
	void updateTabTitle(ScicalcEdit *edit);
	void updateWindowTitle();
	void applyEditorSettings(ScicalcEdit *edit);
	bool loadFileIntoEditor(ScicalcEdit *edit, QString fileName);
	bool saveEditorToFile(ScicalcEdit *edit, QString fileName);
	void saveSession();
	bool restoreSession();
	
	static scicalc* myApp;
	
	int temporaryDigits;
	bool temporaryDigitsActive;
	bool temporaryTrailingZeros;
	bool temporaryTrailingZerosActive;
	bool temporaryAccounting;
	bool temporaryAccountingActive;
	
	QString currentPath; // holds the name of the current path
		// it is used to remember the open/save/save as location in the file-system
	
	Ui::scicalc *ui;
	
	static const QString tempFile;
	static const QString version;
};

#endif // SCICALC_H
