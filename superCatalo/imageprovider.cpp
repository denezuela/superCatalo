#include "imageprovider.h"
#include <QDebug>
#include <QSqlQuery>
#include <QModelIndex>
#include <QImage>
#include <QPixmap>
#include <QUrl>
#include <QSize>

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

    const DataWrapper* ptr = dataForIndex(index);

    if (role == Qt::DisplayRole) {

        switch (ptr->type) {
            case SEMESTER:
            case COURSE:
            case THEME:
            {
                return ptr->data->comments;
                break;
            }
            default:
                break;
        }

        return ptr->data->path;
    }

    if (role == Qt::DecorationRole) {

        if (ptr->type == IMAGE) {
            QImage image(ptr->data->path);
            if (!image.isNull()) {
                return QUrl::fromLocalFile(ptr->data->path);
            }
            else {
                qDebug() << ptr->data->path <<"Image is null";
            }
        }
    }

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

    return createIndex(ptr->parent_pointer->row, 0, static_cast<void*>(ptr->parent_pointer));
}

void ImageProvider::fetchAll(const QModelIndex &parent) {

    DataWrapper* parentDataWrapper = dataForIndex(parent);
    parentDataWrapper->children.clear();

    QSqlQuery query;
    query.prepare("SELECT * FROM IMAGES WHERE PID=:id ORDER BY number");
    query.bindValue(":id", parentDataWrapper->id);
    query.exec();
    beginInsertRows(parent,0,parentDataWrapper->childrenCount-1);
    qint64 count = 0;
    while(query.next()) {
      DataWrapper* childWrapper = new DataWrapper;
      dbData* childData = new dbData;

      childData->comments = query.value("comment").toString();
      childData->pid = query.value("pid").toInt();
      childData->tags = query.value("tags").toStringList();
      childData->path = query.value("path").toString();

      childWrapper->row = count++;
      childWrapper->number = query.value("number").toInt();
      childWrapper->data = childData;
      childWrapper->children = {};
      childWrapper->id = query.value("id").toInt();
      childWrapper->parent_pointer = parentDataWrapper;

      childWrapper->childrenCount = getChildrenCount(childWrapper->id);

      QString type = query.value("type").toString();
      if (type == "semester")
        childWrapper->type = SEMESTER;
      else if (type  == "course")
        childWrapper->type = COURSE;
      else if (type  == "theme")
        childWrapper->type = THEME;
      else if (type == "image")
        childWrapper->type = IMAGE;

      parentDataWrapper->children.append(childWrapper);
    }
     endInsertRows();
}

ImageProvider::~ImageProvider() {
    db.close();
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
        if (ptr->type == SEMESTER) {
            setDataSemester(value.toInt());
        }
        if (ptr->type == COURSE) {
            QModelIndex parent = this->parent(index);
            setDataCourse(parent, value.toString());
        }
        if (ptr->type == THEME) {
            QModelIndex parent = this->parent(index);
            setDataTheme(parent, value.toString());
        }
        if (ptr->type == IMAGE) {
            dbData data = value.value<dbData>();
            QModelIndex parent = this->parent(index);
            setDataImage(parent, data.path, data.comments, data.tags);
        }
        emit dataChanged(index, index);
        return true;
    }

    return false;
}


bool ImageProvider::insertRows(int row, int count, const QModelIndex &parent) {

    qDebug() << "Insert rows";

    DataWrapper* ptr;
    if (!parent.isValid())
        ptr = &root;
    else
        ptr = dataForIndex(parent);

    this->beginInsertRows(parent, row, row+count-1);

    dbData* newData;
    DataWrapper* childWrapper;

    for (int i = 0; i < count; ++i) {
        newData = new dbData;
        newData->pid = ptr->id;
        newData->comments = "";

        childWrapper = new DataWrapper;
        childWrapper->data = newData;
        childWrapper->parent_pointer = ptr;

        if (ptr->type == ROOT)
          childWrapper->type = SEMESTER;
        else if (ptr->type == SEMESTER)
          childWrapper->type = COURSE;
        else if (ptr->type == COURSE)
          childWrapper->type = THEME;
        else
          childWrapper->type = IMAGE;

        ptr->children.append(childWrapper);
        ptr->childrenCount += 1;
    }

    this->endInsertRows();
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

    DataWrapper* childWrapper = root.children[root.childrenCount-1];
    childWrapper->id = id;
    childWrapper->children.clear();
    childWrapper->childrenCount = 0;
    childWrapper->number = semesterNumber;
    childWrapper->row = root.childrenCount-1;

    childWrapper->data->comments = QString::number(semesterNumber) + " semester";
    childWrapper->data->path = "NO_PATH";
    childWrapper->data->tags.append("NO_TAGS");
}


