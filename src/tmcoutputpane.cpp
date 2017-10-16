#include "tmcoutputpane.h"
#include "tmcrunner.h"
#include "tmcresultmodel.h"

#include <aggregation/aggregate.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/find/basetextfind.h>
#include <coreplugin/find/itemviewfind.h>
#include <coreplugin/icontext.h>
#include <coreplugin/icore.h>
#include <projectexplorer/buildmanager.h>
#include <projectexplorer/projectexplorer.h>
#include <texteditor/texteditor.h>
#include <utils/theme/theme.h>
#include <utils/utilsicons.h>
#include <utils/treemodel.h>

#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMenu>
#include <QMessageBox>
#include <QScrollBar>
#include <QStackedWidget>
#include <QToolButton>
#include <QVBoxLayout>

TmcOutputPane::TmcOutputPane(QObject *parent) :
    Core::IOutputPane(parent),
    m_context(new Core::IContext(this))
{
    m_outputWidget = new QStackedWidget;
    QVBoxLayout *outputLayout = new QVBoxLayout;
    outputLayout->setMargin(0);
    outputLayout->setSpacing(0);

    QPalette pal;
    pal.setColor(QPalette::Window,
                 Utils::creatorTheme()->color(Utils::Theme::InfoBarBackground));
    pal.setColor(QPalette::WindowText,
                 Utils::creatorTheme()->color(Utils::Theme::InfoBarText));

    createToolButtons();

    m_model = new TmcResultModel(this);
    m_listView = new Utils::ListView;
    m_listView->setModel(m_model);
    m_listView->setSelectionMode(QAbstractItemView::SingleSelection);

    pal = m_listView->palette();
    pal.setColor(QPalette::Base, pal.window().color());
    m_listView->setPalette(pal);

    outputLayout->addWidget(m_listView);

    connect(TMCRunner::instance(), &TMCRunner::testResultReady,
            this, &TmcOutputPane::addTestResults);
}

void TmcOutputPane::createToolButtons()
{
    m_runAll = new QToolButton(m_listView);
    m_runAll->setIcon(Utils::Icons::RUN_SMALL_TOOLBAR.icon());
    m_runAll->setToolTip(tr("Run TMC Tests"));
    m_runAll->setEnabled(false);
    connect(m_runAll, &QToolButton::clicked, this, &TmcOutputPane::onRunAllTriggered);
}

static TmcOutputPane *s_instance = nullptr;

TmcOutputPane *TmcOutputPane::instance()
{
    if (!s_instance)
        s_instance = new TmcOutputPane;
    return s_instance;
}

TmcOutputPane::~TmcOutputPane()
{
    delete m_listView;
    if (!m_outputWidget->parent())
        delete m_outputWidget;
    s_instance = nullptr;
}

void TmcOutputPane::onRunAllTriggered()
{
    TMCRunner *runner = TMCRunner::instance();
    runner->runOnActiveProject();
}

void TmcOutputPane:: addTestResults(const QList<TmcTestResult> &results) {
    foreach (TmcTestResult r, results) {
        qDebug() << r.name() << r.isSuccessful() << r.message();
        m_model->addResult(r);
    }

    flash();
    navigateStateChanged();
}

void TmcOutputPane::clearContents()
{
    qDebug() << "contents cleared";
    m_model->clearResults();
}

QWidget *TmcOutputPane::outputWidget(QWidget *parent)
{
    if (m_outputWidget) {
        m_outputWidget->setParent(parent);
    } else {
        qDebug() << "This should not happen...";
    }
    return m_outputWidget;
}

QList<QWidget *> TmcOutputPane::toolBarWidgets() const
{
    return { m_runAll };
}

QString TmcOutputPane::displayName() const
{
    return tr("TMC Results");
}

int TmcOutputPane::priorityInStatusBar() const
{
    return -666;
}

void TmcOutputPane::visibilityChanged(bool visible)
{
    if (visible == m_wasVisibleBefore)
        return;
    m_wasVisibleBefore = visible;
}

void TmcOutputPane::setFocus()
{
}

bool TmcOutputPane::hasFocus() const
{
    return m_listView->hasFocus();
}

bool TmcOutputPane::canFocus() const
{
    return true;
}

bool TmcOutputPane::canNavigate() const
{
    return false;
}

bool TmcOutputPane::canNext() const
{
    return true;
}

bool TmcOutputPane::canPrevious() const
{
    return true;
}

void TmcOutputPane::goToNext()
{
}

void TmcOutputPane::goToPrev()
{
}

void TmcOutputPane::updateSummaryLabel()
{
    m_summaryLabel->setText(tr("tmc test results"));
}
