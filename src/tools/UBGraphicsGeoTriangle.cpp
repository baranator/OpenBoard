/*
 * Copyright (C) 2015-2018 Département de l'Instruction Publique (DIP-SEM)
 *
 * Copyright (C) 2013 Open Education Foundation
 *
 * Copyright (C) 2010-2013 Groupement d'Intérêt Public pour
 * l'Education Numérique en Afrique (GIP ENA)
 *
 * This file is part of OpenBoard.
 *
 * OpenBoard is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License,
 * with a specific linking exception for the OpenSSL project's
 * "OpenSSL" library (or with modified versions of it that use the
 * same license as the "OpenSSL" library).
 *
 * OpenBoard is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenBoard. If not, see <http://www.gnu.org/licenses/>.
 */
#include <QtDebug>



#include <QPixmap>

#include <math.h>

#include "tools/UBGraphicsGeoTriangle.h"
#include "domain/UBGraphicsScene.h"
#include "core/UBApplication.h"
#include "gui/UBResources.h"
#include "board/UBBoardController.h" // TODO UB 4.x clean that dependency
#include "board/UBDrawingController.h"

#include "core/memcheck.h"

const QRect UBGraphicsGeoTriangle::sDefaultRect = QRect(0, 0, 800, 400);


UBGraphicsGeoTriangle::UBGraphicsGeoTriangle()
    : QGraphicsRectItem()
    , mResizing(false)
    , mRotating(false)
{
    setRect(sDefaultRect);

    mResizeSvgItem = new QGraphicsSvgItem(":/images/resizeRuler.svg", this);
    mResizeSvgItem->setVisible(false);
    mResizeSvgItem->setData(UBGraphicsItemData::ItemLayerType, QVariant(UBItemLayerType::Control));

    mRotateSvgItem = new QGraphicsSvgItem(":/images/rotateTool.svg", this);
    mRotateSvgItem->setVisible(false);
    mRotateSvgItem->setData(UBGraphicsItemData::ItemLayerType, QVariant(UBItemLayerType::Control));

    create(*this);

    setData(UBGraphicsItemData::itemLayerType, QVariant(itemLayerType::CppTool)); //Necessary to set if we want z value to be assigned correctly

    setFlag(QGraphicsItem::ItemIsSelectable, false);
    updateResizeCursor();
}

void UBGraphicsGeoTriangle::updateResizeCursor()
{
    QPixmap pix(":/images/cursors/resize.png");
    QTransform itemTransform = sceneTransform();
    QRectF itemRect = boundingRect();
    QPointF topLeft = itemTransform.map(itemRect.topLeft());
    QPointF topRight = itemTransform.map(itemRect.topRight());
    QLineF topLine(topLeft, topRight);
    qreal angle = topLine.angle();
    QTransform tr;
    tr.rotate(- angle);
    QCursor resizeCursor  = QCursor(pix.transformed(tr, Qt::SmoothTransformation), pix.width() / 2,  pix.height() / 2);
    mResizeCursor = resizeCursor;
}


UBGraphicsGeoTriangle::~UBGraphicsGeoTriangle()
{
    // NOOP
}

UBItem* UBGraphicsGeoTriangle::deepCopy() const
{
    UBGraphicsGeoTriangle* copy = new UBGraphicsGeoTriangle();

    copyItemParameters(copy);

    // TODO UB 4.7 ... complete all members ?

    return copy;
}

void UBGraphicsGeoTriangle::copyItemParameters(UBItem *copy) const
{
    UBGraphicsGeoTriangle *cp = dynamic_cast<UBGraphicsGeoTriangle*>(copy);
    if (cp)
    {
        cp->setPos(this->pos());
        cp->setRect(this->rect());
        cp->setTransform(this->transform());
    }
}

