#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QDebug>
#include "scicalc.h"
#include "scanner.h"
#include "parser.h"
#include "variables.h"

namespace
{
	int runCli(QString fileName)
	{
		QFile file(fileName);
		if(!file.open(QFile::ReadOnly | QFile::Text))
		{
			QTextStream err(stderr);
			err << "ERROR: could not open file: " << fileName << "\n";
			return 1;
		}

		QTextStream in(&file);
		in.setCodec("UTF-8");
		QTextStream out(stdout);
		out.setCodec("UTF-8");

		scicalc::app()->resetTemporarySettings();
		Variables::init();

		bool accountingMode=scicalc::app()->getAccountingModeSetting();
		bool previousResultAvailable=false;

		while(!in.atEnd())
		{
			QString expression=in.readLine();
			if(expression.startsWith("\t"))
			{
				continue;
			}

			QString evaluatedExpression=expression;
			if(accountingMode && previousResultAvailable)
			{
				int firstCharIndex=0;
				while(firstCharIndex<evaluatedExpression.size() && evaluatedExpression.at(firstCharIndex).isSpace())
				{
					firstCharIndex++;
				}

				if(firstCharIndex<evaluatedExpression.size())
				{
					QChar firstChar=evaluatedExpression.at(firstCharIndex);
					bool isDivisionComment=(firstChar=='/' && firstCharIndex+1<evaluatedExpression.size() && evaluatedExpression.at(firstCharIndex+1)=='/');
					if(!isDivisionComment &&
						(firstChar=='+' || firstChar=='-' || firstChar=='*' || firstChar=='/' || firstChar=='^'))
					{
						evaluatedExpression.insert(firstCharIndex, QChar('$'));
					}
				}
			}

			Scanner::init(evaluatedExpression);
			QString output=Parser::parse();
			out << expression << "\n";
			if(!output.isEmpty())
			{
				QStringList outputLines=output.split("\n");
				for(int i=0; i<outputLines.size(); i++)
				{
					out << "\t\t\t\t" << outputLines.at(i) << "\n";
				}
			}

			QString trimmedInput=expression.trimmed();
			bool isCommentLine=(trimmedInput.startsWith("//") || trimmedInput.startsWith("%"));
			bool hasExpression=!trimmedInput.isEmpty() && !isCommentLine;
			bool parseOk=!output.startsWith("ERROR");
			if(hasExpression)
			{
				previousResultAvailable=accountingMode && parseOk;
			}

			accountingMode=scicalc::app()->getAccountingModeSetting();
		}

		return 0;
	}
}

int main(int argc, char *argv[])
{
	if(argc==2 && qgetenv("QT_QPA_PLATFORM").isEmpty())
	{
		qputenv("QT_QPA_PLATFORM", "offscreen");
	}

	QApplication a(argc, argv);
	QCoreApplication::setOrganizationName("scicalc");
    QCoreApplication::setOrganizationDomain("https://github.com/ElektronikNode/scicalc");
	QCoreApplication::setApplicationName("scicalc");

	if(QApplication::arguments().size()==2)
	{
		a.setProperty("scicalcCliMode", true);
		scicalc app;
		return runCli(QApplication::arguments().at(1));
	}

	scicalc w;
	w.show();
	
	return a.exec();
}
