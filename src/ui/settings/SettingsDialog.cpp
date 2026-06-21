#include "SettingsDialog.h"
#include "GeneralSettingsPage.h"
#include "AppearanceSettingsPage.h"
#include "core/common/Logger.h"

#include <QListWidgetItem>

namespace uim {

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent)
    , m_navigationList(nullptr)
    , m_contentStack(nullptr)
    , m_generalPage(nullptr)
    , m_appearancePage(nullptr)
    , m_buttonBox(nullptr)
{
    setupUi();
    createPages();
    loadSettings();
}

SettingsDialog::~SettingsDialog() = default;

void SettingsDialog::loadSettings()
{
    if (m_generalPage) {
        m_generalPage->loadSettings();
    }
    if (m_appearancePage) {
        m_appearancePage->loadSettings();
    }
}

void SettingsDialog::saveSettings()
{
    if (m_generalPage) {
        m_generalPage->saveSettings();
    }
    if (m_appearancePage) {
        m_appearancePage->saveSettings();
    }

    emit settingsChanged();
    UIM_LOG_INFO("Settings saved");
}

void SettingsDialog::onPageChanged(int index)
{
    if (index >= 0 && index < m_contentStack->count()) {
        m_contentStack->setCurrentIndex(index);
    }
}

void SettingsDialog::onAccepted()
{
    saveSettings();
    accept();
}

void SettingsDialog::onApplied()
{
    saveSettings();
}

void SettingsDialog::setupUi()
{
    setWindowTitle(tr("统一输入法设置"));
    setMinimumSize(700, 500);
    resize(750, 550);

    // 主布局
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 左侧导航
    m_navigationList = new QListWidget(this);
    m_navigationList->setObjectName("navigationList");
    m_navigationList->setFixedWidth(160);
    m_navigationList->setViewMode(QListView::ListMode);
    m_navigationList->setMovement(QListView::Static);
    m_navigationList->setSpacing(2);
    m_navigationList->setIconSize(QSize(24, 24));

    connect(m_navigationList, &QListWidget::currentRowChanged,
            this, &SettingsDialog::onPageChanged);

    mainLayout->addWidget(m_navigationList);

    // 右侧内容区
    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->setContentsMargins(16, 16, 16, 16);
    rightLayout->setSpacing(12);

    m_contentStack = new QStackedWidget(this);
    rightLayout->addWidget(m_contentStack, 1);

    // 按钮
    m_buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel,
        Qt::Horizontal, this);

    connect(m_buttonBox, &QDialogButtonBox::accepted,
            this, &SettingsDialog::onAccepted);
    connect(m_buttonBox, &QDialogButtonBox::rejected,
            this, &QDialog::reject);
    connect(m_buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, &SettingsDialog::onApplied);

    rightLayout->addWidget(m_buttonBox);

    mainLayout->addLayout(rightLayout, 1);

    // 样式
    setStyleSheet(
        "#navigationList {"
        "    background-color: #f5f5f5;"
        "    border: none;"
        "    border-right: 1px solid #e0e0e0;"
        "    padding: 8px 0;"
        "}"
        "#navigationList::item {"
        "    padding: 10px 16px;"
        "    margin: 2px 8px;"
        "    border-radius: 6px;"
        "    color: #333333;"
        "}"
        "#navigationList::item:selected {"
        "    background-color: #e3f2fd;"
        "    color: #1976d2;"
        "    font-weight: bold;"
        "}"
        "#navigationList::item:hover {"
        "    background-color: #eeeeee;"
        "}"
    );
}

void SettingsDialog::createPages()
{
    // 通用设置
    QListWidgetItem* generalItem = new QListWidgetItem(tr("通用设置"), m_navigationList);
    generalItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_generalPage = new GeneralSettingsPage(this);
    m_contentStack->addWidget(m_generalPage);

    // 外观设置
    QListWidgetItem* appearanceItem = new QListWidgetItem(tr("外观设置"), m_navigationList);
    appearanceItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_appearancePage = new AppearanceSettingsPage(this);
    m_contentStack->addWidget(m_appearancePage);

    // 默认选中第一页
    m_navigationList->setCurrentRow(0);
}

} // namespace uim
