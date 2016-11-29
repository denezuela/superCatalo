#include <QApplication>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QTreeView>
#include <QDebug>
#include <QModelIndex>
#include "imageprovider.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    //QTreeView treeView; //создание объекта класса treeview
    //treeView.setModel(&ip); //установить соответствующую модель
    //treeView.show();

    QQmlEngine engine;
    ImageProvider ip;
    engine.rootContext()->setContextProperty("myModel", &ip);
    QQmlComponent component(&engine, QUrl::fromLocalFile("qrc:/main.qml"));
    //QQmlComponent component(&engine,QUrl(QStringLiteral("qrc:/main.qml")));
    component.create();


    return app.exec();
}
    //QDeclarativeView view;
    //view.rootContext()->setContextProperty("myModel", &ip);

    //QQmlApplicationEngine engine;
    //engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    //    QDeclarativeView view;
    //      ImageProvider *model = new ImageProvider;
    //      view.rootContext()->setContextProperty("myModel", model);
    //      view.setSource(QUrl("qrc:/Main.qml"));
    //     view.show();

    //ip.addSemester(3);
    //ip.addCourse(3, "Abba");



