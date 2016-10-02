#include "imageprovider.h"

ImageProvider::ImageProvider(QAbstractItemModel *parent) : QAbstractItemModel(parent)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLLITE");
    db.setHostName("localhost");
    db.setDatabaseName("/home/skt/sqlite/db");
    bool db_ok = db.open();
}

QVariant ImageProvider::data (const QModelIndex &index, int role) const {
    return NULL;
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

