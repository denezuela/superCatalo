#include <QApplication>
#include <QQmlApplicationEngine>
#include <QTreeView>
#include <QDebug>
#include <QModelIndex>
#include <QQmlContext>
#include <QQmlComponent>
#include <QItemSelectionModel>
#include "imageprovider.h"

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    ImageProvider ip;

    ip.addImage(15, "OS", "./22", "LECT2");
    QTreeView treeView;
    treeView.setModel(&ip);
    treeView.show();

//    engine.rootContext()->setContextProperty("ip", &ip);
//    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
