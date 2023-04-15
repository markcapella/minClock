#ifndef BASE_WIDGET_H
#define BASE_WIDGET_H

#include <QWidget>
#include <QPoint>


// *****************************************************************
// *** BaseWidget with resize override for app window presistence***
// *****************************************************************

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
        QString iniPath;

    protected:
        void resizeEvent(QResizeEvent *event);
        void moveEvent(QMoveEvent *event);
};

#endif /* BASE_WIDGET_H */
