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
    QTreeView treeView;
    treeView.setModel(&ip);

    treeView.show();

    ip.addCourse(3, "Abba");

    return app.exec();
}
