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

//    ip.addSemester(5);
//    ip.addCourse(5,"Вектан");
//    ip.addCourse(16,"Финмат");
//    ip.addImage(16,"Вектан","../vect","comments");
//    ip.addImage(16,"Финмат","../finmat","comments");
//    ip.deleteImage(16,"Вектан","../vect");
//    //ip.deleteCourse(16,"Финмат");
//    ip.addSemester(4);
    //ip.deleteSemester(15);

//    QTreeView treeView;
//    treeView.setModel(&ip);
//    treeView.show();

    engine.rootContext()->setContextProperty("mymodel", &ip);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
