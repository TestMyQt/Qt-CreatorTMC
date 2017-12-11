#include "submitwidget.h"

SubmitWidget::SubmitWidget(QWidget *parent) : QWidget(parent)
{
    m_submitWindow = new Ui::Submit;
    m_submitWindow->setupUi(this);

    m_status = m_submitWindow->status;
    m_progressBar = m_submitWindow->progressBar;
    m_progressBar->setStyleSheet("QProgressBar { border: 1px solid grey; border-radius: 0px; text-align: center; }"
                                 "QProgressBar::chunk { background-color: #bcbcbc; width: 2px; }");
    m_cancel = m_submitWindow->cancel->button(QDialogButtonBox::Cancel);

    connect(m_cancel, &QPushButton::clicked, this, &QWidget::close);
}

void SubmitWidget::onSubmitReply(Exercise ex)
{
    this->setWindowTitle("Submitting " + ex.getName());
    m_status->setText("Submitting " + ex.getName());
}

void SubmitWidget::submitProgress(Exercise ex, qint64 bytesSent, qint64 bytesTotal)
{
    this->setWindowTitle("Submitting " + ex.getName());
    m_status->setText("Submitting " + ex.getName());
    m_progressBar->setMaximum(bytesTotal);
    m_progressBar->setValue(bytesSent);
}

void SubmitWidget::updateStatus(Submission submission)
{
    QString output;
    switch (submission.getStatus()) {
    case (Submission::Error):
        m_status->setText("Submission processing failed:\n"
                          + submission.errorString());
        m_progressBar->setMaximum(1);
        m_progressBar->setValue(1);
        break;
    case (Submission::Processing):
        m_status->setText("Processing...");
        m_progressBar->setMaximum(0);
        m_progressBar->setMinimum(0);
        break;
    case (Submission::Fail):
        foreach (QString testCase, submission.getTestCases()) {
            output.append(testCase);
            output.append(" ");
        }
        m_status->setText("Tests did not pass on server:\n"
                          "Testcases: " + output    );
        break;
    case (Submission::Ok):
        foreach (QString point, submission.getPoints()) {
            output.append(point);
            output.append(" ");
        }
        m_status->setText("All tests passed!"
                          "\n"
                          "Points awarded: " + output);
        break;
    case (Submission::Hidden):
        // ????
        break;
    }
}
