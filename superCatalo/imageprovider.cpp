#include "imageprovider.h"
#include <QDebug>
#include <QSqlQuery>
#include <QModelIndex>
#include <QImage>
#include <QPixmap>
#include <QUrl>
#include <QSize>
#include <QPrinter>
#include <QPainter>
#include <QPrintDialog>
#include <QPixmap>

ImageProvider::ImageProvider(QAbstractItemModel *parent) : QAbstractItemModel(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName("localhost");
    db.setDatabaseName("../DB");
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
      QString tagString = query.value("tags").toString();
      childData->tags = tagString.split(",");
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
    if (!this->currentIndex.isValid()) this->currentIndex = createIndex(0,0,&root);

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

void ImageProvider::addCourse(QString courseName) {
    if (!this->currentIndex.isValid()) return;

    qDebug() << "add course";

    DataWrapper* ptr = dataForIndex(this->currentIndex);
    if (containsCourseAlready(ptr->children, courseName)) {
            qDebug() << "contains course already";
            return;
        }

    insertRows(this->rowCount(this->currentIndex), 1, this->currentIndex);

    QModelIndex index = createIndex(this->rowCount(this->currentIndex), 0,
                                    ptr->children[this->rowCount(this->currentIndex)-1]);
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

void ImageProvider::addTheme(QString themeName) {
    if (!this->currentIndex.isValid()) return;

    qDebug() << "add theme";

    DataWrapper* ptr = dataForIndex(this->currentIndex);
    if (containsCourseAlready(ptr->children, themeName)) {
            qDebug() << "contains theme already";
            return;
        }

    insertRows(this->rowCount(this->currentIndex), 1, this->currentIndex);

    QModelIndex index = createIndex(this->rowCount(this->currentIndex), 0,
                                    ptr->children[this->rowCount(this->currentIndex)-1]);
    this->setData(index, themeName, Qt::EditRole);
}


void ImageProvider::setDataImage(const QModelIndex &parent, QString path, QString comments, QStringList tags) {

    qDebug() << "setDataImage";

    DataWrapper* ptr = dataForIndex(parent);
    QString tagString = tags.join(",");

    QSqlQuery queryForInsert;
    queryForInsert.prepare("INSERT INTO IMAGES (pid, number, path, comment, tags, type) VALUES (:pid, :number, :path, :comment, :tags, :type)" );
    queryForInsert.bindValue(":pid", ptr->id);
    queryForInsert.bindValue(":number", ptr->childrenCount);
    queryForInsert.bindValue(":path", path);
    queryForInsert.bindValue(":comment", comments);
    queryForInsert.bindValue(":tags", tagString);
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

void ImageProvider::addImage(QString path, QString comments, QString tags) {
    if (!this->currentIndex.isValid()) return;

    qDebug() << "add image";

    DataWrapper* ptr = dataForIndex(this->currentIndex);
    if (containsPathAlready(ptr->children, path)) {
            qDebug() << "contains image already";
            return;
        }

    dbData data;
    data.comments = comments;
    data.path = path;
    data.tags = tags.split(',');

    QVariant param = QVariant::fromValue(data);

    insertRows(this->rowCount(this->currentIndex), 1, this->currentIndex);

    QModelIndex index = createIndex(this->rowCount(this->currentIndex), 0,
                                    ptr->children[this->rowCount(this->currentIndex)-1]);
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


void ImageProvider::deleteItem() {
    if (!this->currentIndex.isValid()) return;

    this->fetchMore(this->currentIndex);

    DataWrapper* ptr = dataForIndex(this->currentIndex);
    qDebug() << "delete" << ptr->type << "number" << ptr->number;

    QModelIndex parent = this->currentIndex.parent();
    if (ptr->type == SEMESTER)
        parent = QModelIndex();

    if (ptr->childrenCount > 0) {

        DataWrapper* firstLevelSon;
        DataWrapper* secondLevelSon;
        QModelIndex firstLevelIndex, secondlevelIndex;

        for (qint64 i = 0; i < ptr->children.size(); ++i) {
            firstLevelSon = ptr->children[i];
            firstLevelIndex = index(firstLevelSon->row, 0, this->currentIndex);
            this->fetchMore(firstLevelIndex);

            if (firstLevelSon->childrenCount > 0) {
                for (qint64 j = 0; j < firstLevelSon->children.size(); ++j) {
                    secondLevelSon = firstLevelSon->children[j];
                    secondlevelIndex = index(secondLevelSon->row, 0, firstLevelIndex);
                    this->fetchMore(secondlevelIndex);

                    removeRows(0, secondLevelSon->childrenCount, secondlevelIndex);
                    for (qint64 k = 0; k < secondLevelSon->children.size(); ++k)
                        removeDataFromDb(secondLevelSon->children[k]->id);

                    removeDataFromDb(secondLevelSon->id);
                }

                removeRows(0, firstLevelSon->childrenCount, firstLevelIndex);
            }

            removeRows(0, ptr->childrenCount, this->currentIndex);
            removeDataFromDb(firstLevelSon->id);
        }
    }

    removeRows(ptr->row, 1, parent);
    removeDataFromDb(ptr->id);
}

void ImageProvider::removeDataFromDb(qint64 id) {
    QSqlQuery queryForDelete;
    queryForDelete.prepare("DELETE FROM IMAGES WHERE id = :id");
    queryForDelete.bindValue(":id", id);
    queryForDelete.exec();
    queryForDelete.next();
}

void ImageProvider::addTags(QString tags) {
    if (!this->currentIndex.isValid()) return;

    DataWrapper* imagePtr = dataForIndex(this->currentIndex);
    if (imagePtr->type != IMAGE)
        return;

    QStringList tagsList = tags.split(',');
    imagePtr->data->tags.append(tagsList);
    tags = imagePtr->data->tags.join(',');

    QSqlQuery query;
    query.prepare("UPDATE IMAGES SET tags=:tags WHERE ID=:id");
    query.bindValue(":id", imagePtr->id);
    query.bindValue(":tags", imagePtr->data->tags.join(","));
    query.exec();
}

void ImageProvider::setComment(QString comment) {
    if (!this->currentIndex.isValid()) return;

    DataWrapper* imagePtr = dataForIndex(this->currentIndex);
    if (imagePtr->type != IMAGE)
        return;

    imagePtr->data->comments = comment;

    QSqlQuery query;
    query.prepare("UPDATE IMAGES SET comment=:comment WHERE ID=:id");
    query.bindValue(":id", imagePtr->id);
    query.bindValue(":tags", imagePtr->data->comments);
    query.exec();
}

void ImageProvider::setCurrentIndex (const QModelIndex &currentIndex) {
    this->currentIndex = currentIndex;
    DataWrapper* ptr = dataForIndex(this->currentIndex);
}

bool ImageProvider::showMenuItem (const QModelIndex &index, qint64 type) {
    if (!index.isValid()) return false;

    DataWrapper* ptr = dataForIndex(index);
    return (ptr->type == type);
}

QString ImageProvider::showTags() {
    if (!this->currentIndex.isValid()) return "NO_TAGS";

    DataWrapper* imagePtr = dataForIndex(this->currentIndex);
    if (imagePtr->type != IMAGE)
        return "NO_TAGS";

    return imagePtr->data->tags.join(",");
}

QString ImageProvider::showComment() {
    if (!this->currentIndex.isValid()) return "NO_COMMENT";

    DataWrapper* imagePtr = dataForIndex(this->currentIndex);
    if (imagePtr->type != IMAGE)
        return "NO_COMMENT";

    return imagePtr->data->comments;
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

//void ImageProvider::recountRowNumbers(QList<DataWrapper *> children) {
//    qDebug() << "recountRowNumbers";

//    for (qint64 i = 0; i < children.size(); ++i) {
//            children[i]->row = i;
//        }
//}

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

void ImageProvider::print(QUrl data)

{
    QPixmap pix;
    pix.load(data.toLocalFile());
    QPrinter printer;
          QPrintDialog *dlg = new QPrintDialog(&printer,0);
          if(dlg->exec() == QDialog::Accepted) {
                  QPainter painter(&printer);
                  painter.drawPixmap(QPoint(0, 0), pix);
                  painter.end();
          }
}

QVariantList ImageProvider::getChildrenIndexes() {
    if (!this->currentIndex.isValid()) return {};

    QVariantList indexes;
    indexes.push_back(this->currentIndex);
    DataWrapper* ptr = dataForIndex(this->currentIndex);

    if (ptr->childrenCount > 0) {

        QModelIndex firstLevelIndex, secondlevelIndex, thirdLevelIndex;
        DataWrapper *firstLevelSon, *secondLevelSon, *thirdLevelSon;

        for (qint64 i = 0; i < ptr->children.size(); ++i) {
            firstLevelSon = ptr->children[i];
            firstLevelIndex = index(firstLevelSon->row, 0, this->currentIndex);
            indexes.push_back(firstLevelIndex);

            if (firstLevelSon->childrenCount > 0) {
                for (qint64 j = 0; j < firstLevelSon->children.size(); ++j) {
                    secondLevelSon = firstLevelSon->children[j];
                    secondlevelIndex = index(secondLevelSon->row, 0, firstLevelIndex);
                    indexes.push_back(secondlevelIndex);

                    if (secondLevelSon->childrenCount > 0) {
                        for (qint64 k = 0; k < secondLevelSon->children.size(); ++k) {
                            thirdLevelSon = firstLevelSon->children[j];
                            thirdLevelIndex = index(thirdLevelSon->row, 0, firstLevelIndex);
                            indexes.push_back(thirdLevelIndex);
                        }
                    }
                }
            }
        }
    }

    return indexes;
}

void ImageProvider::findByTags (QString _tags) {
    QStringList tags = _tags.split(',');

    QVariantList result;

    for (qint64 i = 0; i < root.children.size(); ++i) {
        DataWrapper* fls = root.children[i];
        QModelIndex flsIndex = index(fls->row,0,QModelIndex());
        this->fetchMore(flsIndex);

        for (qint64 j = 0; j < fls->children.size(); ++j) {
            DataWrapper* sls = fls->children[j];
            QModelIndex slsIndex = index(sls->row,0,flsIndex);
            this->fetchMore(slsIndex);

            for (qint64 k = 0; k < sls->children.size(); ++k) {
                DataWrapper* tls = sls->children[k];
                QModelIndex tlsIndex = index(tls->row,0,slsIndex);
                this->fetchMore(tlsIndex);

                for (qint64 l = 0; l < tls->children.size(); ++l) {
                    DataWrapper* fthls = tls->children[l];
                    QModelIndex fthlsIndex = index(fthls->row,0,tlsIndex);
                    this->fetchMore(fthlsIndex);

                    if (this->include(fthls->data->tags, tags)) {
                        result.push_back(QUrl::fromLocalFile(fthls->data->path));
                    }
                }
            }
        }
    }

   this->currentImages = result;
}

bool ImageProvider::include(const QStringList &big, const QStringList &small) {
    for (qint64 i = 0; i < small.size(); ++i) {
        if (big.contains(small[i]))
            return true;
    }

    return false;
}

QVariant ImageProvider::fetchImage (qint64 number) {
    return this->currentImages[number];
}
