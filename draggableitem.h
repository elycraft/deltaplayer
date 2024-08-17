#ifndef DRAGGABLEITEM_H
#define DRAGGABLEITEM_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QSpacerItem>
#include <QHBoxLayout>
#include <QFrame>
#include <QIcon>
#include <QPixmap>
#include <QSize>
#include <QFont>
#include "yt_music.h"

class DraggableItem : public QWidget {
    Q_OBJECT

public:
    explicit DraggableItem(QWidget *parent = nullptr);

    void setTexte(const QString &text1, const QString &text2);
    void setIcone(const QString &imagePath);
    void setId(yt_music* id);
    QPushButton *dplay;

private:
    yt_music* id;
    QFrame *nwidget;
    QLabel *label_3;
    QLabel *label;

    QPushButton *deletetkt;
    QWidget *pere;
};

#endif // DRAGGABLEITEM_H
