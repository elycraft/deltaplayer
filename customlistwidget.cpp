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
        qInfo() << "cl";

        QVariant data = item->data(Qt::UserRole);
        //qInfo() << data;
        QList<QVariant> dataList = data.toList();
        if (!data.isValid()) {
            qWarning() << "Données invalides dans item à l'index" << index;
            continue;
        }


        if (item && !itemWidget(item) && data.isValid()) {//if (item && !itemWidget(item) && data.isValid()) {

            yt_music* id = dataList[0].value<yt_music*>();
            QString title = dataList[1].toString();
            QString author = dataList[2].toString();
            QString thumb = dataList[3].toString();

            DraggableItem *widgete = new DraggableItem(this); // Assuming 'wid' is a QWidget derived class
            // Setup widget with data
            widgete->setTexte(title, author);
            widgete->setIcone(thumb);
            widgete->setId(id);

            connect(widgete->findChild<QPushButton*>("deletetkt"), &QPushButton::clicked, [this, item]() { deleteMe(item); });
            connect(widgete->dplay, &QPushButton::clicked, [this, item]() { CustomListWidget::dplay_btn(item); } );

            item->setSizeHint(widgete->sizeHint());
            table[widgete] = item;

            setItemWidget(item, widgete);
        }
    }
}

void CustomListWidget::dragMoveEvent(QDragMoveEvent *event) {
    int target = row(itemAt(event->pos()));
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
        yt_music* id = dataList[0].value<yt_music*>();
        nl.append(id);

    }
    return nl;
}

void CustomListWidget::deleteMe(QListWidgetItem *item) {
    takeItem(row(item));
    if (listener) {
        listener();
    }
}

void CustomListWidget::dplay_btn(QListWidgetItem *item) {
    qInfo() << item;
    emit this->hasPlayAt(item);

}

void CustomListWidget::addListener(std::function<void()> fct, std::function<void(QListWidgetItem*)> fct2) {
    listener = fct;
    listener2 = fct2;
}

void CustomListWidget::removeListener() {
    listener = nullptr;
    listener2 = nullptr;
}
