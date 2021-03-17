#ifndef RENDER_DATA_H
#define RENDER_DATA_H

#include <QRect>
#include <QString>
#include <opencv2/opencv.hpp>

struct render_data
{
    cv::Mat image;
    int frame;
    int width;
    int height;
    int first_frame;
    int last_frame;
    QRect bbox;
    QString root_node;
};

#endif // RENDER_DATA_H
