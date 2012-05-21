#pragma once

#include <QtGui/QImage>
#include <QtGui/QColor>

namespace Sprites {

QImage getShiningImage(int imageSize);
QImage getLightOnImage(int imageSize, int bulbSize);
QImage getLightOffImage(int imageSize, int bulbSize);
QImage getTubeImage(int length, int width, Qt::Orientation orientation);
QImage * makeSprite(int sprite, const QSize & cellSize, bool on = false);

const QColor & backgroundColor();

};