void ImageProvider::addSemester(qint64 semesterNumber) {
    qDebug() << "addSemester";
    if (containsSemesterAlready(root.children, semesterNumber)) {
        qDebug() << "contains semester already";
        return;
    }

    QModelIndex rootIndex = createIndex(0, 0, &root);

    insertRows(this->rowCount(rootIndex), 1, QModelIndex());

    QModelIndex index = createIndex(this->rowCount(rootIndex), 0, root.children[this->rowCount(rootIndex)-1]);
    this->setData(index, semesterNumber, Qt::EditRole);
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

    DataWrapper* childWrapper = ptr->children[ptr->children.size()-1];
    childWrapper->id = id;
    childWrapper->children.clear();
    childWrapper->childrenCount = 0;
    childWrapper->number = ptr->childrenCount;
    childWrapper->row = ptr->childrenCount-1;

    childWrapper->data->comments = courseName;
    childWrapper->data->path = "NO_PATH";
    childWrapper->data->tags.append(courseName);
}

void ImageProvider::addCourse(qint64 semesterNumber, QString courseName) {
    qDebug() << "addCourse";

    if (!containsSemesterAlready(root.children, semesterNumber)) {
        qDebug() << "not contains semester";
        this->addSemester(semesterNumber);
    }

    DataWrapper* semesterPtr = this->findFromNumber(root.children, semesterNumber);
    QModelIndex semesterIndex = index(semesterPtr->row, 0, QModelIndex());
    this->fetchMoreWithoutShowing(semesterIndex);
    if (containsCourseAlready(semesterPtr->children, courseName)) {
        qDebug() << "contains course already";
        return;
    }

    insertRows(this->rowCount(semesterIndex), 1, semesterIndex);

    QModelIndex index = createIndex(this->rowCount(semesterIndex), 0,
                                    dataForIndex(semesterIndex)->children[this->rowCount(semesterIndex)-1]);
    this->setData(index, courseName, Qt::EditRole);
}

void ImageProvider::setDataTheme(const QModelIndex &parent, QString themeName) {

    qDebug() << "setDataTheme";

    DataWrapper* ptr = dataForIndex(parent);

    QSqlQuery queryForInsert;
    queryForInsert.prepare("INSERT INTO IMAGES (pid, number, path, comment, tags, type) VALUES (:pid, :number, :path, :comment, :tags, :type)" );
    queryForInsert.bindValue(":pid", ptr->id);
    queryForInsert.bindValue(":number", ptr->childrenCount);
    queryForInsert.bindValue(":path", "NO_PATH");
    queryForInsert.bindValue(":comment", themeName);
    queryForInsert.bindValue(":tags", themeName);
    queryForInsert.bindValue(":type", "theme");
    queryForInsert.exec();

    QSqlQuery queryForSelect;
    queryForSelect.prepare("SELECT id FROM IMAGES WHERE pid=:pid and number=:number and path=:path and comment=:comment and tags=:tags and type=:type");
    queryForSelect.bindValue(":pid", ptr->id);
    queryForSelect.bindValue(":number", ptr->childrenCount);
    queryForSelect.bindValue(":path", "NO_PATH");
    queryForSelect.bindValue(":comment", themeName);
    queryForSelect.bindValue(":tags", themeName);
    queryForSelect.bindValue(":type", "theme");
    queryForSelect.exec();
    queryForSelect.next();

    qint64 id = queryForSelect.value(0).toInt();

    DataWrapper* childWrapper = ptr->children[ptr->children.size()-1];
    childWrapper->id = id;
    childWrapper->children.clear();
    childWrapper->childrenCount = 0;
    childWrapper->number = ptr->childrenCount;
    childWrapper->row = ptr->childrenCount-1;

    childWrapper->data->comments = themeName;
    childWrapper->data->path = "NO_PATH";
    childWrapper->data->tags.append(themeName);
}

