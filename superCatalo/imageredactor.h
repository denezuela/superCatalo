#ifndef IMAGEREDACTOR_H
#define IMAGEREDACTOR_H

#include <QObject>
#include <QImage>
#include <QString>
#include <QDebug>
class ImageRedactor: public QObject
{
    Q_OBJECT

public:

    QImage image;
    QImage blurimage;

public:
    ImageRedactor();
public slots:
Q_INVOKABLE    void loadImage(QString path);
Q_INVOKABLE    void rotate(qreal angle);
Q_INVOKABLE    void saveImage(QString path);
Q_INVOKABLE    void crop(int x, int y, int w, int h);
Q_INVOKABLE    void blackAndWhite();
Q_INVOKABLE    void blur(int radius);
Q_INVOKABLE    void division();
Q_INVOKABLE    void saveBlurImage(QString path);
signals:
    void imageIsChanged();
};

#endif // IMAGEREDACTOR_H
