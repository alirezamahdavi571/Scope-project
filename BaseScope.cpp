#include "BaseScope.h"

BaseScope::BaseScope(QWidget *parent) : QWidget(parent)
{
    m_boundRectMain = m_boundRect = QRectF(QPointF(0,80)  , QPointF(1000,0));

    m_margin.setBottom(0);
    m_margin.setTop(0);
    m_margin.setLeft(0);
    m_margin.setRight(0);

    setAttribute(Qt::WA_MouseTracking);

    grabGesture(Qt::PanGesture);
    grabGesture(Qt::PinchGesture);
}

void BaseScope::updateTransform()
{
    m_transform.reset();
    m_transform.scale(m_boundRect.width() / m_pixRect.width() , m_boundRect.height() / m_pixRect.height());
    QPointF offsetBound = m_transform.inverted().map(m_boundRect.topLeft());
    m_transform.translate(m_margin.left()+offsetBound.x(), m_margin.bottom()+offsetBound.y());

    m_transformToPixel.reset();
    m_transformToPixel = m_transform.inverted();
}

void BaseScope::updateMousePos(QPointF pos)
{
    m_mousePosPix = pos;
    m_mousePos    = m_transform.map(pos);
}

const QRectF &BaseScope::boundRect() const
{
    return m_boundRect;
}

void BaseScope::setBoundRect(const QRectF &newBoundRect)
{
    m_boundRectMain =  m_boundRect = newBoundRect;
    updateTransform();
    updatePointsPix();
}

bool BaseScope::zoomActive() const
{
    return m_zoomActive;
}

void BaseScope::setZoomActive(bool newZoomActive)
{
    m_zoomActive = newZoomActive;
}

void BaseScope::setPoints(QPointF *points, int channel, int size , QColor color)
{
    if( !points )
        return;

    static QElapsedTimer ellapss;

    QHash<int,BasePlot*>::iterator i = plots.find(channel);
    if( i == plots.end() ) // not exist
    {
        BasePlot *plot = new BasePlot;
        plot->setPix(QPixmap(m_pixRect.size().toSize()));
        plot->setPointNumber( size );
        plot->setColor(color);
        plot->points = new QPointF[size];
        memcpy( plot->points , points , sizeof(QPointF)*size );

        plots[channel] = plot;

        plot->drawPoints( m_transformToPixel );
        update();
        ellapss.restart();
    }
    else if( size != plots[channel]->getPointNumber() ) //Size Changed
    {
        BasePlot *plot = i.value();
        plot->resetPoint();
        plot->points = new QPointF[size];
        memcpy( plot->points , points , sizeof(QPointF)*size );

        plot->setPointNumber( size );
        plot->setColor(color);

        plots[channel] = plot;

        plot->drawPoints( m_transformToPixel );
        update();
        ellapss.restart();
    }
    else
    {
        BasePlot *plot = i.value();

        memcpy( plot->points , points , sizeof(QPointF)*size );

        if( ellapss.elapsed() - plot->getLastUpdate() >= 40 )
        {
            plot->drawPoints( m_transformToPixel );
            plot->setLastUpdate( ellapss.elapsed() );
            update();
        }
    }
}

bool BaseScope::mousePosActive() const
{
    return m_mousePosActive;
}

void BaseScope::setMousePosActive(bool newMousePosActive)
{
    m_mousePosActive = newMousePosActive;
}

bool BaseScope::boundTextActive() const
{
    return m_boundTextActive;
}

void BaseScope::setBoundTextActive(bool newBoundTextActive)
{
    m_boundTextActive = newBoundTextActive;
}

bool BaseScope::mouseDragActive() const
{
    return m_mouseDragActive;
}

void BaseScope::setMouseDragActive(bool newMouseDragActive)
{
    m_mouseDragActive = newMouseDragActive;
}

const QTransform &BaseScope::transformToPixel() const
{
    return m_transformToPixel;
}

