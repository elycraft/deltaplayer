#include "DraggableItem.h"
#include <QVBoxLayout>
#include <QPixmap>
#include <QStyle>

DraggableItem::DraggableItem(QWidget *parent)
    : QWidget(parent), pere(parent) {

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
    dplay = new QPushButton(nwidget);
    dplay->setText("");
    QIcon playIcon;
    playIcon.addPixmap(QPixmap(":/16x16/icons/16x16/cil-media-play.png"), QIcon::Normal, QIcon::Off);
    dplay->setIcon(playIcon);
    horizontalLayout->addWidget(dplay);

    // Add another spacer item
    QSpacerItem *spacerItem2u = new QSpacerItem(10, 19, QSizePolicy::Fixed, QSizePolicy::Minimum);
    horizontalLayout->addItem(spacerItem2u);

    // Add label for image
    label_3 = new QLabel(nwidget);
    label_3->setMinimumSize(QSize(60, 7));
    label_3->setMaximumSize(QSize(75, 75));
    label_3->setText("");
    label_3->setScaledContents(true);
    label_3->setObjectName("label_3");
    horizontalLayout->addWidget(label_3);

    // Add spacer item
    QSpacerItem *spacerItem3 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
    horizontalLayout->addItem(spacerItem3);

    // Add label for text
    label = new QLabel(nwidget);
    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    font.setWeight(QFont::Weight(75));
    label->setFont(font);
    label->setScaledContents(false);
    label->setObjectName("label");
    horizontalLayout->addWidget(label);

    // Add expanding spacer item
    QSpacerItem *spacerItem4 = new QSpacerItem(100, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout->addItem(spacerItem4);

    // Add delete button
    deletetkt = new QPushButton(nwidget);
    deletetkt->setText("");
    QIcon deleteIcon;
    deleteIcon.addPixmap(QPixmap(":/20x20/icons/20x20/cil-x-circle.png"), QIcon::Normal, QIcon::Off);
    deletetkt->setIcon(deleteIcon);
    horizontalLayout->addWidget(deletetkt);

    // Add final spacer item
    QSpacerItem *spacerItem2ud = new QSpacerItem(20, 19, QSizePolicy::Fixed, QSizePolicy::Minimum);
    horizontalLayout->addItem(spacerItem2ud);

    // Main layout
    QHBoxLayout *allQHBoxLayout = new QHBoxLayout();
    allQHBoxLayout->addWidget(nwidget, 0);
    setLayout(allQHBoxLayout);
}

void DraggableItem::setTexte(const QString &text1, const QString &text2) {
    int maxcara = 60;
    QString a = QString("    %1 - By %2").arg(text1).arg(text2);
    if (a.length() > maxcara) {
        int tm = maxcara - (a.length() - text1.length());
        QString shortText = text1.left(tm) + "...";
        label->setText(QString("    %1 - By %2").arg(shortText).arg(text2));
    } else {
        label->setText(QString("    %1 - By %2").arg(text1).arg(text2));
    }
}

void DraggableItem::setIcone(const QString &imagePath) {
    label_3->setStyleSheet(QString("border-radius: 2px;"
                                   "border-image: url('%1') 0 0 0 0 stretch stretch").arg(imagePath));
}

void DraggableItem::setId(yt_music* id) {
    this->id = id;
}
