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

    QList<DataWrapper*> semesters = root.children;
    for (int i = 0; i != semesters.size(); ++i) {
        QModelIndex semesterIndex = createIndex(i, 0, semesters[i]);
        fetchAll(semesterIndex);

        QList<DataWrapper*> courses = semesters[i]->children;

        for (int j = 0; j != courses.size(); ++j) {
            QModelIndex courseIndex = createIndex(j, 0, courses[j]);
            fetchAll(courseIndex);
        }

    }
    forTests();
}

QVariant ImageProvider::data (const QModelIndex &index, int role) const {

    if (!index.isValid())
        return QVariant();
    return 1;
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
    DataWrapper* ptr = (DataWrapper*)index.internalPointer();
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

      auto id = query.value("id").toInt();
      auto comments = query.value("comment").toString();
      auto number = query.value("number").toInt()-1;
      auto pid = query.value("pid").toInt();
      QStringList tags = query.value("tags").toStringList();
      auto type = query.value("type").toString();
      auto path = query.value("path").toString();

      childWrapper->data = childData;
      childWrapper->children = {};
      childWrapper->childrenCount = 0;
      childWrapper->id = childData->id;
      childWrapper->number=childData->number;
      childWrapper->parent_pointer = parentDataWrapper;

      if (type == "semester")
        childWrapper->type = SEMESTER;
      else if (type == "course")
        childWrapper->type = COURSE;
      else
        childWrapper->type = IMAGE;

      parentDataWrapper->children.append(childWrapper);
      parentDataWrapper->childrenCount += 1;
    }
}

void ImageProvider::forTests() {
    qDebug() << root.children[0]->children[0]->children[0]->data->path;
}

ImageProvider::~ImageProvider() {
    db.close();
    //delete &root;
}


