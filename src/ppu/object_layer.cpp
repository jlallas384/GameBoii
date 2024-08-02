#include "object_layer.h"

#include "utils.h"

ObjectLayer::ObjectLayer(const Tile& t1, const ObjectData& data) : t1(t1), height(8) {
    initAttributes(data);
}

ObjectLayer::ObjectLayer(const Tile& t1, const Tile& t2, const ObjectData& data) : t1(t1), t2(t2), height(16) {
    initAttributes(data);
}

bool ObjectLayer::isAtScanline(uint8_t ly) const {
    return y - 16 <= ly && ly < y - 16 + height;
}

bool ObjectLayer::isIntersectAtPoint(uint8_t i, uint8_t j) const {
    return y - 16 <= i && i < y - 16 + height && x - 8 <= j && j < x;
}

uint8_t ObjectLayer::getColorIdAt(uint8_t i, uint8_t j) const {
    i -= y - 16;
    j -= x - 8;
    if (flipY) {
        i = height - i - 1;
    }
    if (flipX) {
        j = 8 - j - 1;
    }
    return i < 8 ? t1.at(i, j) : t2.at(i - 8, j);
}

uint8_t ObjectLayer::getPalette(uint8_t first, uint8_t second) const {
    return palette ? second : first;
}

bool ObjectLayer::isDrawn(uint8_t bgId) const {
    return !priority || !(1 <= bgId && bgId <= 3);
}

bool ObjectLayer::operator<(const ObjectLayer& other) const {
    return x < other.x;
}

void ObjectLayer::initAttributes(const ObjectData& data) {
    y = data.y;
    x = data.x;
    priority = getBit(data.attributes, 7);
    flipY = getBit(data.attributes, 6);
    flipX = getBit(data.attributes, 5);
    palette = getBit(data.attributes, 4);
}