void UBGraphicsGeoTriangle::paint(QPainter *painter, const QStyleOptionGraphicsItem *styleOption, QWidget *widget)
{
    Q_UNUSED(styleOption);
    Q_UNUSED(widget);

    UBAbstractDrawRuler::paint();

    QTransform antiScaleTransform2;
    qreal ratio = mAntiScaleRatio > 1.0 ? mAntiScaleRatio : 1.0;
    antiScaleTransform2.scale(ratio, 1.0);

    mResizeSvgItem->setTransform(antiScaleTransform2);
    mResizeSvgItem->setPos(resizeButtonRect().topLeft());

    mRotateSvgItem->setTransform(antiScaleTransform2);
    mRotateSvgItem->setPos(rotateButtonRect().topLeft());



    painter->setPen(drawColor());
    painter->setBrush(edgeFillColor());
    painter->setRenderHint(QPainter::Antialiasing, true);
    
    //draw triangle outline
    
    int width=rect().right()-rect().left();
    QPolygonF polygon;
    polygon 
        << QPointF(rect().bottomLeft()) 
        << QPointF(rect().bottomRight()) 
        << QPointF(rect().left()+width/2, rect().bottom()-width/2)
        << QPointF(rect().bottomLeft()) ;
    painter->drawPolyline(polygon);
    //painter->drawRoundedRect(rect(), sRoundingRadius, sRoundingRadius);
    //fillBackground(painter);
    paintGraduations(painter);
    paintAngleGraduations(painter);
    //if (mRotating)
    //    paintRotationCenter(painter);
    
    
    
    
}


QVariant UBGraphicsGeoTriangle::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemVisibleHasChanged)
    {
        mCloseSvgItem->setParentItem(this);
        mResizeSvgItem->setParentItem(this);
        mRotateSvgItem->setParentItem(this);
    }

    return QGraphicsRectItem::itemChange(change, value);
}

void UBGraphicsGeoTriangle::fillBackground(QPainter *painter)
{
    
    //qDebug() << "Widget" << widget << "at position" << widget->pos();
    

    
    //QRectF rect1(rect().topLeft(), QSizeF(rect().width(), rect().height() / 4));
    //QLinearGradient linearGradient1(rect1.topLeft(), rect1.bottomLeft());
    //linearGradient1.setColorAt(0, edgeFillColor());
    //linearGradient1.setColorAt(1, middleFillColor());
    //painter->fillRect(rect1, linearGradient1);
    
    
    
/*
    QRectF rect2(rect1.bottomLeft(), QSizeF(rect().width(), rect().height() / 2));
    painter->fillRect(rect2, middleFillColor());

    QRectF rect3(rect2.bottomLeft(), rect1.size());
    QLinearGradient linearGradient3(
        rect3.topLeft(),
        rect3.bottomLeft());
    linearGradient3.setColorAt(0, middleFillColor());
    linearGradient3.setColorAt(1, edgeFillColor());
    painter->fillRect(rect3, linearGradient3);*/
}

