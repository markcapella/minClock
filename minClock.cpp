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
const char* APP_ICON = "minClock.svg";
const char* INI_FILENAME = "/minClock.ini";
const float WINDOW_OPACITY = .7;

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

BaseWidget* windowWidget;

QVBoxLayout* parentLayout;

QHBoxLayout* timeLayout;
QLabel* timeLabel;
QLabel* dateLabel;

QHBoxLayout* alarmLayout;
QPushButton* alarmButton;
QDateTimeEdit* alarmEdit;
QPushButton* editCancel;
QPushButton* editOk;


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

void updateClockFace();

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
    const QString appIniFile = QApplication::applicationDirPath() + INI_FILENAME;
    QSettings* settings = new QSettings(appIniFile, QSettings::IniFormat);

    const QString qTime = settings->value("alarmTime", "").toString();
    const STRING sTime = qTime.toStdString();
    return sTime;
}

void setAlarmTime(const STRING& time) {
    const QString appIniFile = QApplication::applicationDirPath() + INI_FILENAME;
    QSettings* settings = new QSettings(appIniFile, QSettings::IniFormat);

    settings->setValue("alarmTime", time.c_str());
}

STRING getAlarmDate() {
    const QString appIniFile = QApplication::applicationDirPath() + INI_FILENAME;
    QSettings* settings = new QSettings(appIniFile, QSettings::IniFormat);

    const QString qDate = settings->value("alarmDate", "").toString();
    const STRING sDate = qDate.toStdString();
    return sDate;
}

void setAlarmDate(const STRING& date) {
    const QString appIniFile = QApplication::applicationDirPath() + INI_FILENAME;
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
    alarmButton = new QPushButton();

    QObject::connect(alarmButton, &QPushButton::clicked, [=]() {
        if (getAlarmTime().empty()) {
            removeAlarmButton_Layout();
            addAlarmEdit_Layout();
        } else {
            setAlarmTime("");
            setAlarmDate("");
            alarmButton->setText("Alarm");
            alarmButton->setStyleSheet(ALARM_GRADIENT_GRAY);
        }
    });

    // Kludge to match alarm Button and Edit layout heights
    alarmButton->setFixedHeight(44);

    // Display button styling
    if (getAlarmTime().empty()) {
        alarmButton->setText("Alarm");
        alarmButton->setStyleSheet(ALARM_GRADIENT_GRAY);
    } else {
        alarmButton->setText(getAlarmTimeDate().c_str());
        alarmButton->setStyleSheet(ALARM_GRADIENT_YELLOW);
    }

    alarmLayout->addWidget(alarmButton);
}

void removeAlarmButton_Layout() {
    QObject::disconnect(alarmButton, &QPushButton::clicked, 0, 0);
    alarmLayout->removeWidget(alarmButton);
    delete alarmButton;
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
    alarmEdit = new QDateTimeEdit();
    alarmEdit->setTime(QTime::currentTime());
    alarmEdit->setDate(QDate::currentDate());

    alarmLayout->addWidget(alarmEdit);
}

void addAlarmEdit_CancelButton() {
    editCancel = new QPushButton();
    editCancel->setIcon(QIcon("cancelButton.png"));

    alarmLayout->addWidget(editCancel);

    QObject::connect(editCancel, &QPushButton::clicked, [=]() {
        removeAlarmEdit_Layout();
        addAlarmButton_Layout();
    });
}