void ImageProvider::addTheme(qint64 semesterNumber, QString courseName, QString themeName) {
    qDebug() << "addTheme";

    if (!containsSemesterAlready(root.children, semesterNumber)) {
        qDebug() << "not contains semester";
        this->addSemester(semesterNumber);
    }

    DataWrapper* semesterPtr = this->findFromNumber(root.children, semesterNumber);
    QModelIndex semesterIndex = index(semesterPtr->row, 0, QModelIndex());
    this->fetchMoreWithoutShowing(semesterIndex);
    if (!containsCourseAlready(semesterPtr->children, courseName)) {
        qDebug() << "not contains course";
        this->addCourse(semesterNumber, courseName);
    }

    DataWrapper* coursePtr = this->findFromName(semesterPtr->children, courseName);
    QModelIndex courseIndex = index(coursePtr->row, 0, semesterIndex);
    this->fetchMoreWithoutShowing(courseIndex);
    if (containsCourseAlready(coursePtr->children, themeName)) {
        qDebug() << "contains theme already";
        return;
    }

    insertRows(this->rowCount(courseIndex), 1, courseIndex);

    QModelIndex index = createIndex(this->rowCount(courseIndex), 0,
                                    dataForIndex(courseIndex)->children[this->rowCount(courseIndex)-1]);
    this->setData(index, themeName, Qt::EditRole);
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

    DataWrapper* childWrapper = ptr->children[ptr->children.size()-1];
    childWrapper->id = id;
    childWrapper->children.clear();
    childWrapper->childrenCount = 0;
    childWrapper->number = ptr->childrenCount;
    childWrapper->row = ptr->childrenCount-1;

    childWrapper->data->comments = comments;
    childWrapper->data->path = path;
    childWrapper->data->tags = tags;
}

void ImageProvider::addImage(qint64 semesterNumber, QString courseName, QString themeName, QString path, QString comments, QStringList tags) {
    qDebug() << "addImage";

    if (!containsSemesterAlready(root.children, semesterNumber)) {
        qDebug() << "not contains semester";
        addSemester(semesterNumber);
    }

    DataWrapper* semesterPtr = this->findFromNumber(root.children, semesterNumber);
    QModelIndex semesterIndex = index(semesterPtr->row, 0, QModelIndex());
    this->fetchMoreWithoutShowing(semesterIndex);
    if (!containsCourseAlready(semesterPtr->children, courseName)) {
        qDebug() << "not contains course";
        addCourse(semesterNumber, courseName);
    }

    DataWrapper* coursePtr = this->findFromName(semesterPtr->children, courseName);
    QModelIndex courseIndex = index(coursePtr->row, 0, semesterIndex);
    this->fetchMoreWithoutShowing(courseIndex);
    if (!containsCourseAlready(coursePtr->children, themeName)) {
        qDebug() << "not contains theme";
        addTheme(semesterNumber, courseName, themeName);
    }

    coursePtr = this->findFromName(semesterPtr->children, courseName);
    courseIndex = index(coursePtr->row, 0, semesterIndex);
    DataWrapper* themePtr = this->findFromName(coursePtr->children, themeName);
    QModelIndex themeIndex = index(themePtr->row, 0, courseIndex);
    this->fetchMoreWithoutShowing(themeIndex);
    if (containsPathAlready(themePtr->children, path)) {
        qDebug() << "contains image already";
        return;
    }

    dbData data;
    data.comments = comments;
    data.path = path;
    data.tags = tags;

    QVariant param = QVariant::fromValue(data);

    insertRows(this->rowCount(themeIndex), 1, themeIndex);

    QModelIndex index = createIndex(this->rowCount(themeIndex), 0,
                                    dataForIndex(themeIndex)->children[this->rowCount(themeIndex)-1]);
    this->setData(index, param, Qt::EditRole);
}


bool ImageProvider::removeRows(int row, int count, const QModelIndex &parent) {

    qDebug() << "Remove rows" << count << "from" << row;

    DataWrapper* ptr;
    if (!parent.isValid())
        ptr = &root;
    else
        ptr = dataForIndex(parent);

   beginRemoveRows(parent, row, row+count-1);

    for (qint64 i = 0; i < count; ++i) {
        ptr->children.removeAt(row);
        ptr->childrenCount--;
    }

    for (qint64 i = 0; i < ptr->children.size(); ++i) {
        ptr->children[i]->row = i;
    }

    endRemoveRows();

    return true;
}

