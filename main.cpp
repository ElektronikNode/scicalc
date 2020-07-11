#include <QApplication>
#include <QSettings>
#include "scicalc.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QCoreApplication::setOrganizationName("scicalc");
    QCoreApplication::setOrganizationDomain("https://github.com/ElektronikNode/scicalc");
	QCoreApplication::setApplicationName("scicalc");
	scicalc w;
	w.show();
	
	return a.exec();
}
