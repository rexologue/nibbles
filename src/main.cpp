#include <QApplication>
#include <QLocale>
#include <QFontDatabase>
#include "mainwindow.h"


int main(int argc, char* argv[])
{
    // В Qt 6 масштабирование HiDPI по умолчанию адекватно, но на всякий случай Locale на C,
    // чтобы точка была разделителем (мы всё равно форматируем вручную):
    QLocale::setDefault(QLocale::C);

    QApplication app(argc, argv);
    app.setApplicationName("Nibble Entropy GUI");
    app.setOrganizationName("YourOrg");

    MainWindow w;
    w.resize(1100, 700);
    w.show();
    return app.exec();
}