void ImageProvider::deleteImage(qint64 semesterNumber, QString courseName, QString themeName, QString path) {
    qDebug() << "Delete image";

    DataWrapper* semesterPtr = findFromNumber(root.children, semesterNumber);
    if (semesterPtr == nullptr) {
        qDebug() << "No such semester";
        return;
    }
//    QModelIndex rootIndex = createIndex(0, 0, &root);
    QModelIndex semesterIndex = index(semesterPtr->row, 0, QModelIndex());
    this->fetchMoreWithoutShowing(semesterIndex);

    DataWrapper* coursePtr = findFromName(semesterPtr->children, courseName);
    if (coursePtr == nullptr) {
        qDebug() << "No such course";
        return;
    }
    QModelIndex courseIndex = index(coursePtr->row, 0, semesterIndex);
    this->fetchMoreWithoutShowing(courseIndex);

    DataWrapper* themePtr = findFromName(coursePtr->children, themeName);
    if (themePtr == nullptr) {
        qDebug() << "No such theme";
        return;
    }
    QModelIndex themeIndex = index(themePtr->row, 0, courseIndex);
    this->fetchMoreWithoutShowing(themeIndex);

    DataWrapper* imagePtr = findFromPath(themePtr->children, path);
    if (imagePtr == nullptr) {
        qDebug() << "No such image";
        return;
    }

    removeDataFromDb(imagePtr->id);

    this->removeRows(imagePtr->row, 1, themeIndex);
}

void ImageProvider::deleteTheme(qint64 semesterNumber, QString courseName, QString themeName) {
    qDebug() << "Delete theme";

    DataWrapper* semesterPtr = findFromNumber(root.children, semesterNumber);
    if (semesterPtr == nullptr) {
        qDebug() << "No such semester";
        return;
    }
//    QModelIndex rootIndex = createIndex(0, 0, &root);
    QModelIndex semesterIndex = index(semesterPtr->row, 0, QModelIndex());
    this->fetchMoreWithoutShowing(semesterIndex);

    DataWrapper* coursePtr = findFromName(semesterPtr->children, courseName);
    if (coursePtr == nullptr) {
        qDebug() << "No such course";
        return;
    }
    QModelIndex courseIndex = index(coursePtr->row, 0, semesterIndex);
    this->fetchMoreWithoutShowing(courseIndex);

    DataWrapper* themePtr = findFromName(coursePtr->children, themeName);
    if (themePtr == nullptr) {
        qDebug() << "No such theme";
        return;
    }

    removeDataFromDb(themePtr->id);

    this->removeRows(themePtr->row, 1, courseIndex);
}

void ImageProvider::deleteCourse(qint64 semesterNumber, QString courseName) {
    qDebug() << "Delete course " << courseName;

    DataWrapper* semesterPtr = findFromNumber(root.children, semesterNumber);
    if (semesterPtr == nullptr) {
        qDebug() << "No such semester";
        return;
    }
    //QModelIndex rootIndex = createIndex(0, 0, &root);
    QModelIndex semesterIndex = index(semesterPtr->row, 0, QModelIndex());

    DataWrapper* coursePtr = findFromName(semesterPtr->children, courseName);
    if (coursePtr == nullptr) {
        qDebug() << "No such course";
        return;
    }
    QModelIndex courseIndex = index(coursePtr->row, 0, semesterIndex);

    this->removeRows(0, coursePtr->childrenCount, courseIndex);
    for (int i = 0; i < coursePtr->children.size(); ++i) {
        removeDataFromDb(coursePtr->children[i]->id);
    }

    removeDataFromDb(coursePtr->id);
    this->removeRows(coursePtr->row, 1, semesterIndex);
}

void ImageProvider::deleteSemester(qint64 semesterNumber) {
    qDebug() << "Delete semester " << semesterNumber;

    DataWrapper* semesterPtr = findFromNumber(root.children, semesterNumber);
    if (semesterPtr == nullptr) {
        qDebug() << "No such semester";
        return;
    }
    qDebug() << "With row" << semesterPtr->row;
    QModelIndex semesterIndex = index(semesterPtr->row, 0, QModelIndex());

    DataWrapper* coursePtr;
    for (qint64 i = 0; i < semesterPtr->children.size(); ++i) {
        coursePtr = semesterPtr->children[i];
        QModelIndex courseIndex = index(coursePtr->row, 0, semesterIndex);

        this->removeRows(0, coursePtr->childrenCount, courseIndex);
        for (qint64 j = 0; j < coursePtr->children.size(); ++j) {
            removeDataFromDb(coursePtr->children[j]->id);
        }

        removeDataFromDb(coursePtr->id);
    }
    this->removeRows(0, semesterPtr->childrenCount, semesterIndex);

    this->removeRows(semesterPtr->row, 1, QModelIndex());
    removeDataFromDb(semesterPtr->id);

    beginResetModel();
    endResetModel();
}

void ImageProvider::removeDataFromDb(qint64 id) {
    QSqlQuery queryForDelete;
    queryForDelete.prepare("DELETE FROM IMAGES WHERE id = :id");
    queryForDelete.bindValue(":id", id);
    queryForDelete.exec();
    queryForDelete.next();
}