void UBGraphicsGeoTriangle::paintGraduations(QPainter *painter)
{
    painter->save();
    painter->setFont(font());
    QFontMetricsF fontMetrics(painter->font());

    // Update the width of one "centimeter" to correspond to the width of the background grid (whether it is displayed or not)
    sPixelsPerCentimeter = UBApplication::boardController->activeScene()->backgroundGridSize();

    qreal pixelsPerMillimeter = sPixelsPerCentimeter/10.0;
    
    //geo triangles graduation starts from center to both sides half of overall length
    int rulerLengthInMillimeters = (rect().width() - 3*sLeftEdgeMargin)/(pixelsPerMillimeter*2);

    // When a "centimeter" is too narrow, we only display every 5th number, and every 5th millimeter mark
    double numbersWidth = fontMetrics.width("00");
    bool shouldDisplayAllNumbers = (numbersWidth <= (sPixelsPerCentimeter - 5));

    for (int millimeters(0); millimeters < rulerLengthInMillimeters; millimeters++) {

        double graduationX = rotationCenter().x() + pixelsPerMillimeter * millimeters;
        double ngraduationX = rotationCenter().x() - pixelsPerMillimeter * millimeters;
        double graduationHeight = 0;

        if (millimeters % UBGeometryUtils::millimetersPerCentimeter == 0)
            graduationHeight = UBGeometryUtils::centimeterGraduationHeight*0.8;

        else if (millimeters % UBGeometryUtils::millimetersPerHalfCentimeter == 0)
            graduationHeight = UBGeometryUtils::halfCentimeterGraduationHeight*0.8;

        else
            graduationHeight = UBGeometryUtils::millimeterGraduationHeight;


        if (shouldDisplayAllNumbers || millimeters % UBGeometryUtils::millimetersPerHalfCentimeter == 0) {
            painter->drawLine(QLineF(graduationX, rotationCenter().y(), graduationX, rotationCenter().y() - graduationHeight));
            painter->drawLine(QLineF(ngraduationX, rotationCenter().y(), ngraduationX, rotationCenter().y() - graduationHeight));
        }


        if ((shouldDisplayAllNumbers && millimeters % UBGeometryUtils::millimetersPerCentimeter == 0)
            || millimeters % (UBGeometryUtils::millimetersPerCentimeter*5) == 0)
        {
            QString text = QString("%1").arg((int)(millimeters / UBGeometryUtils::millimetersPerCentimeter));

            if (graduationX + fontMetrics.width(text) / 2 < rect().right()) {
                qreal textWidth = fontMetrics.width(text);
                qreal textHeight = fontMetrics.tightBoundingRect(text).height() + 5;

                painter->drawText(
                    QRectF(graduationX - textWidth / 2, rect().bottom()  +1 - UBGeometryUtils::centimeterGraduationHeight - textHeight, textWidth, textHeight),
                    Qt::AlignVCenter, text);
                //dont paint zero-label twice
                if(millimeters!=0)
                    painter->drawText( QRectF(ngraduationX - textWidth / 2, rect().bottom()+1 - UBGeometryUtils::centimeterGraduationHeight - textHeight, textWidth, textHeight),
                    Qt::AlignVCenter, text);
            }
        }
    }


    painter->restore();

}

void UBGraphicsGeoTriangle::paintAngleGraduations(QPainter *painter)
{
    painter->save();
    painter->setFont(font());
    QFontMetricsF fontMetrics(painter->font());

    // Update the width of one "centimeter" to correspond to the width of the background grid (whether it is displayed or not)
    sPixelsPerCentimeter = UBApplication::boardController->activeScene()->backgroundGridSize();

    qreal pixelsPerMillimeter = sPixelsPerCentimeter/10.0;
    

    int rulerLengthInMillimeters = rect().width()/pixelsPerMillimeter;
    
    // When a "centimeter" is too narrow, we only display every 5th number, and every 5th millimeter mark
    double numbersWidth = fontMetrics.width("00");
    bool shouldDisplayAllNumbers = (numbersWidth <= (sPixelsPerCentimeter - 5));

    for (int angle(0); angle < 180; angle+=1) {

        //double graduationX = rotationCenter().x() + pixelsPerMillimeter * millimeters;
        //double ngraduationX = rotationCenter().x() - pixelsPerMillimeter * millimeters;
        double graduationHeight = 0;

        if (angle % 10 == 0)
            graduationHeight = UBGeometryUtils::centimeterGraduationHeight*0.8;

        else if (angle % 5 == 0)
            graduationHeight = UBGeometryUtils::halfCentimeterGraduationHeight*0.8;

        else
            graduationHeight = UBGeometryUtils::millimeterGraduationHeight;
        
        int rad=rulerLengthInMillimeters*0.5*0.6*pixelsPerMillimeter;
        double cosa=cos(angle*PI/180);
        double sina=sin(angle*PI/180);
        double tana=tan(angle*PI/180);
        //int markY = *tan(angle*PI/180);
        
       // if (shouldDisplayAllNumbers || millimeters % UBGeometryUtils::millimetersPerHalfCentimeter == 0) {
            painter->drawLine(
                QLineF(rotationCenter().x()-cosa*rad, rotationCenter().y()-rad*sina, 
                       rotationCenter().x()-cosa*(rad-graduationHeight), rotationCenter().y()-(rad-graduationHeight)*sina));
                       //rotationCenter().x()-cos(angle*PI/180)*(rad-2), rotationCenter().y()-(rad-2)*tan(angle*PI/180)));
            //painter->drawLine(QLineF(ngraduationX, rotationCenter().y(), ngraduationX, rotationCenter().y() - graduationHeight));
       // }
            
        //draw bordermarks
        
        double bx=(angle>90?-1:1)*rect().width()/(2*(1+abs(tana)));
        double by=rect().width()/2-abs(bx);
        double len=0;
        if(angle%10==0)
            len=0.075;
        else if(angle%5==0)
            len=0.050;
        else
            len=0.025;
        painter->drawLine(
                QLineF(rotationCenter().x()-bx, rotationCenter().y()-by, 
                       rotationCenter().x()-bx*0.95*(1.0-len), rotationCenter().y()-by*0.95*(1.0-len)));
                      // rotationCenter().x()-bx*0.9, rotationCenter().y()-by*0.9));    

        /*if ((shouldDisplayAllNumbers && millimeters % UBGeometryUtils::millimetersPerCentimeter == 0)
            || millimeters % (UBGeometryUtils::millimetersPerCentimeter*5) == 0)
        {
            QString text = QString("%1").arg((int)(millimeters / UBGeometryUtils::millimetersPerCentimeter));

            if (graduationX + fontMetrics.width(text) / 2 < rect().right()) {
                qreal textWidth = fontMetrics.width(text);
                qreal textHeight = fontMetrics.tightBoundingRect(text).height() + 5;

                painter->drawText(
                    QRectF(graduationX - textWidth / 2, rect().bottom()  +1 - UBGeometryUtils::centimeterGraduationHeight - textHeight, textWidth, textHeight),
                    Qt::AlignVCenter, text);
                //dont paint zero-label twice
                if(millimeters!=0)
                    painter->drawText( QRectF(ngraduationX - textWidth / 2, rect().bottom()+1 - UBGeometryUtils::centimeterGraduationHeight - textHeight, textWidth, textHeight),
                    Qt::AlignVCenter, text);
            }
        }*/
    }


    painter->restore();

}


