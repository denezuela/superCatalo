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
    QApplication app(argc, argv);

    ImageProvider ip;

//    QTreeView treeView; //создание объекта класса treeview
//    treeView.setModel(&ip); //установить соответствующую модель
//    treeView.show();

    QQmlEngine engine;
    engine.rootContext()->setContextProperty("myModel", &ip);
    QQmlComponent component(&engine, QUrl("qrc:/main.qml"));
    component.create();


    return app.exec();
}
    //ip.addSemester(3);
    //ip.addCourse(3, "Abba");



