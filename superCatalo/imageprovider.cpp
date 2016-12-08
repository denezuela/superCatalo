#include "imageprovider.h"
#include <QDebug>
#include <QSqlQuery>
#include <QModelIndex>

ImageProvider::ImageProvider(QAbstractItemModel *parent) : QAbstractItemModel(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName("localhost");
    db.setDatabaseName("/home/skt/sqlite/database");
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

    if (!hasIndex(row, column, parent)) {
        return {};
    }

    if (!parent.isValid())
        return createIndex(row, column, root.children[row]);

    const DataWrapper* ptr = dataForIndex(parent);

    if (ptr->type == IMAGE)
        return QModelIndex();

//    if (ptr->children.size() <= row)
//        return QModelIndex();

    DataWrapper* dataWrapperForIndex = ptr->children[row];
    QModelIndex result = createIndex(row, column, dataWrapperForIndex);
    return result;
}

int ImageProvider::rowCount(const QModelIndex &index) const {

    if (!index.isValid()) {
        return root.childrenCount;
    }

    const DataWrapper* ptr = dataForIndex(index);
    return ptr->children.count();
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

    return createIndex(ptr->parent_pointer->number - 1, 0, static_cast<void*>(ptr->parent_pointer));
}

void ImageProvider::fetchAll(const QModelIndex &parent) {

    DataWrapper* parentDataWrapper = dataForIndex(parent);
    parentDataWrapper->children.clear();

    QSqlQuery query;
    query.prepare("SELECT * FROM IMAGES WHERE PID=:id ORDER BY number");
    query.bindValue(":id", parentDataWrapper->id);
    query.exec();
    beginInsertRows(parent,0,parentDataWrapper->childrenCount-1);
    while(query.next()) {
      DataWrapper* childWrapper = new DataWrapper;
      dbData* childData = new dbData;

      childData->id = query.value("id").toInt();
      childData->comments = query.value("comment").toString();
      childData->number = query.value("number").toInt();
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
     endInsertRows();
}

ImageProvider::~ImageProvider() {
    db.close();
   // delete &root;
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

qint64 ImageProvider::getChildrenCount(qint64 id) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM IMAGES WHERE pid = :id" );
    query.bindValue(":id", id);
    query.exec();
    query.next();
    return query.value(0).toInt();
}

bool ImageProvider::setData(const QModelIndex &index, const QVariant &value, int role) {

    if (!index.isValid())
        return false;

    DataWrapper* ptr = dataForIndex(index);

    if (role == Qt::EditRole) {
        if (ptr->type == ROOT) {
            qDebug() << "Add semester";
            this->beginInsertRows(index, root.childrenCount, root.childrenCount);
            if (insertRows(root.childrenCount, 1, index)) {
                setDataSemester(value.toInt());
            }
            this->endInsertRows();
            return true;
        }
        if (ptr->type == SEMESTER) {
            qDebug() << "Add course";
            this->beginInsertRows(index, ptr->childrenCount, ptr->childrenCount);
            if (insertRows(ptr->childrenCount, 1, index)) {
                setDataCourse(index, value.toString());
            }
            this->endInsertRows();
            return true;
        }
        if (ptr->type == COURSE) {
            qDebug() << "Add image";
            this->beginInsertRows(index, ptr->childrenCount, ptr->childrenCount);
            if (insertRows(ptr->childrenCount, 1, index)) {
                dbData data = value.value<dbData>();
                setDataImage(index, data.path, data.comments, data.tags);
            }
            this->endInsertRows();
            return true;
        }
    }
    else return false;
}


bool ImageProvider::insertRows(int row, int count, const QModelIndex &parent) {

    qDebug() << "Insert rows";
    DataWrapper* ptr = dataForIndex(parent);

    dbData* newData = new dbData;
    newData->pid = ptr->id;
    if (ptr->type == ROOT)
        newData->type = "semester";
    if (ptr->type == SEMESTER)
        newData->type = "course";
    if (ptr->type == COURSE)
        newData->type = "image";

    DataWrapper* childWrapper = new DataWrapper;
    childWrapper->data = newData;
    childWrapper->parent_pointer = ptr;

    if (newData->type == "semester")
      childWrapper->type = SEMESTER;
    else if (newData->type == "course")
      childWrapper->type = COURSE;
    else
      childWrapper->type = IMAGE;

    ptr->children.append(childWrapper);
    ptr->childrenCount += 1;

    return true;
}


void ImageProvider::setDataSemester(qint64 semesterNumber) {

    qDebug() << "setDataSemester";
    QSqlQuery queryForInsert;
    queryForInsert.prepare("INSERT INTO IMAGES (pid, number, path, comment, tags, type) VALUES (:pid, :number, :path, :comment, :tags, :type)" );
    queryForInsert.bindValue(":pid", root.id);
    queryForInsert.bindValue(":number", semesterNumber);
    queryForInsert.bindValue(":path", "NO_PATH");
    queryForInsert.bindValue(":comment", QString::number(semesterNumber) + " semester");
    queryForInsert.bindValue(":tags", "NO_TAGS");
    queryForInsert.bindValue(":type", "semester");
    queryForInsert.exec();

    QSqlQuery queryForSelect;
    queryForSelect.prepare("SELECT id FROM IMAGES WHERE pid=:pid and number=:number and path=:path and comment=:comment and tags=:tags and type=:type");
    queryForSelect.bindValue(":pid", root.id);
    queryForSelect.bindValue(":number", semesterNumber);
    queryForSelect.bindValue(":path", "NO_PATH");
    queryForSelect.bindValue(":comment", QString::number(semesterNumber) + " semester");
    queryForSelect.bindValue(":tags", "NO_TAGS");
    queryForSelect.bindValue(":type", "semester");
    queryForSelect.exec();
    queryForSelect.next();

    qint64 id = queryForSelect.value(0).toInt();

    DataWrapper* childWrapper = root.children[root.children.size()-1];
    childWrapper->id = id;
    childWrapper->children.clear();
    childWrapper->childrenCount = 0;
    childWrapper->number = semesterNumber;

    childWrapper->data->comments = QString::number(semesterNumber) + " semester";
    childWrapper->data->path = "NO_PATH";
    childWrapper->data->tags.append("NO_TAGS");
    childWrapper->data->number = semesterNumber;
}


void ImageProvider::addSemester(qint64 semesterNumber) {
    qDebug() << "addSemester";
    if (containsSemesterAlready(root.children, semesterNumber)) {
        qDebug() << "contains semester already";
        return;
    }
    QModelIndex rootIndex = createIndex(0, 0, &root);
    this->setData(rootIndex, semesterNumber, Qt::EditRole);
}

void ImageProvider::setDataCourse(const QModelIndex &parent, QString courseName) {

    qDebug() << "setDataCourse";

    DataWrapper* ptr = dataForIndex(parent);

    QSqlQuery queryForInsert;
    queryForInsert.prepare("INSERT INTO IMAGES (pid, number, path, comment, tags, type) VALUES (:pid, :number, :path, :comment, :tags, :type)" );
    queryForInsert.bindValue(":pid", ptr->id);
    queryForInsert.bindValue(":number", ptr->childrenCount);
    queryForInsert.bindValue(":path", "NO_PATH");
    queryForInsert.bindValue(":comment", courseName);
    queryForInsert.bindValue(":tags", courseName);
    queryForInsert.bindValue(":type", "course");
    queryForInsert.exec();

    QSqlQuery queryForSelect;
    queryForSelect.prepare("SELECT id FROM IMAGES WHERE pid=:pid and number=:number and path=:path and comment=:comment and tags=:tags and type=:type");
    queryForSelect.bindValue(":pid", ptr->id);
    queryForSelect.bindValue(":number", ptr->childrenCount);
    queryForSelect.bindValue(":path", "NO_PATH");
    queryForSelect.bindValue(":comment", courseName);
    queryForSelect.bindValue(":tags", courseName);
    queryForSelect.bindValue(":type", "course");
    queryForSelect.exec();
    queryForSelect.next();

    qint64 id = queryForSelect.value(0).toInt();
    qDebug() << "addCourseId" << id;

    DataWrapper* childWrapper = ptr->children[ptr->children.size()-1];
    childWrapper->id = id;
    childWrapper->children.clear();
    childWrapper->childrenCount = 0;
    childWrapper->number = ptr->childrenCount;

    childWrapper->data->comments = courseName;
    childWrapper->data->path = "NO_PATH";
    childWrapper->data->tags.append(courseName);
    childWrapper->data->number = ptr->childrenCount;
}

void ImageProvider::addCourse(qint64 semesterNumber, QString courseName) {
    qDebug() << "addCourse";

    if (!containsSemesterAlready(root.children, semesterNumber)) {
        qDebug() << "not contains semester";
        this->addSemester(semesterNumber);
    }

    DataWrapper* semesterPtr = this->findFromNumber(root.children, semesterNumber);
    QModelIndex semesterIndex = createIndex(root.children.size(), 0, semesterPtr);
    this->fetchMore(semesterIndex);
    if (containsCourseAlready(semesterPtr->children, courseName)) {
        qDebug() << "contains course already";
        return;
    }

    this->setData(semesterIndex, courseName, Qt::EditRole);
}

void ImageProvider::setDataImage(const QModelIndex &parent, QString path, QString comments, QStringList tags) {

    qDebug() << "setDataImage";

    DataWrapper* ptr = dataForIndex(parent);

    QSqlQuery queryForInsert;
    queryForInsert.prepare("INSERT INTO IMAGES (pid, number, path, comment, tags, type) VALUES (:pid, :number, :path, :comment, :tags, :type)" );
    queryForInsert.bindValue(":pid", ptr->id);
    queryForInsert.bindValue(":number", ptr->childrenCount);
    queryForInsert.bindValue(":path", path);
    queryForInsert.bindValue(":comment", comments);
    queryForInsert.bindValue(":tags", tags);
    queryForInsert.bindValue(":type", "image");
    queryForInsert.exec();

    QSqlQuery queryForSelect;
    queryForSelect.prepare("SELECT id FROM IMAGES WHERE pid=:pid and number=:number and path=:path and comment=:comment and tags=:tags and type=:type");
    queryForSelect.bindValue(":pid", ptr->id);
    queryForSelect.bindValue(":number", ptr->childrenCount);
    queryForSelect.bindValue(":path",path);
    queryForSelect.bindValue(":comment", comments);
    queryForSelect.bindValue(":tags", tags);
    queryForSelect.bindValue(":type", "image");
    queryForSelect.exec();
    queryForSelect.next();

    qint64 id = queryForSelect.value(0).toInt();
    qDebug() << "addImageId" << id;

    DataWrapper* childWrapper = ptr->children[ptr->children.size()-1];
    childWrapper->id = id;
    childWrapper->children.clear();
    childWrapper->childrenCount = 0;
    childWrapper->number = ptr->childrenCount;

    childWrapper->data->comments = comments;
    childWrapper->data->path = path;
    childWrapper->data->tags = tags;
    childWrapper->data->number = ptr->childrenCount;
}

void ImageProvider::addImage(qint64 semesterNumber, QString courseName, QString path, QString comments, QStringList tags) {

    qDebug() << "addImage";
    if (!containsSemesterAlready(root.children, semesterNumber)) {
        qDebug() << "not contains semester";
        this->addSemester(semesterNumber);
    }

    DataWrapper* semesterPtr = this->findFromNumber(root.children, semesterNumber);
    QModelIndex semesterIndex = createIndex(semesterPtr->number - 1, 0, semesterPtr);
    this->fetchMore(semesterIndex);
    if (!containsCourseAlready(semesterPtr->children, courseName)) {
        qDebug() << "not contains course";
        this->addCourse(semesterNumber, courseName);
    }

    DataWrapper* coursePtr = this->findFromName(semesterPtr->children, courseName);
    QModelIndex courseIndex = createIndex(semesterPtr->childrenCount, 0, coursePtr);
    this->fetchMore(courseIndex);
    if (containsPathAlready(coursePtr->children, path)) {
        qDebug() << "contains image already";
        return;
    }

    dbData data;
    data.comments = comments;
    data.path = path;
    data.tags = tags;

    QVariant param = QVariant::fromValue(data);
    this->setData(courseIndex, param, Qt::EditRole);
}

//private functions

bool ImageProvider::containsSemesterAlready (QList<DataWrapper*> children, qint64 number) {

    for (auto it = children.begin(); it != children.end(); ++it)
    {
        if ((*it)->number == number)
            return true;
    }

    return false;
}

bool ImageProvider::containsCourseAlready (QList<DataWrapper*> children, QString comments) {

    for (auto it = children.begin(); it != children.end(); ++it)
    {
        if ((*it)->data->comments == comments)
            return true;
    }

    return false;
}

bool ImageProvider::containsPathAlready (QList<DataWrapper*> children, QString path) {

    for (auto it = children.begin(); it != children.end(); ++it)
    {
        if ((*it)->data->path == path)
            return true;
    }

    return false;
}

DataWrapper* ImageProvider::findFromNumber (QList<DataWrapper*> children, qint64 number) {

    for (auto it = children.begin(); it != children.end(); ++it)
    {
        if ((*it)->number == number)
            return *it;
    }

    qDebug()<<"Bad";
    return nullptr;
}

DataWrapper* ImageProvider::findFromName (QList<DataWrapper*> children, QString name) {

    for (auto it = children.begin(); it != children.end(); ++it)
    {
        if ((*it)->data->comments == name)
            return *it;
    }

    qDebug()<<"Bad";
    return nullptr;
}

bool ImageProvider::hasChildren(const QModelIndex &parent) const
{
    const DataWrapper* ptr = dataForIndex(parent);
    return ptr->childrenCount!=0;
}