// void UBGraphicsGeoTriangle::paintRotationCenter(QPainter *painter)
// {
//     painter->drawArc(
//         rotationCenter().x() - sRotationRadius, rotationCenter().y() - sRotationRadius,
//         2 * sRotationRadius, 2 * sRotationRadius,
//         270 * sDegreeToQtAngleUnit, 90 * sDegreeToQtAngleUnit);
// }

void UBGraphicsGeoTriangle::rotateAroundCenter(qreal angle)
{
    QTransform transform;
    transform.translate(rotationCenter().x(), rotationCenter().y());
    transform.rotate(angle);
    transform.translate(- rotationCenter().x(), - rotationCenter().y());
    setTransform(transform, true);
}

QPointF UBGraphicsGeoTriangle::rotationCenter() const
{
    return QPointF((rect().left() + rect().right())/2, rect().bottom());
}


QRectF UBGraphicsGeoTriangle::resizeButtonRect() const
{
    QPixmap resizePixmap(":/images/resizeRuler.svg");
    QSizeF resizeRectSize(
        resizePixmap.rect().width(),
        rect().height());

    qreal ratio = mAntiScaleRatio > 1.0 ? mAntiScaleRatio : 1.0;
    QPointF resizeRectTopLeft(rect().width() - resizeRectSize.width() * ratio, 0);

    QRectF resizeRect(resizeRectTopLeft, resizeRectSize);
    resizeRect.translate(rect().topLeft());

    return resizeRect;
}

QRectF UBGraphicsGeoTriangle::closeButtonRect() const
{
    QPixmap closePixmap(":/images/closeTool.svg");

    QSizeF closeRectSize(
        closePixmap.width() * mAntiScaleRatio,
        closePixmap.height() * mAntiScaleRatio);

    QPointF closeRectCenter(
        rect().left() + sLeftEdgeMargin + sPixelsPerCentimeter/2,
        rect().center().y());

    QPointF closeRectTopLeft(
        closeRectCenter.x() - closeRectSize.width() / 2,
        closeRectCenter.y() - closeRectSize.height() / 2);

    return QRectF(closeRectTopLeft, closeRectSize);
}

