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

    engine.rootContext()->setContextProperty("mymodel", &ip);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
