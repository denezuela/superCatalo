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
    root.childrenCount = getChildrenCount(root.id);
    fetchAll(rootIndex);
}

QVariant ImageProvider::data (const QModelIndex &index, int role) const {

    if (!index.isValid())
        return {};

    if (role == Qt::DisplayRole) {
        const DataWrapper* ptr = dataForIndex(index);
        return ptr->data->comments;
    }
    else
        return QVariant();
}

QModelIndex ImageProvider::index(int row, int column, const QModelIndex &parent) const {

    if (column != 0)
        return QModelIndex();

    if (!parent.isValid())
        return createIndex(row, column, root.children[row]);

    const DataWrapper* ptr = dataForIndex(parent);

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
        return root.childrenCount;
    }

    const DataWrapper* ptr = dataForIndex(index);
    return ptr->childrenCount;
}

int ImageProvider::columnCount(const QModelIndex &index) const {
    return 1;
}

QModelIndex ImageProvider::parent(const QModelIndex &index) const {

    if (!index.isValid()) {
        return QModelIndex();
    }

    const DataWrapper* ptr = dataForIndex(index);

    if (ptr == nullptr) {
        return QModelIndex();
    }

    if (!ptr->parent_pointer) {
        return QModelIndex();
    }

    return createIndex(ptr->parent_pointer->number, 0, static_cast<void*>(ptr->parent_pointer));
}

void ImageProvider::fetchAll(const QModelIndex &parent) {

    DataWrapper* parentDataWrapper = dataForIndex(parent);
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
      childWrapper->id = childData->id;
      childWrapper->number=childData->number;
      childWrapper->parent_pointer = parentDataWrapper;

      childWrapper->childrenCount = getChildrenCount(childWrapper->id);

      if (childData->type == "semester")
        childWrapper->type = SEMESTER;
      else if (childData->type == "course")
        childWrapper->type = COURSE;
      else
        childWrapper->type = IMAGE;

      parentDataWrapper->children.append(childWrapper);
    }
}

void ImageProvider::forTests() {

}

ImageProvider::~ImageProvider() {
    db.close();
    delete &root;
}

const DataWrapper* ImageProvider::dataForIndex(const QModelIndex &index) const {
    if (!index.isValid())
        return &root;

    return static_cast<DataWrapper*>(index.internalPointer());
}

DataWrapper* ImageProvider::dataForIndex(const QModelIndex &index) {
    if (!index.isValid())
        return &root;

    return static_cast<DataWrapper*>(index.internalPointer());
}

void ImageProvider::fetchMore(const QModelIndex &parent) {
    fetchAll(parent);
}

bool ImageProvider::canFetchMore(const QModelIndex &parent) const {
    const DataWrapper* ptr = dataForIndex(parent);
    return (ptr->children.size() < ptr->childrenCount);
}

////////////////////////////////////////////////

void ImageProvider::addSemester(qint64 semesterNumber) {

    if (root.childrenCount >= semesterNumber) {
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO IMAGES (pid, number, path, comment, tags, type) VALUES (:pid, :number, :path, :comment, :tags, :type)" );
    query.bindValue(":pid", root.id);
    query.bindValue(":number", semesterNumber);
    query.bindValue(":path", "NO_PATH");
    query.bindValue(":comment", QString::number(semesterNumber) + " semester");
    query.bindValue(":tags", "NO_TAGS");
    query.bindValue(":type", "semester");
    query.exec();
}

void ImageProvider::addCourse(qint64 semesterNumber, QString courseName) {

    if (root.childrenCount < semesterNumber) {
        qDebug() << root.childrenCount;
        this->addSemester(semesterNumber);
    }

    QSqlQuery query;
    query.prepare("INSERT INTO IMAGES (pid, number, path, comment, tags, type) VALUES (:pid, :number, :path, :comment, :tags, :type)" );
    query.bindValue(":pid", root.children[semesterNumber-1]->id);
    query.bindValue(":number", root.children[semesterNumber-1]->childrenCount + 1);
    query.bindValue(":path", "NO_PATH");
    query.bindValue(":comment", courseName);
    query.bindValue(":tags", courseName);
    query.bindValue(":type", "course");
    qDebug() << query.exec();
}

void ImageProvider::deleteSemester(qint64 semesterNumber) {

    if (root.childrenCount < semesterNumber) {
        return;
    }

    QSqlQuery query;
    query.prepare("DELETE FROM IMAGES WHERE id = :id" );
    query.bindValue(":id", root.children[semesterNumber-1]->id);
    query.exec();
}

qint64 ImageProvider::getChildrenCount(qint64 id) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM IMAGES WHERE pid = :id" );
    query.bindValue(":id", id);
    query.exec();
    query.next();
    return query.value(0).toInt();
}
