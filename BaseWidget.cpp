// *****************************************************************
// *** BaseWidget with resize override for app window presistence***
// *****************************************************************

#include <iostream>

#include <QApplication>
#include <QSettings>
#include <QWidget>
#include <QRect>

#include <QResizeEvent>
#include <QMoveEvent>

#include "BaseWidget.h"

using namespace std;

BaseWidget::BaseWidget(QWidget *parent) :
    QWidget(parent) {

    iniPath = std::getenv("HOME") + iniPath.append("/.local/minClock/minClock.ini");
}

int
BaseWidget::getWindowHeightAttr() {
    const QString winIniFile = iniPath;
    QSettings* settings = new QSettings(winIniFile, QSettings::IniFormat);

    return settings->value("windowHeight", 100).toInt();
}

int
BaseWidget::getWindowWidthAttr() {
    const QString winIniFile = iniPath;
    QSettings* settings = new QSettings(winIniFile, QSettings::IniFormat);

    return settings->value("windowWidth", 285).toInt();
}

int
BaseWidget::getWindowPosXAttr() {
    const QString winIniFile = iniPath;
    QSettings* settings = new QSettings(winIniFile, QSettings::IniFormat);

    return settings->value("windowPosX", 100).toInt();
}

int
BaseWidget::getWindowPosYAttr() {
    const QString winIniFile = iniPath;
    QSettings* settings = new QSettings(winIniFile, QSettings::IniFormat);

    return settings->value("windowPosY", 100).toInt();
}

void
BaseWidget::resizeEvent(QResizeEvent* event) {
    const QString winIniFile = iniPath;
    QSettings* settings = new QSettings(winIniFile, QSettings::IniFormat);

    QString height, width;
    height.setNum(event->size().height());
    width.setNum(event->size().width());

    settings->setValue("windowHeight", height);
    settings->setValue("windowWidth", width);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void
BaseWidget::moveEvent(QMoveEvent* event) {
    const QString winIniFile = iniPath;
    QSettings* settings = new QSettings(winIniFile, QSettings::IniFormat);

    QString x, y;
    QRect winPos = frameGeometry();
    x.setNum(winPos.x());
    y.setNum(winPos.y());

    settings->setValue("windowPosY", y);
    settings->setValue("windowPosX", x);
}
#pragma GCC diagnostic pop

BaseWidget::~BaseWidget() { }
