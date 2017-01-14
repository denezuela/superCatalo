#include "imageredactor.h"
#include <QMatrix>
#include <QImage>
#include <QVector>
#include <QColor>
#include <QString>
#include <QtMath>
ImageRedactor::ImageRedactor()
{
    qDebug()<<"it's O'k";
}

void ImageRedactor::loadImage(QString path)
{
    if(image.load(path.remove("file:"))) qDebug()<<"Image is loaded";
    else qDebug()<<"Image is no loaded";
}

void ImageRedactor::rotate(qreal angle)
{
    QTransform matrix;
    matrix.rotate(angle);
    image=image.transformed(matrix, Qt::SmoothTransformation);
}

void ImageRedactor::saveImage(QString path)
{
    image.save(path.remove("file:"));
    qDebug()<<"image saved"<<" "<<path;
}

void ImageRedactor::saveBlurImage(QString path)
{
    blurimage.save(path.remove("file:"));
    qDebug()<<"image saved";
}
void ImageRedactor::crop(int x, int y, int w, int h)
{
    if(((qMax(x,w)-qMin(x,w))!=image.width())||((qMax(y,h)-qMin(y,h))!=image.height())){
    QImage temp(qMax(x,w)-qMin(x,w),qMax(y,h)-qMin(y,h),image.format());
    for(int i=0; i<temp.width(); i++){
        for(int j=0; j<temp.height(); j++){
            temp.setPixel(i,j,image.pixel(i+qMin(x,w),j+qMin(y,h)));
        }
    }

image=temp;
    }
}

void ImageRedactor::blackAndWhite()
{
    if (!image.isNull())
    {
        int pixels = image.width() * image.height();
        if (pixels*(int)sizeof(QRgb) <= image.byteCount())
        {
            QRgb *data = (QRgb *)image.bits();
            for (int i = 0; i < pixels; i++)
            {
                int val = qGray(data[i]);
                data[i] = qRgba(val, val, val, qAlpha(data[i]));
            }
        }
    }

}

