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

class DraggableItem : public QWidget {
    Q_OBJECT

public:
    explicit DraggableItem(QString b1, QString b2, QString b3, QWidget *parent = nullptr);

    void setDText(const QString &text1);
    void setThumb(const QString &imagePath);
    void setId(QString id);
    void setDFormat(QString b1, QString b2, QString b3);
    QPushButton *fbutton1;
    QPushButton *fbutton2;
    QPushButton *fbutton3;

private:
    QString id;
    QFrame *nwidget;
    QLabel *thumb;
    QLabel *dtext;

    QPushButton *deletetkt;
    QWidget *pere;
};

#endif // DRAGGABLEITEM_H