void BaseScope::setTransformToPixel(const QTransform &newTransformToPixel)
{
    m_transformToPixel = newTransformToPixel;
}

const QTransform &BaseScope::transform() const
{
    return m_transform;
}

void BaseScope::setTransform(const QTransform &newTransform)
{
    m_transform = newTransform;
}

const QRectF &BaseScope::pixRect() const
{
    return m_pixRect;
}

void BaseScope::setPixRect(const QRectF &newPixRect)
{
    m_pixRect = newPixRect.marginsAdded(m_margin);
}


void BaseScope::paintEvent(QPaintEvent *)
{
    QPainter painter(this)    ;

    painter.setBrush( Qt::black );
    painter.drawRect( m_pixRect );

    if(mousePosActive())
        createMousePos(&painter);
    if(boundTextActive())
        createBoundaryText(&painter);

    drawScopeLines(&painter);

    QHash<int,BasePlot*>::const_iterator it = plots.constBegin();
    while( it != plots.constEnd())
    {
        BasePlot *plot = it.value();
        painter.drawPixmap(plot->getPix().rect() , plot->getPix() , m_pixRect);
        ++it;
    }
    painter.end();
}

void BaseScope::createMousePos(QPainter *painter)
{
    painter->save();
    QFont font;
    font.setPixelSize(12);
    font.setBold(true);
    painter->setFont( font );
    painter->setPen( Qt::green );
    painter->drawText( m_mousePosPix + QPoint(0,-15), "x:" + QString::number(m_mousePos.x()) );
    painter->drawText( m_mousePosPix  , "y:" + QString::number(m_mousePos.y()) );
    painter->restore();
}

void BaseScope::createBoundaryText(QPainter *painter)
{
    painter->save();
    QFont font;
    font.setPixelSize(14);
    font.setBold(true);
    painter->setFont( font );
    painter->setPen( QColor(90 , 150 ,160 , 200) );
    painter->drawText(m_pixRect.topLeft() + QPoint(15,15) , QString::number(m_boundRect.left()));
    painter->drawText(m_pixRect.topRight() + QPoint(-50,15) , QString::number(m_boundRect.right()));
    painter->drawText(m_pixRect.topLeft()+QPoint(m_pixRect.center().x() - 10 , 0) + QPoint(-3,15) , QString::number(m_boundRect.top()));
    painter->drawText(m_pixRect.bottomLeft()+QPoint(m_pixRect.center().x() , 0) + QPoint(-40,15) , QString::number(m_boundRect.bottom()));
    painter->restore();

}

void BaseScope::updatePointsPix()
{
    QHash<int,BasePlot*>::const_iterator it = plots.constBegin();
    while( it != plots.constEnd())
    {
        BasePlot *plot = it.value();
        plot->setPix(QPixmap(m_pixRect.size().toSize()));
        plot->drawPoints( m_transformToPixel );
        ++it;
    }
    update();
}

void BaseScope::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    setPixRect(rect());
    updateTransform();
    updatePointsPix();

}

void BaseScope::mouseMoveEvent(QMouseEvent *e)
{
    if(mouseDragActive())
    {
        if( e->buttons() == Qt::RightButton )
        {
            QPointF p = m_transform.map(e->pos() - m_mousePosPix) - m_transform.map(QPointF(0,0));
            m_boundRect.moveLeft(m_boundRect.left() - p.x());
            m_boundRect.moveTop(m_boundRect.top()   - p.y());

            updateTransform();
            updatePointsPix();
        }
    }
    updateMousePos(e->pos());
    if(mouseDragActive() || mousePosActive())
        update();
}

void BaseScope::mouseDoubleClickEvent(QMouseEvent *e)
{
    if( !m_zoomActive )
        return;

    m_boundRect = m_boundRectMain ;
    updateTransform();
    updateMousePos(e->pos());
    updatePointsPix();
}

