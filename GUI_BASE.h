#ifndef GUI_BASE_H
#define GUI_BASE_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QPoint>
#include <QFrame>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QSizePolicy>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindowDP;
class topBar;
}
QT_END_NAMESPACE

class MainWindowt : public QMainWindow
{
    Q_OBJECT

public:
    MainWindowt(QWidget *parent = nullptr);
    ~MainWindowt();
    Ui::MainWindowDP *ui;
    bool canMove;
    int winState;
    QString style_bt_standard;


public slots:
    void maximize_restore();
    void toggleMenu();
    void ButtonMenu();

private:

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void addNewMenu(QString name, QString objName, QString icon, bool isTopMenu);
    void selectStandardMenu(QString widget);
    void resetStyle(QString widget);
    void labelPage(QString text);
    QString selectMenu(QString getStyle);
    QString deselectMenu(QString getStyle);
    void labelTitle(QString text);
    void labelDescription(QString text);
    void userIcon(QString initialsTooltip,QString icon, bool showHide);
    QPoint cur_pos;
    QPoint new_pos;

};

#endif
