// *****************************************************************
// *** minClock QT Application                                   ***
// ***    mark capella 6/4/22                                    ***
// *****************************************************************

#include <ctime>
#include <string>
#include <unistd.h>
#include <iostream>
 
#include <QDebug>
#include <QSize>
#include <QApplication>
#include <QSettings>
#include <QIcon>
#include <QCloseEvent>

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDateTimeEdit>
#include <QTime>

#include <QTimer>
#include <QSound>

#include "BaseWidget.h"

using namespace std;  
using STRING = std::string;

/* Global consts */
const STRING mDeskSession = std::getenv("XDG_SESSION_DESKTOP");

const float WINDOW_OPACITY = .7;

const int GNOME_ICON_HEIGHT = 24;
const int GNOME_ICON_WIDTH = 24;

const string MONTH_NAMES[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

const QString ALARM_GRADIENT_GRAY = "background-color: qlineargradient"
    "(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0.3 white, stop: 1 gray);"
    "border-style: solid; border-color: black; border-width: 1px; border-radius: 10px;";
const QString ALARM_GRADIENT_YELLOW = "background-color: qlineargradient"
    "(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0.15 white, stop: 1 #ffec96);"
    "border-style: solid; border-color: black; border-width: 1px; border-radius: 10px;";
const QString ALARM_GRADIENT_RED = "background-color: qlineargradient"
    "(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 white, stop: 1 red);"
    "border-style: solid; border-color: black; border-width: 1px; border-radius: 10px;";

// *****************************************************************
// *** Global var stubs                                          ***
// *****************************************************************

QString mIniPath;
STRING mIconPath;
QIcon* mIconImage;
QLabel* mGNOMEIconLabel;

BaseWidget* mWindowWidget;

QVBoxLayout* mParentLayout;
    QHBoxLayout* mTimeLayout;
        QLabel* mTimeLabel;
        QLabel* mDateLabel;

    QHBoxLayout* mAlarmLayout;
        QPushButton* mAlarmButton;
        QDateTimeEdit* mAlarmEdit;
        QPushButton* mEditCancelButton;
        QPushButton* mEditOkButton;

// *****************************************************************
// *** Global method stubs                                       ***
// *****************************************************************

void addAlarmButton_Layout();
void removeAlarmButton_Layout();

void addAlarmEdit_Layout();
void removeAlarmEdit_Layout();

void addAlarmEdit_Widget();
void addAlarmEdit_CancelButton();
void addAlarmEdit_OkButton();

void removeAlarmEdit_Widget();
void removeAlarmEdit_CancelButton();
void removeAlarmEdit_OkButton();

void updateClockFaceIcon();

// *****************************************************************
// *** Support methods                                           ***
// *****************************************************************

STRING addLeadZeroToNN(const STRING& NN) {
    STRING result = NN;
    if (result.length() < 2) {
        result = "0" + result;
    }
    return result;
}

// *****************************************************************
// *** Getters for CURRENT TIME VALUE                            ***
// *****************************************************************

STRING getCurrentTime() {
    const std::time_t time = std::time(0);
    const std::tm* now = std::localtime(&time);

    return addLeadZeroToNN(std::to_string(now->tm_hour)) + ":" +
        addLeadZeroToNN(std::to_string(now->tm_min));
}

STRING getCurrentDate() {
    const std::time_t time = std::time(0);
    const std::tm* now = std::localtime(&time);

    return MONTH_NAMES[now->tm_mon] + " " +
        std::to_string(now->tm_mday);
}

STRING getCurrentTimeDate() {
    return getCurrentTime() + " " + getCurrentDate();
}

// *****************************************************************
// *** Getter / setters for ALARM TIME VALUE                     ***
// *****************************************************************

STRING getAlarmTime() {
    const QString appIniFile = mIniPath;
    QSettings* settings = new QSettings(appIniFile, QSettings::IniFormat);

    const QString qTime = settings->value("alarmTime", "").toString();
    const STRING sTime = qTime.toStdString();
    return sTime;
}

void setAlarmTime(const STRING& time) {
    const QString appIniFile = mIniPath;
    QSettings* settings = new QSettings(appIniFile, QSettings::IniFormat);

    settings->setValue("alarmTime", time.c_str());
}

STRING getAlarmDate() {
    const QString appIniFile = mIniPath;
    QSettings* settings = new QSettings(appIniFile, QSettings::IniFormat);

    const QString qDate = settings->value("alarmDate", "").toString();
    const STRING sDate = qDate.toStdString();
    return sDate;
}

void setAlarmDate(const STRING& date) {
    const QString appIniFile = mIniPath;
    QSettings* settings = new QSettings(appIniFile, QSettings::IniFormat);

    settings->setValue("alarmDate", date.c_str());
}

STRING getAlarmTimeDate() {
    return getAlarmTime() + " " + getAlarmDate();
}

// *****************************************************************
// *** Getter / setters for ALARM BUTTON LAYOUT                  ***
// *****************************************************************

void addAlarmButton_Layout() {
    mAlarmButton = new QPushButton();

    QObject::connect(mAlarmButton, &QPushButton::clicked, [=]() {
        if (getAlarmTime().empty()) {
            removeAlarmButton_Layout();
            addAlarmEdit_Layout();
        } else {
            setAlarmTime("");
            setAlarmDate("");
            mAlarmButton->setText("Alarm");
            mAlarmButton->setStyleSheet(ALARM_GRADIENT_GRAY);
        }
    });

    // Kludge to match alarm Button and Edit layout heights
    mAlarmButton->setFixedHeight(44);

    // Display button styling
    if (getAlarmTime().empty()) {
        mAlarmButton->setText("Alarm");
        mAlarmButton->setStyleSheet(ALARM_GRADIENT_GRAY);
    } else {
        mAlarmButton->setText(getAlarmTimeDate().c_str());
        mAlarmButton->setStyleSheet(ALARM_GRADIENT_YELLOW);
    }

    mAlarmLayout->addWidget(mAlarmButton);
}

void removeAlarmButton_Layout() {
    QObject::disconnect(mAlarmButton, &QPushButton::clicked, 0, 0);
    mAlarmLayout->removeWidget(mAlarmButton);
    delete mAlarmButton;
}

// *****************************************************************
// *** Adds for ALARM EDIT LAYOUT                                ***
// *****************************************************************

void addAlarmEdit_Layout() {
    addAlarmEdit_Widget();
    addAlarmEdit_CancelButton();
    addAlarmEdit_OkButton();
}

void addAlarmEdit_Widget() {
    mAlarmEdit = new QDateTimeEdit();
    mAlarmEdit->setTime(QTime::currentTime());
    mAlarmEdit->setDate(QDate::currentDate());

    mAlarmLayout->addWidget(mAlarmEdit);
}

void addAlarmEdit_CancelButton() {
    mEditCancelButton = new QPushButton();
    mEditCancelButton->setIcon(QIcon("cancelButton.png"));

    mAlarmLayout->addWidget(mEditCancelButton);

    QObject::connect(mEditCancelButton, &QPushButton::clicked, [=]() {
        removeAlarmEdit_Layout();
        addAlarmButton_Layout();
    });
}

void addAlarmEdit_OkButton() {
    mEditOkButton = new QPushButton();
    mEditOkButton->setIcon(QIcon("okButton.png"));

    mAlarmLayout->addWidget(mEditOkButton);

    QObject::connect(mEditOkButton, &QPushButton::clicked, [=]() {
        /* Store alarm time from edit mWindowWidget */
        const QTime editTime = mAlarmEdit->time();
        const STRING editHour = std::to_string(editTime.hour());
        const STRING editMinute = std::to_string(editTime.minute());
        const STRING editTimeString =
            addLeadZeroToNN(editHour) + ":" +
            addLeadZeroToNN(editMinute);

        setAlarmTime(editTimeString);

        /* Store alarm time from date mWindowWidget */
        const QDate editDate = mAlarmEdit->date();
        const STRING editDateString =
            MONTH_NAMES[editDate.month() - 1] + " " +
            std::to_string(editDate.day());

        setAlarmDate(editDateString);

        /* Morph edit layout back to alarm button layout */
        removeAlarmEdit_Layout();
        addAlarmButton_Layout();
    });
}

// *****************************************************************
// *** Removes for ALARM EDIT LAYOUT                             ***
// *****************************************************************

void removeAlarmEdit_Layout() {
    removeAlarmEdit_Widget();
    removeAlarmEdit_CancelButton();
    removeAlarmEdit_OkButton();
}

void removeAlarmEdit_Widget() {
    mAlarmLayout->removeWidget(mAlarmEdit);
    delete mAlarmEdit;
}

void removeAlarmEdit_CancelButton() {
    QObject::disconnect(mEditCancelButton, &QPushButton::clicked, 0, 0);

    mAlarmLayout->removeWidget(mEditCancelButton);
    delete mEditCancelButton;
}

void removeAlarmEdit_OkButton() {
    QObject::disconnect(mEditOkButton, &QPushButton::clicked, 0, 0);

    mAlarmLayout->removeWidget(mEditOkButton);
    delete mEditOkButton;
}

// *****************************************************************
// *** Clock face real-time rendering                            ***
// *****************************************************************

void updateClockFaceIcon() {
    // Set off the icon while we update it.
    qApp->setWindowIcon(QIcon());

    // Get current time now->tm_hour, now->tm_min.
    const float TOT_SECONDS_IN_HOUR = 12 * 60 * 60;

    const std::time_t time = std::time(0);
    const std::tm* now = std::localtime(&time);
    const int nowHour = now->tm_hour;    // nowHour = 4; // test
    const int nowMin = now->tm_min;    // nowMin = 20; // test

    // Write clock hand string HOUR to tempMinClock.svg.
    const int hourMod = nowHour % 12;
    const float hourSec = (hourMod * 3600.0);
    const float minSec = (nowMin * 60.0);
    const float totSec = (hourSec + minSec);

    const float hourRot = totSec / TOT_SECONDS_IN_HOUR * 360.0;
    const int minRot = nowMin * 360 / 60;

    const string hourSVG = "    <rect x=\"23\" y=\"15\" width=\"2\" height=\"13\""
        " transform=\"rotate(" + std::to_string(hourRot) + ", 24, 24)\"/>";
    const string minSVG = "    <rect x=\"23\" y=\"10\" width=\"2\" height=\"18\""
        " transform=\"rotate(" + std::to_string(minRot) + ", 24, 24)\"/>";

    // Copy preMinClock.svg to tempMinClock.svg.
    std::ignore = system("cat preMinClock.svg > ~/.local/minClock/minClock.svg");

    // Write custom hour and minute hands SVG insts.
    FILE* tempMinClock = fopen(mIconPath.c_str(), "a");
    fprintf(tempMinClock, hourSVG.c_str(), "");
    fprintf(tempMinClock, "\n");
    fprintf(tempMinClock, minSVG.c_str(), "");
    fprintf(tempMinClock, "\n");
    fclose(tempMinClock);

    // Copy postMinClock.svg to tempMinClock.svg.
    std::ignore = system("cat postMinClock.svg >> ~/.local/minClock/minClock.svg");

    // Set on the icon after we update it.
    mIconImage = new QIcon(mIconPath.c_str());
    qApp->setWindowIcon(*mIconImage);

    // Add Icon to Window contents as GNOME doesn't use it in the titlebar.
    if (mDeskSession.find("GNOME") != std::string::npos) {
        mTimeLayout->removeWidget(mGNOMEIconLabel);
        mTimeLayout->removeWidget(mTimeLabel);
        mTimeLayout->removeWidget(mDateLabel);

        mGNOMEIconLabel = new QLabel();
        QPixmap iconPixMap(mIconPath.c_str());
        mGNOMEIconLabel->setPixmap(iconPixMap);

        mTimeLayout->addWidget(mGNOMEIconLabel);
        mTimeLayout->addWidget(mTimeLabel);
        mTimeLayout->addWidget(mDateLabel);
    }
}

// *****************************************************************
// *** minClock QT Application                                   ***
// *****************************************************************

int main(int argc, char* argv[]) {
    /* Create and setup app */
    QApplication mainApp(argc, argv);

    // Pre-define local temp folder, create initial app Icon
    std::ignore = system("mkdir -p ~/.local/minClock");

    mIconPath = std::getenv("HOME") + mIconPath.append("/.local/minClock/minClock.svg");
    mIniPath = std::getenv("HOME") + mIniPath.append("/.local/minClock/minClock.ini");

    /* Build main form */
    mWindowWidget = new BaseWidget();
    mWindowWidget->setWindowTitle(" ");
    mWindowWidget->setWindowFlags(Qt::WindowStaysOnTopHint);
    mWindowWidget->setWindowOpacity(WINDOW_OPACITY);

    mParentLayout = new QVBoxLayout();

    mTimeLayout = new QHBoxLayout();
    mTimeLabel = new QLabel();
    mTimeLabel->setText(getCurrentTime().c_str());
    mTimeLabel->setAlignment(Qt::AlignCenter);
    mTimeLabel->setStyleSheet("font-size: 36px;");
    mTimeLayout->addWidget(mTimeLabel);

    mDateLabel = new QLabel();
    mDateLabel->setText(getCurrentDate().c_str());
    mDateLabel->setAlignment(Qt::AlignCenter);
    mDateLabel->setStyleSheet("font-size: 26px; color: #4c4545");
    mTimeLayout->addWidget(mDateLabel);

    mParentLayout->addLayout(mTimeLayout);

    mAlarmLayout = new QHBoxLayout();
    addAlarmButton_Layout();
    mParentLayout->addLayout(mAlarmLayout);

    mWindowWidget->setLayout(mParentLayout);
    mWindowWidget->resize(
        mWindowWidget->getWindowWidthAttr(),
        mWindowWidget->getWindowHeightAttr());
    mWindowWidget->move(
        mWindowWidget->getWindowPosXAttr(),
        mWindowWidget->getWindowPosYAttr());

    /* Set Date/time Textview Update Timer on 1 second Interval */
    QTimer* dateTime_updateTimer = new QTimer();
    dateTime_updateTimer->setInterval(1000);
    QObject::connect(dateTime_updateTimer, &QTimer::timeout, [=]() {
        /* Update time & date each second */
        mTimeLabel->setText(getCurrentTime().c_str());
        mDateLabel->setText(getCurrentDate().c_str());

        /* Alarm check, is it time ? */
        if (getAlarmTimeDate().compare(getCurrentTimeDate()) == 0) {
            mAlarmButton->setText("STOP");
            mAlarmButton->setStyleSheet(ALARM_GRADIENT_RED);
            QSound::play("alarmBeep.wav");
        }
    });
    dateTime_updateTimer->start();

    /* Initial Clock-hands Update */
    updateClockFaceIcon();

    /* Set Clock-hands Update Timer on 1 minute Interval */
    QTimer* clockHands_updateTimer = new QTimer();
    clockHands_updateTimer->setInterval(1000 * 60);
    QObject::connect(clockHands_updateTimer, &QTimer::timeout, [=]() {
        /* Update clock hands each minute */
        updateClockFaceIcon();
    });
    clockHands_updateTimer->start();

    /* Set mWindowWidget size, show and exit */
    mWindowWidget->show();
    return mainApp.exec();
}
