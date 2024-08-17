#include "GUI_BASE.h"
#include "./ui_GUI_BASE.h"

MainWindowt::MainWindowt(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindowDP)


{
    //QPoint dragPos;
    canMove = false;
    winState = 0;
    style_bt_standard = "QPushButton { background-image: ICON_REPLACE; background-position: left center; background-repeat: no-repeat; border: none; border-left: 28px solid rgb(27, 29, 35); background-color: rgb(27, 29, 35); text-align: left; padding-left: 45px; } QPushButton[Active=true] { background-image: ICON_REPLACE; background-position: left center; background-repeat: no-repeat; border: none; border-left: 28px solid rgb(27, 29, 35); border-right: 5px solid rgb(44, 49, 60); background-color: rgb(27, 29, 35); text-align: left; padding-left: 45px; } QPushButton:hover { background-color: rgb(33, 37, 43); border-left: 28px solid rgb(33, 37, 43); } QPushButton:pressed { background-color: rgb(85, 170, 255); border-left: 28px solid rgb(85, 170, 255); }";

    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowIcon(QIcon(":/16x16/icons/logo_new.png"));
    setWindowTitle("Deltaplayer");
    labelTitle("Deltaplayer");
    labelDescription("");
    QSize startSize = QSize(1500, 900);
    resize(startSize);
    setMinimumSize(startSize);



    connect(ui->btn_close,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui->btn_minimize,SIGNAL(clicked()),this,SLOT(showMinimized()));
    connect(ui->btn_maximize_restore,SIGNAL(clicked()),this,SLOT(maximize_restore()));
    connect(ui->btn_toggle_menu,SIGNAL(clicked()),this,SLOT(toggleMenu()));

    ui->stackedWidget->setMinimumWidth(20);
    addNewMenu("Playlists", "btn_playlists", "url(:/20x20/icons/20x20/cil-music-note.png)", true);
    addNewMenu("Games", "btn_games", "url(:/16x16/icons/16x16/cil-gamepad.png)", true);
    addNewMenu("Settings","btn_settings","url(:/16x16/icons/16x16/cil-settings.png)",true);
    selectStandardMenu("btn_playlists");
    ui->stackedWidget->setCurrentWidget(ui->page_playlists);
    userIcon(getenv("username"), "", true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}


MainWindowt::~MainWindowt()
{
    delete ui;
}

//Reste

void MainWindowt::labelTitle(QString text) {
    ui->label_title_bar_top->setText(text);
}

void MainWindowt::labelDescription(QString text) {
    ui->label_top_info_1->setText(text);
}

void MainWindowt::userIcon(QString initialsTooltip,QString icon, bool showHide) {
    if (showHide==true) {

        ui->label_user_icon->setText(initialsTooltip);

        if (icon != "") {
            QString style = ui->label_user_icon->styleSheet();
            QString setIcon = "QLabel { background-image: " + icon + "; }";
            ui->label_user_icon->setStyleSheet(style + setIcon);
            ui->label_user_icon->setText("");
            ui->label_user_icon->setToolTip(initialsTooltip);
        }
    }else {
        ui->label_user_icon->hide();
    }
}
//Menu

void MainWindowt::toggleMenu(){

    int maxWidth = 220;
    bool enable = true;
    if (enable == true){
        // GET WIDTH
        int width = ui->frame_left_menu->width();
        int maxExtend = maxWidth;
        int standard = 70;

        //SET MAX WIDTH
        int widthExtended;
        if (width == 70){
            widthExtended = maxExtend;
        }
        else{
            widthExtended = standard;
        }

        //ANIMATION
        QPropertyAnimation *animation = new QPropertyAnimation(ui->frame_left_menu, "minimumWidth");
        animation->setDuration(300);
        animation->setStartValue(width);
        animation->setEndValue(widthExtended);
        animation->setEasingCurve(QEasingCurve::InOutQuart);
        animation->start();

    }

}

void MainWindowt::addNewMenu(QString name, QString objName, QString icon, bool isTopMenu){
    QFont font = QFont();
    font.setFamily("Segoe UI");
    QPushButton *button = new QPushButton(this);
    button->setObjectName(objName);
    QSizePolicy sizePolicy3 = QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(button->sizePolicy().hasHeightForWidth());
    button->setSizePolicy(sizePolicy3);
    button->setMinimumSize(QSize(0, 70));
    button->setLayoutDirection(Qt::LeftToRight);
    button->setFont(font);
    QString kk = style_bt_standard;
    kk.replace("ICON_REPLACE", icon);
    button->setStyleSheet(kk);
    button->setText(name);
    button->setToolTip(name);
    connect(button,SIGNAL(clicked()),this,SLOT(ButtonMenu()));

    if (isTopMenu){
        ui->layout_menus->addWidget(button);
    }
    else{
        ui->layout_menu_bottom->addWidget(button);
    }

}

void MainWindowt::selectStandardMenu(QString widget){
    for (QWidget* w : ui->frame_left_menu->findChildren<QWidget*>()) {
        if (w->objectName() == widget) {
            w->setStyleSheet(selectMenu(w->styleSheet()));
        }
    }
}

void MainWindowt::resetStyle(QString widget){
    for (QWidget* w : ui->frame_left_menu->findChildren<QWidget*>()) {
        if (w->objectName() != widget) {
            w->setStyleSheet(deselectMenu(w->styleSheet()));
        }
    }
}

QString MainWindowt::selectMenu(QString getStyle) {
    QString select = getStyle + ("QPushButton { border-right: 7px solid rgb(44, 49, 60); }");
    return select;
}

QString MainWindowt::deselectMenu(QString getStyle){
    QString deselect = getStyle.replace("QPushButton { border-right: 7px solid rgb(44, 49, 60); }", "");
    return deselect;
}

void MainWindowt::labelPage(QString text) {
    QString newText = "| " + text.toUpper();
    ui->label_top_info_2->setText(newText);
}

void MainWindowt::ButtonMenu() {
    QWidget* btnWidget = qobject_cast<QWidget*>(sender());

    // PAGE Playlists
    if ( btnWidget->objectName() == "btn_playlists"){
        ui->stackedWidget->setCurrentWidget(ui->page_playlists);
        resetStyle( "btn_playlists");
        labelPage( "My Playlists");
        btnWidget->setStyleSheet(selectMenu(btnWidget->styleSheet()));
    }

            // PAGE Games
    if ( btnWidget->objectName() == "btn_games"){
        ui->stackedWidget->setCurrentWidget(ui->page_games);
        resetStyle( "btn_games");
        labelPage( "My Games");
        btnWidget->setStyleSheet(selectMenu(btnWidget->styleSheet()));

    }

                // Settings
    if ( btnWidget->objectName() == "btn_settings"){
        ui->stackedWidget->setCurrentWidget(ui->page_settings);
        resetStyle( "btn_settings");
        labelPage( "Settings");
        btnWidget->setStyleSheet(selectMenu(btnWidget->styleSheet()));


    }
    qInfo("Go to "+btnWidget->objectName().toLatin1());
}


// Maximize

void MainWindowt::maximize_restore() {
    if (winState == 0) {
        showMaximized();
        ui->horizontalLayout->setContentsMargins(0, 0, 0, 0);
        ui->btn_maximize_restore->setToolTip("Restore");
        ui->btn_maximize_restore->setIcon(QIcon(":/16x16/icons/16x16/cil-window-restore.png"));
        ui->frame_top_btns->setStyleSheet("background-color: rgb(27, 29, 35)");
        ui->frame_size_grip->hide();
        winState = 1;
    } else {
        showNormal();
        resize(width()+1, height()+1);
        ui->horizontalLayout->setContentsMargins(10, 10, 10, 10);
        ui->btn_maximize_restore->setToolTip("Maximize");
        ui->btn_maximize_restore->setIcon(QIcon(":/16x16/icons/16x16/cil-window-maximize.png"));
        ui->frame_top_btns->setStyleSheet("background-color: rgba(27, 29, 35, 200)");
        ui->frame_size_grip->show();
        winState = 0;
    }
}

void MainWindowt::mouseDoubleClickEvent(QMouseEvent *event){
    QWidget* widget = childAt(event->pos());;
    QString name;
    if (widget == 0) {return;}
    name = widget->objectName();

    if ( name == "label_title_bar_top" ) {maximize_restore();}
}

// Mouve

void MainWindowt::mousePressEvent(QMouseEvent *event) {
    QWidget* widget = childAt(event->pos());;
    QString name;
    if (widget == 0) {return;}
    name = widget->objectName();

    if ( name == "label_title_bar_top" ) {canMove = true;}
    cur_pos = event->globalPosition().toPoint();
}


void MainWindowt::mouseReleaseEvent(QMouseEvent *event) {
    canMove = false;
}

void MainWindowt::mouseMoveEvent(QMouseEvent *event) {

    if ( canMove == true ) {

        new_pos = QPoint(event->globalPosition().toPoint()- cur_pos);
        move(x()+new_pos.x(),y()+new_pos.y());
        cur_pos = event->globalPosition().toPoint();
    }
}



