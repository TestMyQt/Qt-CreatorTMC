#include "exercisedelegate.h"
#include "exercise.h"

#include <QDebug>
#include <QLabel>
#include <QStyle>
#include <QApplication>

ExerciseDelegate::ExerciseDelegate(QWidget *parent) : QStyledItemDelegate(parent)
{

}

void ExerciseDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() != 1) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    int progress = index.data().toInt();

    if (progress == 0) {
        QStyleOptionButton opt;
        opt.state |= QStyle::State_Active;
        opt.text =  "Download";
        opt.rect = option.rect;
        QApplication::style()->drawControl(QStyle::CE_PushButton, &opt, painter);
        return;
    }

    if (progress == -1) {
        QStyleOptionButton opt;
        opt.state |= QStyle::State_Active;
        opt.text =  "Open";
        opt.rect = option.rect;
        QApplication::style()->drawControl(QStyle::CE_PushButton, &opt, painter);
        return;
    }


    QStyleOptionProgressBar progressBarOption;
    progressBarOption.rect = option.rect;
    progressBarOption.minimum = 0;
    progressBarOption.maximum = 100;
    progressBarOption.progress = progress;
    progressBarOption.text = QString::number(progress) + "%";
    progressBarOption.textVisible = true;

    QApplication::style()->drawControl(QStyle::CE_ProgressBar,
                                       &progressBarOption, painter);
}