void ImageProvider::addTags(qint64 semesterNumber, QString courseName, QString themeName, QString path, QStringList tags) {
    DataWrapper* semesterPtr = findFromNumber(root.children, semesterNumber);
    if (semesterPtr == nullptr) {
        qDebug() << "No such semester";
        return;
    }
    QModelIndex semesterIndex = index(semesterPtr->row, 0, QModelIndex());
    this->fetchMoreWithoutShowing(semesterIndex);

    DataWrapper* coursePtr = findFromName(semesterPtr->children, courseName);
    if (coursePtr == nullptr) {
        qDebug() << "No such course";
        return;
    }
    QModelIndex courseIndex = index(coursePtr->row, 0, semesterIndex);
    this->fetchMoreWithoutShowing(courseIndex);

    DataWrapper* themePtr = findFromName(coursePtr->children, themeName);
    if (themePtr == nullptr) {
        qDebug() << "No such theme";
        return;
    }
    QModelIndex themeIndex = index(themePtr->row, 0, courseIndex);
    this->fetchMoreWithoutShowing(themeIndex);

    DataWrapper* imagePtr = findFromPath(themePtr->children, path);

    imagePtr->data->tags.append(tags);
}

//private functions

bool ImageProvider::containsSemesterAlready (QList<DataWrapper*> children, qint64 number) {

    for (auto it = children.begin(); it < children.end(); ++it)
    {
        if ((*it)->number == number)
            return true;
    }

    return false;
}

bool ImageProvider::containsCourseAlready (QList<DataWrapper*> children, QString comments) {

    for (auto it = children.begin(); it < children.end(); ++it)
    {
        if ((*it)->data->comments == comments)
            return true;
    }

    return false;
}

bool ImageProvider::containsPathAlready (QList<DataWrapper*> children, QString path) {

    for (auto it = children.begin(); it < children.end(); ++it)
    {
        if ((*it)->data->path == path)
            return true;
    }

    return false;
}

DataWrapper* ImageProvider::findFromNumber (QList<DataWrapper*> children, qint64 number) {
    qDebug() << "findFromNumber " << number;

    for (auto it = children.begin(); it != children.end(); ++it)
    {
        if ((*it)->number == number)
            return *it;
    }

    return nullptr;
}

DataWrapper* ImageProvider::findFromName (QList<DataWrapper*> children, QString name) {

    for (auto it = children.begin(); it != children.end(); ++it)
    {
        if ((*it)->data->comments == name)
            return *it;
    }

    return nullptr;
}

DataWrapper* ImageProvider::findFromPath (QList<DataWrapper*> children, QString path) {

    for (auto it = children.begin(); it != children.end(); ++it)
    {
        if ((*it)->data->path == path)
            return *it;
    }

    return nullptr;
}

void ImageProvider::recountRowNumbers(QList<DataWrapper *> children) {
    qDebug() << "recountRowNumbers";

    for (qint64 i = 0; i < children.size(); ++i) {
            children[i]->row = i;
        }
}

bool ImageProvider::hasChildren(const QModelIndex &parent) const
{
    const DataWrapper* ptr = dataForIndex(parent);
    return ptr->childrenCount!=0;
}

void ImageProvider::fetchMoreWithoutShowing(const QModelIndex &parent) {
    DataWrapper* parentDataWrapper = dataForIndex(parent);
    parentDataWrapper->children.clear();

    QSqlQuery query;
    query.prepare("SELECT * FROM IMAGES WHERE PID=:id ORDER BY number");
    query.bindValue(":id", parentDataWrapper->id);
    query.exec();
    qint64 count = 0;
    while(query.next()) {
      DataWrapper* childWrapper = new DataWrapper;
      dbData* childData = new dbData;

      childData->comments = query.value("comment").toString();
      childData->pid = query.value("pid").toInt();
      childData->tags = query.value("tags").toStringList();
      childData->path = query.value("path").toString();

      childWrapper->row = count++;
      childWrapper->number = query.value("number").toInt();
      childWrapper->data = childData;
      childWrapper->children = {};
      childWrapper->id = query.value("id").toInt();
      childWrapper->parent_pointer = parentDataWrapper;

      childWrapper->childrenCount = getChildrenCount(childWrapper->id);

      QString type = query.value("type").toString();
      if (type == "semester")
        childWrapper->type = SEMESTER;
      else if (type  == "course")
        childWrapper->type = COURSE;
      else if (type  == "theme")
        childWrapper->type = THEME;
      else if (type == "image")
        childWrapper->type = IMAGE;

      parentDataWrapper->children.append(childWrapper);
    }
}


