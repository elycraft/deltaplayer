#include "DraggableItem.h"
#include <QVBoxLayout>
#include <QPixmap>
#include <QStyle>

DraggableItem::DraggableItem(QString b1, QString b2, QString b3, QWidget *parent)
    : QWidget(parent), pere(parent) {

    QString icon1 = b1;
    QString icon2 = b2;
    QString icon3 = b3;
    //QString thumbUrl = "";

    // Create the frame for the item
    nwidget = new QFrame(this);
    nwidget->setMinimumSize(QSize(0, 80));
    nwidget->setStyleSheet("background-color: rgb(39, 44, 54);"
                           "border-radius: 15px;");
    nwidget->setObjectName("horizontalWidget");

    // Layout for the frame
    QHBoxLayout *horizontalLayout = new QHBoxLayout(nwidget);
    horizontalLayout->setObjectName("horizontalLayout_16");

    // Add spacer item
    QSpacerItem *spacerItem2 = new QSpacerItem(10, 19, QSizePolicy::Fixed, QSizePolicy::Minimum);
    horizontalLayout->addItem(spacerItem2);

    // Add play button
    if (icon1 != "") {
        fbutton1 = new QPushButton(nwidget);
        fbutton1->setText("");
        QIcon playIcon;
        playIcon.addPixmap(QPixmap(icon1), QIcon::Normal, QIcon::Off);
        fbutton1->setIcon(playIcon);
        horizontalLayout->addWidget(fbutton1);
        // Add another spacer item
        QSpacerItem *spacerItem2u = new QSpacerItem(10, 19, QSizePolicy::Fixed, QSizePolicy::Minimum);
        horizontalLayout->addItem(spacerItem2u);
    } else {
        fbutton1 = new QPushButton(nwidget);
        fbutton1->setDisabled(true);
        fbutton1->setVisible(false);
        horizontalLayout->addWidget(fbutton1);
    }



    // Add label for image
    thumb = new QLabel(nwidget);
    thumb->setMinimumSize(QSize(60, 7));
    thumb->setMaximumSize(QSize(75, 75));
    thumb->setText("");
    thumb->setScaledContents(true);
    thumb->setObjectName("label_3");
    horizontalLayout->addWidget(thumb);

    // Add spacer item
    QSpacerItem *spacerItem3 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
    horizontalLayout->addItem(spacerItem3);

    // Add label for text
    dtext = new QLabel(nwidget);
    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    font.setWeight(QFont::Weight(75));
    dtext->setFont(font);
    dtext->setScaledContents(false);
    dtext->setObjectName("dtext");
    horizontalLayout->addWidget(dtext);

    // Add expanding spacer item
    QSpacerItem *spacerItem4 = new QSpacerItem(100, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout->addItem(spacerItem4);

    // Add delete button
    if (icon2 != "") {
        fbutton2 = new QPushButton(nwidget);
        fbutton2->setText("");
        QIcon deleteIcon;
        deleteIcon.addPixmap(QPixmap(icon2), QIcon::Normal, QIcon::Off);
        fbutton2->setIcon(deleteIcon);
        horizontalLayout->addWidget(fbutton2);
        // Add another spacer item
        QSpacerItem *spacerItem2ue = new QSpacerItem(10, 19, QSizePolicy::Fixed, QSizePolicy::Minimum);
        horizontalLayout->addItem(spacerItem2ue);
    } else {
        fbutton2 = new QPushButton(nwidget);
        fbutton2->setDisabled(true);
        fbutton2->setVisible(false);
        horizontalLayout->addWidget(fbutton2);
    }

    // Add delete button
    if (icon3 != "") {
        fbutton3 = new QPushButton(nwidget);
        fbutton3->setText("");
        QIcon deIcon;
        deIcon.addPixmap(QPixmap(icon3), QIcon::Normal, QIcon::Off);
        fbutton2->setIcon(deIcon);
        horizontalLayout->addWidget(fbutton3);
        // Add another spacer item
        QSpacerItem *spacerItem2uf = new QSpacerItem(10, 19, QSizePolicy::Fixed, QSizePolicy::Minimum);
        horizontalLayout->addItem(spacerItem2uf);
    } else {
        fbutton3 = new QPushButton(nwidget);
        fbutton3->setDisabled(true);
        fbutton3->setVisible(false);
        horizontalLayout->addWidget(fbutton3);
    }


    // Add final spacer item
    QSpacerItem *spacerItem2ud = new QSpacerItem(20, 19, QSizePolicy::Fixed, QSizePolicy::Minimum);
    horizontalLayout->addItem(spacerItem2ud);

    // Main layout
    QHBoxLayout *allQHBoxLayout = new QHBoxLayout();
    allQHBoxLayout->addWidget(nwidget, 0);
    setLayout(allQHBoxLayout);
}

void DraggableItem::setDText(const QString &text1) {
    dtext->setText(text1);
}

void DraggableItem::setThumb(const QString &imagePath) {
    thumb->setStyleSheet(QString("border-radius: 2px;"
                                   "border-image: url('%1') 0 0 0 0 stretch stretch").arg(imagePath));
}

void DraggableItem::setId(QString id) {
    this->id = id;
}
