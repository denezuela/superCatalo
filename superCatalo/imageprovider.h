#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QObject>
#include <QAbstractItemModel>
#include <QSqlDatabase>
#include <QString>
#include <QList>
#include <QStringList>

enum dataType {ROOT, SEMESTER, COURSE, LECTURE, IMAGE};

struct dbData{
    qint64 id;
    qint64 pid;
    qint64 number;
    QString path;
    QString comments;
    QStringList tags;
    QString type;
};

struct DataWrapper {
    qint64 id;
    dataType type;
    dbData* data;
    qint64 number;
    DataWrapper* parent_pointer;
    QList<DataWrapper*> children;
    qint64 childrenCount;
};

class ImageProvider : public QAbstractItemModel
{
    private:
        Q_OBJECT
        QSqlDatabase db;
        DataWrapper root{0, ROOT, nullptr, 0, nullptr, {}, 0};

    public:
        ImageProvider(QAbstractItemModel *parent = 0);
        ~ImageProvider();
        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
        virtual int rowCount(const QModelIndex &index) const;
        virtual int columnCount(const QModelIndex &index) const;
        virtual QModelIndex parent(const QModelIndex &index) const;
        void fetchAll(const QModelIndex &parent);
        void forTests();

    signals:

    public slots:
};

#endif // IMAGEPROVIDER_H
