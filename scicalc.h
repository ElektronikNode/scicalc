#ifndef SCICALC_H
#define SCICALC_H

#include <QMainWindow>
#include <QStringList>
#include "dialoggeneralsettings.h"

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

	void load(QString fileName);
	void save(QString fileName);
	
	DialogGeneralSettings* dialogGeneralSettings;
	
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

private:
	void closeEvent(QCloseEvent *event);
	void askForSave();
	void setSaved(bool saved);
	void showTextFile(QString filename);
	
	static scicalc* myApp;
	
	bool saved; // this flag is true, if the content of the editor is saved to file.
		// it changes to false, if any character of the editor is changed.
	
	QString currentFile; // holds the filename including the path to the current file.
		// it is empty in temporary-mode
	
	QString currentPath; // holds the name of the current path
		// it is used to remember the open/save/save as location in the file-system
	
	Ui::scicalc *ui;
	
	static const QString tempFile;
	static const QString version;
};

#endif // SCICALC_H
