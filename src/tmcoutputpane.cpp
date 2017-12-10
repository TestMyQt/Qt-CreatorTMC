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

using ProjectExplorer::SessionManager;

TmcOutputPane::TmcOutputPane(QObject *parent) :
    Core::IOutputPane(parent),
    m_context(new Core::IContext(this)),
    m_outputWidget(nullptr),
    m_runTMC(nullptr)
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

    m_listView = new Utils::ListView;
    m_model = new TmcResultModel(m_listView);
    m_listView->setModel(m_model);
    m_listView->setSelectionMode(QAbstractItemView::SingleSelection);

    pal = m_listView->palette();
    pal.setColor(QPalette::Base, pal.window().color());
    m_listView->setPalette(pal);
    m_listView->setVisible(true);

    m_outputWidget->addWidget(m_listView);

    createToolButtons();

    connect(TMCRunner::instance(), &TMCRunner::testResultReady,
            this, &TmcOutputPane::addTestResults);
}

void TmcOutputPane::createToolButtons()
{
    m_runTMC = new QToolButton(m_listView);
    m_runTMC->setIcon(Utils::Icons::RUN_SMALL_TOOLBAR.icon());
    m_runTMC->setToolTip(tr("Run TMC Tests"));
    m_runTMC->setEnabled(true);
    connect(m_runTMC, &QToolButton::clicked, this, &TmcOutputPane::onTMCTriggered);
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

void TmcOutputPane::onTMCTriggered()
{
    TMCRunner *runner = TMCRunner::instance();
    Project *project = SessionManager::startupProject();
    runner->testProject(project);
}

void TmcOutputPane:: addTestResults(const QList<TmcTestResult> &results) {
    foreach (TmcTestResult r, results) {
        m_model->addResult(r);
    }

    // Focus on the results pane
    popup(IOutputPane::Flag(WithFocus));
    flash();
    navigateStateChanged();
}

void TmcOutputPane::clearContents()
{
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
    return { m_runTMC };
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
    return false;
}

bool TmcOutputPane::canPrevious() const
{
    return false;
}

void TmcOutputPane::goToNext()
{
}

void TmcOutputPane::goToPrev()
{
}

