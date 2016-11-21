#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QObject>
#include <QAbstractItemModel>
#include <QSqlDatabase>
#include <QString>
#include <QList>
#include <QStringList>

enum dataType {ROOT, SEMESTER, COURSE, IMAGE};

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
        bool contains (QList<DataWrapper*> children, qint64 number);
        bool contains (QList<DataWrapper*> children, QString comments);
        DataWrapper* findFromNumber (QList<DataWrapper*> children, qint64 number);

    public:
        ImageProvider(QAbstractItemModel *parent = 0);
        ~ImageProvider();
        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
        virtual int rowCount(const QModelIndex &index) const;
        virtual int columnCount(const QModelIndex &index) const;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
        virtual QModelIndex parent(const QModelIndex &index) const;
        void fetchAll(const QModelIndex &parent);
        const DataWrapper* dataForIndex(const QModelIndex &parent) const;
        DataWrapper* dataForIndex(const QModelIndex &parent);
        void fetchMore(const QModelIndex &parent);
        bool canFetchMore(const QModelIndex &parent) const;
        qint64 getChildrenCount(qint64 id);

        virtual bool insertRows(int row, int count, const QModelIndex &parent);
        void setDataSemester(qint64 semesterNumber);
        void setDataCourse(const QModelIndex &parent, QString courseName);

        void addImage(qint64 semesterNumber, QString courseName, QString imagePath);
        void deleteSemester(qint64 semesterNumber);

        void addSemester(qint64 semesterNumber);
        void addCourse(qint64 semesterNumber, QString courseName);
    signals:

    public slots:
};

#endif // IMAGEPROVIDER_H
