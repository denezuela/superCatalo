#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QObject>
#include <QAbstractItemModel>
#include <QSqlDatabase>
#include <QDebug>
#include <QString>

class ImageProvider : public QAbstractItemModel
{
    Q_OBJECT

    struct Node {
        qint64 id;
        qint64 pid;
        QString path;
        QString text;
        QString comments;
        QString tags;
    };

public:
    explicit ImageProvider(QAbstractItemModel *parent = 0);
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QModelIndex index(int row, int column, const QModelIndex &index) const;
    virtual int rowCount(const QModelIndex &index) const;
    virtual int columnCount(const QModelIndex &index) const;
    virtual QModelIndex parent(const QModelIndex &index) const;

signals:

public slots:
};

#endif // IMAGEPROVIDER_H