void BaseScope::wheelEvent(QWheelEvent *e)
{
    if( !m_zoomActive )
        return;


    QPointF p1 = m_mousePos;


    double coef  = 0.9;
    if( e->angleDelta().y() > 0 )
    {
        if( m_boundRect.width()*coef  < 1  || -1*m_boundRect.height()*coef  < 1)
        {
            m_boundRect.setHeight(-1);
            m_boundRect.setWidth(1);
        }
        else
        {

            if(m_zoomType == Qt::Key_W)
            {
                m_boundRect.setWidth(m_boundRect.width()*coef);

                QPointF p2 = m_transformToPixel.map(p1);
                m_boundRect.moveLeft( m_boundRect.left() + m_transform.map(p2- e->position()).x() -  m_transform.map(QPointF(0,0)).x()   );
                m_boundRect.moveTop(m_boundRect.top() - m_transform.map(e->position() - p2 ).y() +  m_transform.map(QPointF(0,0)).y()   );
                updateTransform();
                updateMousePos( e->position().toPoint() );
                updatePointsPix();
                m_zoomType = 0;
            }
            else if(m_zoomType == Qt::Key_H)
            {
                m_boundRect.setHeight(m_boundRect.height()*coef);

                QPointF p2 = m_transformToPixel.map(p1);
                m_boundRect.moveLeft( m_boundRect.left() + m_transform.map(p2- e->position()).x() -  m_transform.map(QPointF(0,0)).x()   );
                m_boundRect.moveTop(m_boundRect.top() - m_transform.map(e->position() - p2 ).y() +  m_transform.map(QPointF(0,0)).y()   );
                updateTransform();
                updateMousePos( e->position().toPoint() );
                updatePointsPix();
                m_zoomType = 0;
            }
            else
            {
                m_boundRect.setHeight(m_boundRect.height()*coef);
                m_boundRect.setWidth(m_boundRect.width()*coef);

                QPointF p2 = m_transformToPixel.map(p1);
                m_boundRect.moveLeft( m_boundRect.left() + m_transform.map(p2- e->position()).x() -  m_transform.map(QPointF(0,0)).x()   );
                m_boundRect.moveTop(m_boundRect.top() - m_transform.map(e->position() - p2 ).y() +  m_transform.map(QPointF(0,0)).y()   );
                updateTransform();
                updateMousePos( e->position().toPoint() );
                updatePointsPix();

            }


        }

    }
    else
    {

        if(m_zoomType == Qt::Key_W)
        {
            m_boundRect.setWidth(m_boundRect.width()/coef);
            QPointF p2 = m_transformToPixel.map(p1);
            m_boundRect.moveLeft( m_boundRect.left() + m_transform.map(p2- e->position()).x() -  m_transform.map(QPointF(0,0)).x()   );
            m_boundRect.moveTop(m_boundRect.top() - m_transform.map(e->position() - p2 ).y() +  m_transform.map(QPointF(0,0)).y()   );
            updateTransform();
            updateMousePos( e->position().toPoint() );
            updatePointsPix();
            m_zoomType = 0;
        }

        else if(m_zoomType == Qt::Key_H)
        {
            m_boundRect.setHeight(m_boundRect.height()/coef);
            QPointF p2 = m_transformToPixel.map(p1);
            m_boundRect.moveLeft( m_boundRect.left() + m_transform.map(p2- e->position()).x() -  m_transform.map(QPointF(0,0)).x()   );
            m_boundRect.moveTop(m_boundRect.top() - m_transform.map(e->position() - p2 ).y() +  m_transform.map(QPointF(0,0)).y()   );
            updateTransform();
            updateMousePos( e->position().toPoint() );
            updatePointsPix();
            m_zoomType = 0;
        }

        else
        {
            m_boundRect.setHeight(m_boundRect.height()/coef);
            m_boundRect.setWidth(m_boundRect.width()/coef);
            QPointF p2 = m_transformToPixel.map(p1);
            m_boundRect.moveLeft( m_boundRect.left() + m_transform.map(p2- e->position()).x() -  m_transform.map(QPointF(0,0)).x()   );
            m_boundRect.moveTop(m_boundRect.top() - m_transform.map(e->position() - p2 ).y() +  m_transform.map(QPointF(0,0)).y()   );
            updateTransform();
            updateMousePos( e->position().toPoint() );
            updatePointsPix();
        }



    }

    QPointF p2 = m_transformToPixel.map(p1);
    m_boundRect.moveLeft( m_boundRect.left() + m_transform.map(p2- e->position()).x() -  m_transform.map(QPointF(0,0)).x()   );
    m_boundRect.moveTop(m_boundRect.top() - m_transform.map(e->position() - p2 ).y() +  m_transform.map(QPointF(0,0)).y()   );
    updateTransform();
    updateMousePos( e->position().toPoint() );
    updatePointsPix();

}


