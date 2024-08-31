#ifndef CUSTOMLISTWIDGET_H
#define CUSTOMLISTWIDGET_H

#include <QListWidget>
#include <QWidget>
#include <QMap>
#include <QIcon>
#include <QSize>
#include "QString"
#include "QJsonArray"
#include "QLayout"
#include "QScrollArea"
#include "dp_audioapi.h"
#include "GUI_BASE.h"
#include "filemanager.h"
#include "imageloader.h"
#include "QPushButton"
#include "QLabel"
#include "QLayout"
#include "QFrame"
#include "QPair"
#include "draggableitem.h"
#include <QListWidget>
#include <QListWidgetItem>
#include <QVariant>
#include <QPair>
#include <QList>

class CustomListWidget : public QListWidget {
    Q_OBJECT

public:
    CustomListWidget(QWidget *wid, QWidget *parent = nullptr );

signals:
    void OnButton1(QListWidgetItem* wo);
    void OnButton2(QListWidgetItem* wo);
    void OnButton3(QListWidgetItem* wo);
    void onRow();

protected:
    void dragMoveEvent(QDragMoveEvent *event) override;

private slots:
    void handleRowsInserted(const QModelIndex &parent, int first, int last);

public:
    QList<yt_music*> getNewList();
    void deleteMe(QListWidgetItem *item);
    //void addListener(std::function<void()> fct, std::function<void(QListWidgetItem*)> fct2);
    //void removeListener();
    void button1(QListWidgetItem *item);
    void button2(QListWidgetItem *item);
    void button3(QListWidgetItem *item);

private:
    QWidget *wid;
    QString type;
    QMap<QWidget*, QListWidgetItem*> table;
    std::function<void()> listener;
    std::function<void(QListWidgetItem*)> listener2;
};

#endif // CUSTOMLISTWIDGET_H