QRectF UBGraphicsGeoTriangle::rotateButtonRect() const
{
    QPixmap rotatePixmap(":/images/closeTool.svg");

    QSizeF rotateRectSize(
        rotatePixmap.width() * mAntiScaleRatio,
        rotatePixmap.height() * mAntiScaleRatio);

    int centimeters = (int)(rect().width() - sLeftEdgeMargin - resizeButtonRect().width()) / (sPixelsPerCentimeter);
    QPointF rotateRectCenter(
        rect().left() + sLeftEdgeMargin + (centimeters - 0.5) * sPixelsPerCentimeter,
        rect().center().y());

    QPointF rotateRectTopLeft(
        rotateRectCenter.x() - rotateRectSize.width() / 2,
        rotateRectCenter.y() - rotateRectSize.height() / 2);

    return QRectF(rotateRectTopLeft, rotateRectSize);
}

void UBGraphicsGeoTriangle::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    UBStylusTool::Enum currentTool = (UBStylusTool::Enum)UBDrawingController::drawingController ()->stylusTool ();

    if (currentTool == UBStylusTool::Selector || currentTool == UBStylusTool::Play)
    {
        mCloseSvgItem->setVisible(mShowButtons);
        mResizeSvgItem->setVisible(mShowButtons);
        mRotateSvgItem->setVisible(mShowButtons);
        if (resizeButtonRect().contains(event->pos()))
            setCursor(resizeCursor());
        else if (closeButtonRect().contains(event->pos()))
            setCursor(closeCursor());
        else if (rotateButtonRect().contains(event->pos()))
            setCursor(rotateCursor());
        else
            setCursor(moveCursor());

        event->accept();
    }
    else if (UBDrawingController::drawingController()->isDrawingTool())
    {
        event->accept();
    }
}

void UBGraphicsGeoTriangle::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->pos().x() > resizeButtonRect().left())
    {
        mResizing = true;
        event->accept();
    }
    else if (rotateButtonRect().contains(event->pos()))
    {
        mRotating = true;
        event->accept();
    }
    else
    {
        mResizeSvgItem->setVisible(false);
        mRotateSvgItem->setVisible(false);
        QGraphicsItem::mousePressEvent(event);
    }
    mResizeSvgItem->setVisible(mShowButtons && mResizing);
    mRotateSvgItem->setVisible(mShowButtons && mRotating);
    mCloseSvgItem->setVisible(false);
}

void UBGraphicsGeoTriangle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!mResizing && !mRotating)
    {
        QGraphicsItem::mouseMoveEvent(event);
    }
    else
    {
        if (mResizing)
        {
            QPointF delta = event->pos() - event->lastPos();
            if (rect().width() + delta.x() < sMinLength)
                delta.setX(sMinLength - rect().width());

            if (rect().width() + delta.x() > sMaxLength)
                delta.setX(sMaxLength - rect().width());

            setRect(QRectF(rect().topLeft(), QSizeF(rect().width() + delta.x(), rect().height())));
        }
        else
        {
            QLineF currentLine(rotationCenter(), event->pos());
            QLineF lastLine(rotationCenter(), event->lastPos());
            rotateAroundCenter(currentLine.angleTo(lastLine));
        }

        event->accept();
    }
}

void UBGraphicsGeoTriangle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (mResizing)
    {
        mResizing = false;
        event->accept();
    }
    else if (mRotating)
    {
        mRotating = false;
        updateResizeCursor();
        update(QRectF(rotationCenter(), QSizeF(sRotationRadius, sRotationRadius)));
        event->accept();
    }
    else if (closeButtonRect().contains(event->pos()))
    {
        hide();
        event->accept();
    }
    else
    {
        QGraphicsItem::mouseReleaseEvent(event);
    }

    if (scene())
        scene()->setModified(true);
}