bool BaseScope::gestureEvent(QGestureEvent *event)
{
    if (QGesture *gesture = event->gesture(Qt::PanGesture)) {
        QPanGesture *pan = static_cast<QPanGesture *>(gesture);
        // QChart::scroll(-(pan->delta().x()), pan->delta().y());
    }

    if (QGesture *gesture = event->gesture(Qt::PinchGesture)) {
        QPinchGesture *pinch = static_cast<QPinchGesture *>(gesture);
        // if (pinch->changeFlags() & QPinchGesture::ScaleFactorChanged)
        // QChart::zoom(pinch->scaleFactor());
    }

    return true;
}


void BaseScope::drawScopeLines(QPainter *painter)
{
    double stepHorizental = this->width()/5 ;

    double stepVertical   = this->height()/5 ;

    QPen penScopeLines;

    penScopeLines.setColor(QColor(90, 90, 90, 180));
    penScopeLines.setStyle(Qt::DashDotLine);

    painter->save();


    for(int i = 1 ; i < 5 ; i++)
    {
        painter->setPen(penScopeLines);

        painter->drawLine(QPointF(i*stepHorizental , 0) , QPointF(i*stepHorizental,this->height()));

        painter->drawLine(QPointF(0 , i*stepVertical) , QPointF(this->width(),i*stepVertical));
    }

    painter->restore();
}

void BaseScope::keyPressEvent(QKeyEvent *event)
{
   // if(event->key() == Qt::Key_W)
   // {
   //
   // }
   // else if(event->key() == Qt::Key_H)
   // {
   //     m_wModifire = false;
   //     m_hModifire = true;
   // }

    m_zoomType = event->key();
}

//-----------------------------------------------------------------------------------


const QColor &BasePlot::getColor() const
{
    return color;
}

void BasePlot::setColor(const QColor &newColor)
{
    color = newColor;
}

int BasePlot::getPointNumber() const
{
    return pointNumber;
}

void BasePlot::setPointNumber(int newPointNumber)
{
    pointNumber = newPointNumber;
}

QPointF *BasePlot::getPoints() const
{
    return points;
}

void BasePlot::setPoints(QPointF *newPoints)
{
    points = newPoints;
}

int BasePlot::getLastUpdate() const
{
    return lastUpdate;
}

void BasePlot::setLastUpdate(int newLastUpdate)
{
    lastUpdate = newLastUpdate;
}

void BasePlot::drawPoints(QTransform tansform)
{
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setPen(QPen(getColor(), 1));
    for (int i = 0; i < getPointNumber(); i++)
       // points[i] = tansform.map(points[i]) - QPointF(1,-1);
        painter.drawPoint(tansform.map(points[i]) );


    //painter.drawPoints(points, getPointNumber());
}

const QPixmap &BasePlot::getPix() const
{
    return pix;
}

void BasePlot::setPix(const QPixmap &newPix)
{
    pix = newPix;
}




