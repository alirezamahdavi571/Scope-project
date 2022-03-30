#ifndef BASESCOPE_H
#define BASESCOPE_H

#include <QWidget>
#include <QTransform>
#include <QMargins>
#include <QPainter>
#include <QMouseEvent>
#include <QTextItem>
#include <qmath.h>
#include <QGridLayout>
#include <QElapsedTimer>
#include <QDebug>
#include <QtWidgets/QGesture>
#include <QtCharts/QChart>
#include <QKeyEvent>
#include <QDesktopServices>
#include <QCoreApplication>

struct BasePlot
{
    BasePlot()
    {
        color           = Qt::yellow;
        pointNumber     = 0 ;
        points          = nullptr;
    }

    ~BasePlot()
    {
       resetPoint();
    }


public:
    const QColor &getColor() const;
    void setColor(const QColor &newColor);

    int getPointNumber() const;
    void setPointNumber(int newPointNumber);

    QPointF *getPoints() const;
    void setPoints(QPointF *newPoints);

    void resetPoint()
    {
        if( points )
        {
            delete[] points;
            points =  nullptr;
        }
    }

    QPointF* points = nullptr;  // friend
    int      getLastUpdate() const;
    void     setLastUpdate(int newLastUpdate);
    void     drawPoints(QTransform tansform);

    const QPixmap &getPix() const;
    void setPix(const QPixmap &newPix);

private:
    int      pointNumber;
    int      lastUpdate = 0;
    QColor   color ;
    QPixmap  pix;
};

class BaseScope : public QWidget , QChar
{
    Q_OBJECT
public:
    explicit BaseScope(QWidget *parent = nullptr);


    void updateTransform();
    void updateMousePos(QPointF pos);

    const QRectF &boundRect() const;
    void setBoundRect(const QRectF &newBoundRect);

    bool zoomActive() const;
    void setZoomActive(bool newZoomActive);

    void setPoints(QPointF *points , int channel , int size, QColor color=Qt::yellow);



    bool mousePosActive() const;
    void setMousePosActive(bool newMousePosActive);

    bool boundTextActive() const;
    void setBoundTextActive(bool newBoundTextActive);

    bool mouseDragActive() const;
    void setMouseDragActive(bool newMouseDragActive);

    const QTransform &transformToPixel() const;
    void setTransformToPixel(const QTransform &newTransformToPixel);

    const QTransform &transform() const;
    void setTransform(const QTransform &newTransform);

    const QRectF &pixRect() const;
    void setPixRect(const QRectF &newPixRect);

private:
    bool       m_zoomActive      = true;
    bool       m_mousePosActive  = true;
    bool       m_mouseDragActive = true;
    bool       m_boundTextActive = true;
    bool       m_wModifire       = false;
    bool       m_hModifire       = false;

    QTransform m_transform;
    QTransform m_transformToPixel;
    QMarginsF  m_margin;
    QRectF     m_boundRectMain;
    QRectF     m_boundRect;
    QRectF     m_pixRect;
    QPointF    m_mousePosPix;
    QPointF    m_mousePos;
    QPointF    m_mousePress;
    QHash< int ,  BasePlot* > plots;
    int        m_zoomType;

    void createMousePos(QPainter*painter);
    void createBoundaryText(QPainter*painter);
    void updatePointsPix();
    bool gestureEvent(QGestureEvent *event);
    void drawScopeLines(QPainter *painter)  ;



signals:

protected:
    void paintEvent(QPaintEvent*e)override;
    void resizeEvent(QResizeEvent*e)override;
    void mouseMoveEvent(QMouseEvent*e)override;
    void mouseDoubleClickEvent(QMouseEvent*e)override;
    void wheelEvent(QWheelEvent*e)override;
    void keyPressEvent(QKeyEvent *event)override;
    //bool nativeEvent(const QByteArray &eventType, void *message, long *result)override;


};

#endif // BASESCOPE_H
