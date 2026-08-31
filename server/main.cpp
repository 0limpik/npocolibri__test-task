#include <QApplication>
#include <QDateTime>
#include <QLocale>
#include <QTranslator>

#include "Log.h"
#include "MainWindow.h"

int main(int argc, char* argv[])
{
    QApplication application {argc, argv};
    qInstallMessageHandler(globalLogCallback);

    QTranslator translator;
    const auto uiLanguages {QLocale::system().uiLanguages()};
    for (const auto& locale : uiLanguages)
    {
        const auto baseName {QLocale(locale).name()};
        if (translator.load(":/i18n/" + baseName))
        {
            application.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return QCoreApplication::exec();
}
