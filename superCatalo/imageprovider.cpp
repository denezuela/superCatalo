#include "imageprovider.h"
#include <QDebug>
#include <QSqlQuery>
#include <QModelIndex>

ImageProvider::ImageProvider(QAbstractItemModel *parent) : QAbstractItemModel(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName("localhost");
    db.setDatabaseName("/home/skt/sqlite/db");
    bool db_ok = db.open();
    if (!db_ok) {
        qDebug() << "Database error";
        return;
    }
    else {
        qDebug() << "Database OK";
    }

    QModelIndex rootIndex = createIndex(0, 0, &root);
    fetchAll(rootIndex);
    forTests();
}

QVariant ImageProvider::data (const QModelIndex &index, int role) const {

    if (!index.isValid())
        return QVariant();
    return QVariant();
}

QModelIndex ImageProvider::index(int row, int column, const QModelIndex &index) const {
    return index;
}

int ImageProvider::rowCount(const QModelIndex &index) const {
    return 0;
}

int ImageProvider::columnCount(const QModelIndex &index) const {
    return 0;
}

QModelIndex ImageProvider::parent(const QModelIndex &index) const {
    return index;
}

void ImageProvider::fetchAll(const QModelIndex &parent) {
    DataWrapper* parentDataWrapper = (DataWrapper*)parent.internalPointer();

    QSqlQuery query;
    query.prepare("SELECT * FROM IMAGES WHERE PID=:id");
    query.bindValue(":id", parentDataWrapper->id);
    query.exec();

    while(query.next()) {
      DataWrapper* childWrapper = new DataWrapper;
      dbData* childData = new dbData;

      childData->id = query.value("id").toInt();
      childData->comments = query.value("comment").toString();
      childData->number = query.value("number").toInt();
      childData->pid = query.value("pid").toInt();
      childData->tags = query.value("tags").toString();
      childData->type = query.value("type").toString();
      childData->path = query.value("path").toString();

      childWrapper->data = childData;
      childWrapper->children = {};
      childWrapper->childrenCount = 0;
      childWrapper->id = childData->id;
      childWrapper->number=childData->number;
      childWrapper->parent_pointer = parentDataWrapper;

      if (childData->type == "semester")
        childWrapper->type = SEMESTER;
      else if (childData->type == "course")
        childWrapper->type = COURSE;
      else
        childWrapper->type = IMAGE;

      parentDataWrapper->children.append(childWrapper);
      parentDataWrapper->childrenCount += 1;
    }
}

void ImageProvider::forTests() {
    qDebug() << root.children[0]->id;
}