void UBGraphicsGeoTriangle::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    UBStylusTool::Enum currentTool = (UBStylusTool::Enum)UBDrawingController::drawingController ()->stylusTool ();

    if (currentTool == UBStylusTool::Selector ||
        currentTool == UBStylusTool::Play)
    {
        mCloseSvgItem->setParentItem(this);
        mResizeSvgItem->setParentItem(this);
        mRotateSvgItem->setParentItem(this);

        mShowButtons = true;
        mCloseSvgItem->setVisible(mShowButtons);
        mResizeSvgItem->setVisible(mShowButtons);
        mRotateSvgItem->setVisible(mShowButtons);
        if (event->pos().x() >= resizeButtonRect().left())
        {
            setCursor(resizeCursor());
        }
        else
        {
            if (closeButtonRect().contains(event->pos()))
                setCursor(closeCursor());
            else if (rotateButtonRect().contains(event->pos()))
                setCursor(rotateCursor());
            else
                setCursor(moveCursor());
        }
        event->accept();
        update();
    }
    else if (UBDrawingController::drawingController()->isDrawingTool())
    {
        setCursor(drawRulerLineCursor());
        UBDrawingController::drawingController()->mActiveRuler = this;
        event->accept();
    }
}

void UBGraphicsGeoTriangle::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    mShowButtons = false;
    setCursor(Qt::ArrowCursor);
    mCloseSvgItem->setVisible(mShowButtons);
    mResizeSvgItem->setVisible(mShowButtons);
    mRotateSvgItem->setVisible(mShowButtons);
    UBDrawingController::drawingController()->mActiveRuler = NULL;
    event->accept();
    update();
}



UBGraphicsScene* UBGraphicsGeoTriangle::scene() const
{
    return static_cast<UBGraphicsScene*>(QGraphicsRectItem::scene());
}

void UBGraphicsGeoTriangle::StartLine(const QPointF& scenePos, qreal width)
{
    Q_UNUSED(width);

    QPointF itemPos = mapFromScene(scenePos);

    mStrokeWidth = UBDrawingController::drawingController()->currentToolWidth();

    qreal y;

    if (itemPos.y() > rect().y() + rect().height() / 2)
    {
        drawLineDirection = 0;
        y = rect().y() + rect().height() + mStrokeWidth / 2;
    }
    else
    {
        drawLineDirection = 1;
        y = rect().y() - mStrokeWidth /2;
    }

    if (itemPos.x() < rect().x() + sLeftEdgeMargin)
        itemPos.setX(rect().x() + sLeftEdgeMargin);
    if (itemPos.x() > rect().x() + rect().width() - sLeftEdgeMargin)
        itemPos.setX(rect().x() + rect().width() - sLeftEdgeMargin);

    itemPos.setY(y);
    itemPos = mapToScene(itemPos);

    scene()->moveTo(itemPos);
    scene()->drawLineTo(itemPos, mStrokeWidth, true);
}

void UBGraphicsGeoTriangle::DrawLine(const QPointF& scenePos, qreal width)
{
    Q_UNUSED(width);
    QPointF itemPos = mapFromScene(scenePos);

    qreal y;
    if (drawLineDirection == 0)
    {
        y = rect().y() + rect().height() + mStrokeWidth / 2;
    }
    else
    {
        y = rect().y() - mStrokeWidth /2;
    }
    if (itemPos.x() < rect().x() + sLeftEdgeMargin)
        itemPos.setX(rect().x() + sLeftEdgeMargin);
    if (itemPos.x() > rect().x() + rect().width() - sLeftEdgeMargin)
        itemPos.setX(rect().x() + rect().width() - sLeftEdgeMargin);

    itemPos.setY(y);
    itemPos = mapToScene(itemPos);

    // We have to use "pointed" line for marker tool
    scene()->drawLineTo(itemPos, mStrokeWidth, UBDrawingController::drawingController()->stylusTool() != UBStylusTool::Marker);
}

void UBGraphicsGeoTriangle::EndLine()
{
    // We never come to this place
    scene()->inputDeviceRelease();
}
