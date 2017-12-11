#ifndef SUBMITWIDGET_H
#define SUBMITWIDGET_H

#include <QWidget>
#include <QPushButton>
#include "ui_submit.h"
#include "exercise.h"

class SubmitWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SubmitWidget(QWidget *parent = nullptr);

signals:

public slots:
    void onSubmitReply(Exercise ex);
    void submitProgress(Exercise ex, qint64 bytesSent, qint64 bytesTotal);
    void updateStatus(Submission submission);

private:
    Ui::Submit *m_submitWindow;

    QLabel *m_status;
    QProgressBar *m_progressBar;
    QPushButton *m_cancel;

};

#endif // SUBMITWIDGET_H