void ImageRedactor::blur(int radius)
{
    int divsum=0;
    divsum=(radius+1)*(radius+1);
    qDebug()<<divsum;
    blurimage=image;
    double r,g,b, sumRed, sumGreen, sumBlue, sumRed2, sumGreen2, sumBlue2,r1,g1,b1;
    int n=image.height();
    int m=image.width();
    QImage temp;
    QColor rgb;

    for(int i=0; i<n; i++)
    {
        r=0; g=0; b=0; sumRed=0;sumGreen=0; sumBlue=0; sumRed2=0; sumGreen2=0; sumBlue2=0; r1=g1=b1=0;
        for(int k=0;k<=radius;k++)
        {
            rgb=image.pixelColor(radius+k,i);
            if(k>0){
            r+=rgb.red()*(1+radius-k);
            g+=rgb.green()*(1+radius-k);
            b+=rgb.blue()*(1+radius-k);
                sumRed2+=rgb.red();
                sumBlue2+=rgb.blue();
                sumGreen2+=rgb.green();
            }
            rgb=image.pixelColor(radius-k,i);
            r+=rgb.red()*(1+radius-k);
            g+=rgb.green()*(1+radius-k);
            b+=rgb.blue()*(1+radius-k);
            sumRed+=rgb.red();
            sumBlue+=rgb.blue();
            sumGreen+=rgb.green();
        }
            r1=static_cast<double>(r)/(divsum);
            g1=static_cast<double>(g)/(divsum);
            b1=static_cast<double>(b)/(divsum);

            rgb.setRed(qMin(r1,255.0));
            rgb.setGreen(qMin(g1,255.0));
            rgb.setBlue(qMin(b1,255.0));
            blurimage.setPixelColor(radius,i,rgb);


        for(int j=radius+1; j<m-radius-1; j++)
        {
            r-=sumRed;
            g-=sumGreen;
            b-=sumBlue;
            rgb=image.pixelColor(j-radius-1,i);
            sumRed-=rgb.red();
            sumGreen-=rgb.green();
            sumBlue-=rgb.blue();
            rgb=image.pixelColor(j+radius,i);
            sumRed2+=rgb.red();
            sumGreen2+=rgb.green();
            sumBlue2+=rgb.blue();
            r+=sumRed2;
            g+=sumGreen2;
            b+=sumBlue2;
            rgb=image.pixelColor(j,i);
            sumRed2-=rgb.red();
            sumGreen2-=rgb.green();
            sumBlue2-=rgb.blue();
            sumRed+=rgb.red();
            sumGreen+=rgb.green();
            sumBlue+=rgb.blue();
            r1=static_cast<double>(r)/(divsum);
            g1=static_cast<double>(g)/(divsum);
            b1=static_cast<double>(b)/(divsum);

            rgb.setRed(qMin(r1,255.0));
            rgb.setGreen(qMin(g1,255.0));
            rgb.setBlue(qMin(b1,255.0));
            blurimage.setPixelColor(j,i,rgb);
        }
    }
    temp=blurimage;
    for(int j=0; j<m; j++)
    {
        r=0; g=0; b=0; sumRed=0;sumGreen=0; sumBlue=0; sumRed2=0; sumGreen2=0; sumBlue2=0; r1=g1=b1=0;
        for(int k=0;k<=radius;k++)
        {
            rgb=temp.pixelColor(j,radius+k);
            if(k>0){
            r+=rgb.red()*(1+radius-k);
            g+=rgb.green()*(1+radius-k);
            b+=rgb.blue()*(1+radius-k);
                sumRed2+=rgb.red();
                sumBlue2+=rgb.blue();
                sumGreen2+=rgb.green();
            }
            rgb=temp.pixelColor(j,radius-k);
            r+=rgb.red()*(1+radius-k);
            g+=rgb.green()*(1+radius-k);
            b+=rgb.blue()*(1+radius-k);
            sumRed+=rgb.red();
            sumBlue+=rgb.blue();
            sumGreen+=rgb.green();
        }
            r1=static_cast<double>(r)/(divsum);
            g1=static_cast<double>(g)/(divsum);
            b1=static_cast<double>(b)/(divsum);

            rgb.setRed(qMin(r1,255.0));
            rgb.setGreen(qMin(g1,255.0));
            rgb.setBlue(qMin(b1,255.0));
            blurimage.setPixelColor(j,radius,rgb);


        for(int i=radius+1; i<n-radius-1; i++)
        {
            r-=sumRed;
            g-=sumGreen;
            b-=sumBlue;
            rgb=temp.pixelColor(j,i-radius-1);
            sumRed-=rgb.red();
            sumGreen-=rgb.green();
            sumBlue-=rgb.blue();
            rgb=temp.pixelColor(j,i+radius);
            sumRed2+=rgb.red();
            sumGreen2+=rgb.green();
            sumBlue2+=rgb.blue();
            r+=sumRed2;
            g+=sumGreen2;
            b+=sumBlue2;
            rgb=temp.pixelColor(j,i);
            sumRed2-=rgb.red();
            sumGreen2-=rgb.green();
            sumBlue2-=rgb.blue();
            sumRed+=rgb.red();
            sumGreen+=rgb.green();
            sumBlue+=rgb.blue();
            r1=static_cast<double>(r)/(divsum);
            g1=static_cast<double>(g)/(divsum);
            b1=static_cast<double>(b)/(divsum);

            rgb.setRed(qMin(r1,255.0));
            rgb.setGreen(qMin(g1,255.0));
            rgb.setBlue(qMin(b1,255.0));
            blurimage.setPixelColor(j,i,rgb);
        }
    }
    blurimage=temp;

}

void ImageRedactor::division()
{
    int step=220;
    int n=image.height();
    int m=image.width();
    for(int i=0; i<n; i++)
    {
        for(int j=0; j<m; j++)
        {
            QColor rgb1, rgb2;
            rgb1=image.pixelColor(j,i);
            rgb2=blurimage.pixelColor(j,i);
            double r=static_cast<double>(rgb1.red())/(rgb2.red())*255;
            double g=static_cast<double>(rgb1.green())/(rgb2.green())*255;
            double b=static_cast<double>(rgb1.blue())/(rgb2.blue())*255;
            if(r<step) r=0;
  //          if(r>=step) r=245;
            if(g<step) g=0;
   //         if(g>=step) g=245;
            if(b<step) b=0;
   //         if(b>=step) b=245;
            rgb1.setRed(qMin(r,255.0));
            rgb1.setGreen(qMin(g,255.0));
            rgb1.setBlue(qMin(b,255.0));
            image.setPixelColor(j,i,rgb1);
        }
    }
}

