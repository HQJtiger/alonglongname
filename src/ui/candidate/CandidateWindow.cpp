#include "CandidateWindow.h"
#include "core/common/Logger.h"

#include <QMouseEvent>
#include <QApplication>
#include <QScreen>
#include <QScrollBar>

namespace uim {

CandidateWindow::CandidateWindow(QWidget* parent)
    : QWidget(parent)
    , m_pinyinLabel(nullptr)
    , m_candidateListView(nullptr)
    , m_pageLabel(nullptr)
    , m_listModel(nullptr)
    , m_pinyinEngine(nullptr)
    , m_currentTheme(ThemeType::Light)
    , m_fontSize(14)
    , m_candidateCount(9)
    , m_isDragging(false)
{
    setupUi();
    applyStyle();
}

CandidateWindow::~CandidateWindow() = default;

void CandidateWindow::setPinyinEngine(PinyinEngine* engine)
{
    if (m_pinyinEngine) {
        disconnect(m_pinyinEngine, &PinyinEngine::candidatesChanged,
                   this, &CandidateWindow::onCandidatesChanged);
        disconnect(m_pinyinEngine, &PinyinEngine::compositionChanged,
                   this, &CandidateWindow::onCompositionChanged);
    }

    m_pinyinEngine = engine;

    if (m_pinyinEngine) {
        connect(m_pinyinEngine, &PinyinEngine::candidatesChanged,
                this, &CandidateWindow::onCandidatesChanged);
        connect(m_pinyinEngine, &PinyinEngine::compositionChanged,
                this, &CandidateWindow::onCompositionChanged);
    }
}

void CandidateWindow::updateCandidates()
{
    if (!m_pinyinEngine) {
        return;
    }

    CandidateList candidates = m_pinyinEngine->candidates();
    m_listModel->setCandidates(candidates);

    updatePageInfo();

    // 调整窗口大小
    if (!candidates.isEmpty()) {
        int height = 30 + candidates.size() * 28;  // 拼音行 + 候选词行
        setMinimumHeight(height);
        resize(width(), height);
    }
}

void CandidateWindow::updatePinyin()
{
    if (!m_pinyinEngine) {
        return;
    }

    QString pinyin = m_pinyinEngine->compositionText();
    m_pinyinLabel->setText(pinyin);
    m_pinyinLabel->setVisible(!pinyin.isEmpty());
}

void CandidateWindow::showAt(int x, int y)
{
    // 确保窗口不会超出屏幕
    QScreen* screen = QApplication::screenAt(QPoint(x, y));
    if (screen) {
        QRect screenGeometry = screen->availableGeometry();
        int windowWidth = width();
        int windowHeight = height();

        if (x + windowWidth > screenGeometry.right()) {
            x = screenGeometry.right() - windowWidth;
        }
        if (y + windowHeight > screenGeometry.bottom()) {
            y = y - windowHeight - 10;  // 显示在光标上方
        }

        move(x, y);
    } else {
        move(x, y);
    }

    show();
    raise();
    activateWindow();
}

void CandidateWindow::hideWindow()
{
    hide();
}

void CandidateWindow::setTheme(ThemeType theme)
{
    m_currentTheme = theme;
    applyStyle();
}

void CandidateWindow::setFontSize(int size)
{
    m_fontSize = size;
    applyStyle();
}

void CandidateWindow::setCandidateCount(int count)
{
    m_candidateCount = count;
}

void CandidateWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void CandidateWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (m_isDragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

void CandidateWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        event->accept();
    }
}

void CandidateWindow::onItemClicked(const QModelIndex& index)
{
    if (index.isValid()) {
        emit candidateSelected(index.row());
    }
}

void CandidateWindow::onCandidatesChanged()
{
    updateCandidates();
}

void CandidateWindow::onCompositionChanged()
{
    updatePinyin();
}

void CandidateWindow::setupUi()
{
    // 窗口属性
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);

    // 主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 8, 10, 8);
    mainLayout->setSpacing(4);

    // 拼音显示
    m_pinyinLabel = new QLabel(this);
    m_pinyinLabel->setObjectName("pinyinLabel");
    m_pinyinLabel->setVisible(false);
    mainLayout->addWidget(m_pinyinLabel);

    // 候选词列表
    m_listModel = new CandidateListModel(this);

    m_candidateListView = new QListView(this);
    m_candidateListView->setObjectName("candidateListView");
    m_candidateListView->setModel(m_listModel);
    m_candidateListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_candidateListView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_candidateListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_candidateListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_candidateListView->setUniformItemSizes(true);

    connect(m_candidateListView, &QListView::clicked,
            this, &CandidateWindow::onItemClicked);

    mainLayout->addWidget(m_candidateListView);

    // 页码显示
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();

    m_pageLabel = new QLabel("1/1", this);
    m_pageLabel->setObjectName("pageLabel");
    m_pageLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    bottomLayout->addWidget(m_pageLabel);

    mainLayout->addLayout(bottomLayout);

    // 设置初始大小
    setFixedWidth(280);
    setMinimumHeight(100);
}

void CandidateWindow::applyStyle()
{
    QString bgColor, borderColor, textColor, pinyinColor, pageColor;
    QString selectedBg, selectedText;

    if (m_currentTheme == ThemeType::Dark) {
        bgColor = "rgba(40, 40, 40, 240)";
        borderColor = "rgba(80, 80, 80, 200)";
        textColor = "#e0e0e0";
        pinyinColor = "#90caf9";
        pageColor = "#888888";
        selectedBg = "#1976d2";
        selectedText = "#ffffff";
    } else {
        bgColor = "rgba(255, 255, 255, 240)";
        borderColor = "rgba(200, 200, 200, 200)";
        textColor = "#333333";
        pinyinColor = "#1976d2";
        pageColor = "#999999";
        selectedBg = "#e3f2fd";
        selectedText = "#1976d2";
    }

    QString styleSheet = QString(
        "CandidateWindow {"
        "    background-color: %1;"
        "    border: 1px solid %2;"
        "    border-radius: 8px;"
        "}"
        "#pinyinLabel {"
        "    color: %3;"
        "    font-size: %4px;"
        "    padding: 2px 4px;"
        "    font-family: 'Consolas', 'Monaco', monospace;"
        "}"
        "#candidateListView {"
        "    background: transparent;"
        "    border: none;"
        "    outline: none;"
        "    font-size: %4px;"
        "}"
        "#candidateListView::item {"
        "    padding: 4px 8px;"
        "    border-radius: 4px;"
        "    color: %5;"
        "}"
        "#candidateListView::item:selected {"
        "    background-color: %6;"
        "    color: %7;"
        "}"
        "#candidateListView::item:hover {"
        "    background-color: %6;"
        "}"
        "#pageLabel {"
        "    color: %8;"
        "    font-size: 11px;"
        "    padding: 2px 4px;"
        "}"
    ).arg(bgColor, borderColor, pinyinColor,
         QString::number(m_fontSize),
         textColor, selectedBg, selectedText, pageColor);

    setStyleSheet(styleSheet);
}

void CandidateWindow::updatePageInfo()
{
    if (!m_pinyinEngine) {
        m_pageLabel->setText("1/1");
        return;
    }

    int currentPage = m_pinyinEngine->currentPage() + 1;
    int totalPages = m_pinyinEngine->totalPages();
    if (totalPages == 0) {
        totalPages = 1;
    }

    m_pageLabel->setText(QString("%1/%2").arg(currentPage).arg(totalPages));
}

} // namespace uim