void addAlarmEdit_OkButton() {
    editOk = new QPushButton();
    editOk->setIcon(QIcon("okButton.png"));

    alarmLayout->addWidget(editOk);

    QObject::connect(editOk, &QPushButton::clicked, [=]() {
        /* Store alarm time from edit windowWidget */
        const QTime editTime = alarmEdit->time();
        const STRING editHour = std::to_string(editTime.hour());
        const STRING editMinute = std::to_string(editTime.minute());
        const STRING editTimeString =
            addLeadZeroToNN(editHour) + ":" +
            addLeadZeroToNN(editMinute);

        setAlarmTime(editTimeString);

        /* Store alarm time from date windowWidget */
        const QDate editDate = alarmEdit->date();
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
    alarmLayout->removeWidget(alarmEdit);
    delete alarmEdit;
}

void removeAlarmEdit_CancelButton() {
    QObject::disconnect(editCancel, &QPushButton::clicked, 0, 0);

    alarmLayout->removeWidget(editCancel);
    delete editCancel;
}

void removeAlarmEdit_OkButton() {
    QObject::disconnect(editOk, &QPushButton::clicked, 0, 0);

    alarmLayout->removeWidget(editOk);
    delete editOk;
}

// *****************************************************************
// *** Clock face real-time rendering                            ***
// *****************************************************************

void updateClockFace() {
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

    // Pre-delete temp file.
    std::ignore = system("rm tempMinClock.svg 1>/dev/null 2>/dev/null");

    // Copy preMinClock.svg to tempMinClock.svg.
    std::ignore = system("cat preMinClock.svg > tempMinClock.svg");

    // Write custome hour and minute hands SVG insts.
    FILE* tempMinClock = fopen("tempMinClock.svg", "a");
    fprintf(tempMinClock, hourSVG.c_str(), "");
    fprintf(tempMinClock, "\n");
    fprintf(tempMinClock, minSVG.c_str(), "");
    fprintf(tempMinClock, "\n");
    fclose(tempMinClock);

    // Copy postMinClock.svg to tempMinClock.svg.
    std::ignore = system("cat postMinClock.svg >> tempMinClock.svg");

    // Delete current minClock.svg & replace with tempMinClock.svg.
    std::ignore = system("cp tempMinClock.svg minClock.svg");

    // Set on the icon after we update it.
    qApp->setWindowIcon(QIcon(APP_ICON));
}

// *****************************************************************
// *** minClock QT Application                                   ***
// *****************************************************************

int main(int argc, char* argv[]) {
    /* Create and setup app */
    QApplication mainApp(argc, argv);
    mainApp.setWindowIcon(QIcon(APP_ICON));

    /* Build main form */
    windowWidget = new BaseWidget();
    windowWidget->setWindowTitle(" ");
    windowWidget->setWindowFlags(Qt::WindowStaysOnTopHint);
    windowWidget->setWindowOpacity(WINDOW_OPACITY);
    //windowWidget->setStyleSheet("background-color: yellow;");

    parentLayout = new QVBoxLayout();
    timeLayout = new QHBoxLayout();
    alarmLayout = new QHBoxLayout();

    timeLabel = new QLabel();
    timeLabel->setText(getCurrentTime().c_str());
    timeLabel->setAlignment(Qt::AlignCenter);
    timeLabel->setStyleSheet("font-size: 36px;");

    dateLabel = new QLabel();
    dateLabel->setText(getCurrentDate().c_str());
    dateLabel->setAlignment(Qt::AlignCenter);
    dateLabel->setStyleSheet("font-size: 26px; color: #4c4545");

    timeLayout->addWidget(timeLabel);
    timeLayout->addWidget(dateLabel);
    parentLayout->addLayout(timeLayout);

    addAlarmButton_Layout();
    parentLayout->addLayout(alarmLayout);

    windowWidget->setLayout(parentLayout);
    windowWidget->resize(
        windowWidget->getWindowWidthAttr(),
        windowWidget->getWindowHeightAttr());
    windowWidget->move(
        windowWidget->getWindowPosXAttr(),
        windowWidget->getWindowPosYAttr());

    /* Set Date/time Textview Update Timer on 1 second Interval */
    QTimer* dateTime_updateTimer = new QTimer();
    dateTime_updateTimer->setInterval(1000);
    QObject::connect(dateTime_updateTimer, &QTimer::timeout, [=]() {
        /* Update time & date each second */
        timeLabel->setText(getCurrentTime().c_str());
        dateLabel->setText(getCurrentDate().c_str());

        /* Alarm check, is it time ? */
        if (getAlarmTimeDate().compare(getCurrentTimeDate()) == 0) {
            alarmButton->setText("STOP");
            alarmButton->setStyleSheet(ALARM_GRADIENT_RED);
            QSound::play("alarmBeep.wav");
        }
    });
    dateTime_updateTimer->start();

    /* Initial Clock-hands Update */
    updateClockFace();

    /* Set Clock-hands Update Timer on 1 minute Interval */
    QTimer* clockHands_updateTimer = new QTimer();
    clockHands_updateTimer->setInterval(1000 * 60);
    QObject::connect(clockHands_updateTimer, &QTimer::timeout, [=]() {
        /* Update clock hands each minute */
        updateClockFace();
    });
    clockHands_updateTimer->start();

    /* Set windowWidget size, show and exit */
    windowWidget->show();
    return mainApp.exec();
}
