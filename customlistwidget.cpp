#include "CustomListWidget.h"
#include <QDragMoveEvent>

#include <QListWidgetItem>
#include <QDebug>
#include "GUI_BASE.h"
#include "./ui_GUI_BASE.h"

CustomListWidget::CustomListWidget(QWidget *wid, QWidget *parent )
    : QListWidget(parent) {
    setIconSize(QSize(124, 124));
    setDragDropMode(QAbstractItemView::DragDrop);
    setDefaultDropAction(Qt::MoveAction);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setAcceptDrops(true);

    connect(model(), &QAbstractItemModel::rowsInserted, this, &CustomListWidget::handleRowsInserted);
}

void CustomListWidget::handleRowsInserted(const QModelIndex &parent, int first, int last) {
    for (int index = first; index <= last; ++index) {
        QListWidgetItem *item = this->item(index);
        //qInfo() << "cl";

        QVariant data = item->data(Qt::UserRole);
        //qInfo() << data;
        QList<QVariant> dataList = data.toList();
        if (!data.isValid()) {
            qWarning() << "Données invalides dans item à l'index" << index;
            continue;
        }


        if (item && !itemWidget(item) && data.isValid()) {//if (item && !itemWidget(item) && data.isValid()) {

            QString c1 = dataList[0].toString();
            QString c2 = dataList[1].toString();
            QString c3 = dataList[2].toString();
            QString texte = dataList[3].toString();
            QString thumb = dataList[4].toString();
            QString id = dataList[5].toString();

            DraggableItem *widgete = new DraggableItem(c1,c2,c3,this); // Assuming 'wid' is a QWidget derived class
            widgete->setDText(texte);
            widgete->setThumb(thumb);
            widgete->setId(id);

            connect(widgete->fbutton1, &QPushButton::clicked, [this, item]() { CustomListWidget::button1(item); } );
            connect(widgete->fbutton2, &QPushButton::clicked, [this, item]() { CustomListWidget::button2(item); } );
            connect(widgete->fbutton3, &QPushButton::clicked, [this, item]() { CustomListWidget::button3(item); } );

            item->setSizeHint(widgete->sizeHint());
            table[widgete] = item;

            setItemWidget(item, widgete);
        }
    }
}

void CustomListWidget::dragMoveEvent(QDragMoveEvent *event) {
    int target = row(itemAt(event->position().toPoint()));
    int current = currentRow();

    if ((target == current + 1) || (current == count() - 1 && target == -1)) {
        event->ignore();
    } else {
        QListWidget::dragMoveEvent(event);

    }
}

QList<yt_music*> CustomListWidget::getNewList() {
    QList<yt_music*> nl;
    for (int i = 0; i < count(); ++i) {
        QListWidgetItem *item = this->item(i);
        QList<QVariant> dataList = item->data(Qt::UserRole).toList();;
        yt_music* id = dataList[6].value<yt_music*>();
        nl.append(id);

    }
    return nl;
}

void CustomListWidget::button1(QListWidgetItem *item) {
    emit this->OnButton1(item);
}

void CustomListWidget::button2(QListWidgetItem *item) {
    emit this->OnButton2(item);

}

void CustomListWidget::button3(QListWidgetItem *item) {
    emit this->OnButton3(item);

}
