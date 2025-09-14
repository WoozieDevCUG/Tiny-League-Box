#include "widget.h"

#include <QApplication>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("QtDemoOrg");
    QCoreApplication::setApplicationName("QtNetworkDemo");
    Widget w;
    w.show();
    return a.exec();
}
