#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QObject>
#include <QAbstractItemModel>
#include <QSqlDatabase>
#include <QString>
#include <QList>
#include <QStringList>
#include <QModelIndex>
#include <QUrl>
#include <QVariant>


enum dataType {ROOT, SEMESTER, COURSE, THEME, IMAGE};

struct dbData{
    qint64 pid;
    QString path;
    QString comments;
    QStringList tags;
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
        QModelIndex currentIndex;
        QVariantList currentImages;
        bool containsSemesterAlready (QList<DataWrapper*> children, qint64 number);
        bool containsCourseAlready (QList<DataWrapper*> children, QString comments);
        bool containsPathAlready (QList<DataWrapper*> children, QString path);
        DataWrapper* findFromNumber (QList<DataWrapper*> children, qint64 number);
        DataWrapper* findFromName (QList<DataWrapper*> children, QString name);
        DataWrapper* findFromPath (QList<DataWrapper*> children, QString path);
        void recountRowNumbers(QList<DataWrapper*> children);
        bool include(const QStringList &big, const QStringList &small);

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
        void setDataTheme(const QModelIndex &parent, QString themeName);
        void setDataImage(const QModelIndex &parent, QString path, QString comments, QStringList tags={});
        void removeDataFromDb(qint64 id);
        bool hasChildren(const QModelIndex &parent) const;

Q_INVOKABLE  void setCurrentIndex (const QModelIndex &currentIndex);
Q_INVOKABLE  bool showMenuItem (const QModelIndex &index, qint64 type);
Q_INVOKABLE  void deleteItem();

Q_INVOKABLE  void addImage(QString path, QString comments = "", QString tags = "");
Q_INVOKABLE  void addSemester(qint64 semesterNumber);
Q_INVOKABLE  void addCourse(QString courseName);
Q_INVOKABLE  void addTheme(QString themeName);

Q_INVOKABLE  void addTags(QString tags);
Q_INVOKABLE  void setComment(QString comment);

Q_INVOKABLE  QString showTags();
Q_INVOKABLE  QString showComment();
Q_INVOKABLE void print(QUrl data);

Q_INVOKABLE QVariantList getChildrenIndexes();

Q_INVOKABLE qint64 findByTags (QString _tags);
Q_INVOKABLE QVariant fetchImage (qint64 number);

   signals:

    public slots:
};

#endif // IMAGEPROVIDER_H
