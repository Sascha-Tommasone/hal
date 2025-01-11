#include "gui/graph_widget/items/utility_items/node_drag_shadow.h"

#include "gui/graph_widget/items/nodes/gates/graphics_gate.h"

#include <assert.h>

#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QPainter>
#include <QPaintEvent>
#include <iostream>

namespace hal
{
    QPen NodeDragShadow::sPen;
    qreal NodeDragShadow::sLod;

    QColor NodeDragShadow::sColorPen[3];
    QColor NodeDragShadow::sColorSolid[3];
    QColor NodeDragShadow::sColorTranslucent[3];

    void NodeDragShadow::loadSettings()
    {
        sColorPen[static_cast<int>(DragCue::Rejected)] = QColor(166, 31, 31, 255);
        sColorPen[static_cast<int>(DragCue::Movable)] = QColor(48, 172, 79, 255);
        sColorPen[static_cast<int>(DragCue::Swappable)] = QColor(37, 97, 176, 255);

        sColorSolid[static_cast<int>(DragCue::Rejected)] = QColor(166, 31, 31, 200);
        sColorSolid[static_cast<int>(DragCue::Movable)] = QColor(48, 172, 79, 200);
        sColorSolid[static_cast<int>(DragCue::Swappable)] = QColor(37, 97, 176, 200);

        sColorTranslucent[static_cast<int>(DragCue::Rejected)] = QColor(166, 31, 31, 150);
        sColorTranslucent[static_cast<int>(DragCue::Movable)] = QColor(48, 172, 79, 150);
        sColorTranslucent[static_cast<int>(DragCue::Swappable)] = QColor(37, 97, 176, 150);

        sPen.setCosmetic(true);
        sPen.setJoinStyle(Qt::MiterJoin);
    }

    NodeDragShadow::NodeDragShadow()
        : QGraphicsItem(), mRect(0,0,100,100)
    {
        setAcceptedMouseButtons(0);
    }

    void NodeDragShadow::start(const QPointF& posF, const QSizeF& sizeF)
    {
        mRect = QRectF(QPointF(0,0),sizeF);
        setPos(posF);
        setZValue(1);
        show();
    }

    /*
    qreal NodeDragShadow::width() const
    {
        return mWidth;
    }

    qreal NodeDragShadow::height() const
    {
        return mHeight;
    }

    QSizeF NodeDragShadow::size() const
    {
        return QSizeF(mWidth, mHeight);
    }

    void NodeDragShadow::setWidth(const qreal width)
    {
        mWidth = width;
    }

    void NodeDragShadow::setHeight(const qreal height)
    {
        mHeight = height;
    }
*/

    void NodeDragShadow::setLod(const qreal lod)
    {
        sLod = lod;
    }

    void NodeDragShadow::setVisualCue(const DragCue cue)
    {
        mCue = cue;
        update();
    }

    void NodeDragShadow::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
    {
        Q_UNUSED(option)
        Q_UNUSED(widget)

        int color_index = static_cast<int>(mCue);
        assert(color_index <= 3);

        sPen.setColor(sColorPen[color_index]);
        painter->setPen(sPen);

        if (sLod < 0.5)
        {

            painter->fillRect(mRect, sColorSolid[color_index]);
        }
        else
        {
            painter->drawRect(mRect);
            painter->fillRect(mRect, sColorTranslucent[color_index]);
        }
    }

    QRectF NodeDragShadow::boundingRect() const
    {
        return mRect;
    }

    QPainterPath NodeDragShadow::shape() const
    {
        QPainterPath path;
        path.addRect(mRect);
        return path;
    }
}
