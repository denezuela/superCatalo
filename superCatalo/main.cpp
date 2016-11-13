#include <QApplication>
#include <QQmlApplicationEngine>
#include <QTreeView>
#include <QDebug>
#include <QModelIndex>
#include "imageprovider.h"

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);

//    QQmlApplicationEngine engine;
//    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    ImageProvider ip;

    //ip.addSemester(3);
    //ip.deleteSemester(3);
    //ip.addCourse(3, "Arch");
    QTreeView treeView;
    treeView.setModel(&ip);
    treeView.show();

    return app.exec();
}
