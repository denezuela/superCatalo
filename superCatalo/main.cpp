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



//      ip.addSemester(5);
//      ip.addCourse(5,"Вектан");
//      ip.addCourse(5,"Финмат");
//      ip.addImage(5,"Вектан","../vect","comments");
//      ip.addImage(5,"Финмат","../finmat","comments");

//    QTreeView treeView;
//    treeView.setModel(&ip);
//    treeView.show();

    engine.rootContext()->setContextProperty("mymodel", &ip);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
