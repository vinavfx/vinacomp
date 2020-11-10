#ifndef CURVE_VIEW_HPP
#define CURVE_VIEW_HPP

#include <gl_view.hpp>

class curve_view : public gl_view
{

private:
    struct key_frame
    {
        QPointF pos;
        int rotation;
        float exaggeration; // 0.0 - 1.0
        int interpolation;
        // 0: Linear
        // 1: Horizontal
        // 2: Smooth
        // 3: Break
        // 4: Custom
    };

    QMap<QString, QList<key_frame>> curves;

    // Drag KeyFrame
    QString drag_curve;
    int drag_index;
    bool is_drag;
    //

    void draw_circle();
    void draw_grid();
    void draw_coordinate_numbers();
    void create_handle(QPoint position);
    void draw_point(QPointF coords);
    void create_curve();
    void draw_curve();
    QPointF rotate_point(QPointF point, QPointF anchor_point, float angle);
    void draw_bezier(QPointF src_key, QPointF dst_key);
    QPointF cubic_bezier(
        QPointF point_a,
        QPointF point_b,
        float value,
        float exaggeration = 0.34);

public:
    curve_view(/* args */);
    ~curve_view();

protected:
    void initializeGL() override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
};

#endif // CURVE_VIEW_HPP