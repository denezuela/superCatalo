#include <QApplication>
#include <QQmlApplicationEngine>
#include <QTreeView>
#include <QDebug>
#include <QModelIndex>
#include <QQmlContext>
#include <QQmlComponent>
#include <QItemSelectionModel>
#include "imageprovider.h"
#include "imageredactor.h"

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    ImageProvider ip;
    ImageRedactor ir;
    engine.rootContext()->setContextProperty("ip", &ip);
    engine.rootContext()->setContextProperty("redactor", &ir);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();


}
