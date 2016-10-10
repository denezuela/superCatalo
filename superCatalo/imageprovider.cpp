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

    for (int i = 0; i != root.children.size(); ++i) {
        QModelIndex semesterIndex = createIndex(i, 0,  root.children[i]);
        fetchAll(semesterIndex);

        DataWrapper* ptr = (DataWrapper*)semesterIndex.internalPointer();
        QList<DataWrapper*> courses = ptr->children;

        for (int j = 0; j != courses.size(); ++j) {
            QModelIndex courseIndex = createIndex(j, 0, courses[j]);
            fetchAll(courseIndex);

            DataWrapper* ptr = (DataWrapper*)courseIndex.internalPointer();
            QList<DataWrapper*> images = ptr->children;

            for (int k = 0; k != images.size(); ++k) {
                QModelIndex imageIndex = createIndex(k, 0, images[k]);
                fetchAll(imageIndex);
            }
        }
    }

    forTests();
}

QVariant ImageProvider::data (const QModelIndex &index, int role) const {

    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        DataWrapper* ptr = (DataWrapper*) index.internalPointer();
        qDebug() << ptr->id;
        return ptr->id;
    }
    else
        return QVariant();
}

QModelIndex ImageProvider::index(int row, int column, const QModelIndex &parent) const {

    if (column != 0)
        return QModelIndex();

    if (!parent.isValid())
        return createIndex(row, column, root.children[row]);

    DataWrapper* ptr = (DataWrapper*)parent.internalPointer();

    if (ptr->type == IMAGE)
        return QModelIndex();

    if (ptr->children.size() <= row)
        return QModelIndex();

    DataWrapper* dataWrapperForIndex = ptr->children[row];
    QModelIndex result = createIndex(row, column, dataWrapperForIndex);
    return result;
}

int ImageProvider::rowCount(const QModelIndex &index) const {
    if (!index.isValid()) {
        return -1;
    }

    DataWrapper* ptr = (DataWrapper*)index.internalPointer();
    qDebug() << ptr->childrenCount;
    return ptr->childrenCount;
}

int ImageProvider::columnCount(const QModelIndex &index) const {
    return 0;
}

QModelIndex ImageProvider::parent(const QModelIndex &index) const {
    DataWrapper* ptr = (DataWrapper*)index.internalPointer();
    return createIndex(ptr->parent_pointer->number, 0, ptr->parent_pointer);
}

void ImageProvider::fetchAll(const QModelIndex &parent) {

    DataWrapper* parentDataWrapper = (DataWrapper*)parent.internalPointer();
    parentDataWrapper->children.clear();

    QSqlQuery query;
    query.prepare("SELECT * FROM IMAGES WHERE PID=:id ORDER BY number");
    query.bindValue(":id", parentDataWrapper->id);
    query.exec();

    while(query.next()) {
      DataWrapper* childWrapper = new DataWrapper;
      dbData* childData = new dbData;

      childData->id = query.value("id").toInt();
      childData->comments = query.value("comment").toString();
      childData->number = query.value("number").toInt()-1;
      childData->pid = query.value("pid").toInt();
      childData->tags = query.value("tags").toStringList();
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

//    QModelIndex rootIndex = createIndex(0, 0, &root);
//    QModelIndex index = this->index(0, 0, rootIndex);
//   DataWrapper* ptr = (DataWrapper*)rootIndex.internalPointer();
//   qDebug() << ptr->children[0]->children[0]->children[0]->children.size();

//    DataWrapper* ptr = root.children[0];
//    QModelIndex index = createIndex(0, 0, ptr);
//    QModelIndex par = this->parent(index);
//    DataWrapper* ptr2 = (DataWrapper*)par.internalPointer();
//    qDebug() << ptr2->id;

//    qDebug() << this->data(index, 0);

}

ImageProvider::~ImageProvider() {
    db.close();
    delete &root;
}


