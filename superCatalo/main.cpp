#include <QApplication>
#include <QQmlApplicationEngine>
#include <QTreeView>
#include "imageprovider.h"

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    ImageProvider ip;

    QTreeView treeView;
    treeView.setModel(&ip);
    treeView.show();

    return app.exec();
}
