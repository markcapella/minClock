#ifndef BASE_WIDGET_H
#define BASE_WIDGET_H

// *****************************************************************
// *** BaseWidget with resize override for app window presistence***
// *****************************************************************

#include <QWidget>
#include <QPoint>

class BaseWidget : public QWidget {
    Q_OBJECT

    public:
        explicit BaseWidget(QWidget *parent = nullptr);
    ~BaseWidget();

    public:
        int getWindowHeightAttr();
        int getWindowWidthAttr();
        int getWindowPosXAttr();
        int getWindowPosYAttr();

    private:
        const char* INI_FILENAME = "/minClock.ini";

    protected:
        void resizeEvent(QResizeEvent *event);
        void moveEvent(QMoveEvent *event);
};

#endif /* BASE_WIDGET_H */
