#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QObject>
#include <QAbstractItemModel>
#include <QSqlDatabase>
#include <QString>
#include <QList>
#include <QStringList>
#include <QModelIndex>


enum dataType {ROOT, SEMESTER, COURSE, IMAGE};

struct dbData{
//    qint64 id;
    qint64 pid;
//    qint64 number;
    QString path;
    QString comments;
    QStringList tags;
//    QString type;
};
Q_DECLARE_METATYPE(dbData)

struct DataWrapper {
    qint64 id;
    dataType type;
    dbData* data;
    qint64 number;
    qint64 row;
    DataWrapper* parent_pointer;
    QList<DataWrapper*> children;
    qint64 childrenCount;
};

class ImageProvider : public QAbstractItemModel
{
    Q_OBJECT

    private:
        QSqlDatabase db;
        DataWrapper root{0, ROOT, nullptr, 0, 0, nullptr, {}, 0};
        bool containsSemesterAlready (QList<DataWrapper*> children, qint64 number);
        bool containsCourseAlready (QList<DataWrapper*> children, QString comments);
        bool containsPathAlready (QList<DataWrapper*> children, QString path);
        DataWrapper* findFromNumber (QList<DataWrapper*> children, qint64 number);
        DataWrapper* findFromName (QList<DataWrapper*> children, QString name);
        DataWrapper* findFromPath (QList<DataWrapper*> children, QString path);
        void recountRowNumbers(QList<DataWrapper*> children);

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
        virtual bool removeRows(int row, int count, const QModelIndex &parent);
        void setDataSemester(qint64 semesterNumber);
        void setDataCourse(const QModelIndex &parent, QString courseName);
        void setDataImage(const QModelIndex &parent, QString path, QString comments, QStringList tags={});
        void removeDataFromDb(qint64 id);

Q_INVOKABLE  void addImage(qint64 semesterNumber, QString courseName, QString path, QString comments, QStringList tags = {});
Q_INVOKABLE  void addSemester(qint64 semesterNumber);
Q_INVOKABLE  void addCourse(qint64 semesterNumber, QString courseName);

Q_INVOKABLE  void deleteImage(qint64 semesterNumber, QString courseName, QString path);
Q_INVOKABLE  void deleteCourse(qint64 semesterNumber, QString courseName);
Q_INVOKABLE  void deleteSemester(qint64 semesterNumber);






        bool hasChildren(const QModelIndex &parent) const;

    signals:

    public slots:
};

#endif // IMAGEPROVIDER_H
