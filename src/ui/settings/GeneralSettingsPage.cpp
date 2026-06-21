#include "GeneralSettingsPage.h"
#include "config/ConfigManager.h"

namespace uim {

GeneralSettingsPage::GeneralSettingsPage(QWidget* parent)
    : QWidget(parent)
    , m_enableChinese(nullptr)
    , m_enableJianpin(nullptr)
    , m_enableAutoSpace(nullptr)
    , m_candidateCount(nullptr)
    , m_toggleKey(nullptr)
    , m_enableClipboardSync(nullptr)
    , m_enableClipboardHistory(nullptr)
    , m_clipboardHistorySize(nullptr)
    , m_autoStart(nullptr)
    , m_minimizeToTray(nullptr)
{
    setupUi();
}

GeneralSettingsPage::~GeneralSettingsPage() = default;

void GeneralSettingsPage::loadSettings()
{
    ConfigManager& config = ConfigManager::instance();

    m_enableChinese->setChecked(config.value("input/defaultChinese", true).toBool());
    m_enableJianpin->setChecked(config.value("input/enableJianpin", true).toBool());
    m_enableAutoSpace->setChecked(config.value("input/autoSpace", false).toBool());
    m_candidateCount->setValue(config.value("input/candidateCount", 9).toInt());

    int toggleIndex = config.value("input/toggleKey", 0).toInt();
    m_toggleKey->setCurrentIndex(toggleIndex);

    m_enableClipboardSync->setChecked(config.value("clipboard/enableSync", false).toBool());
    m_enableClipboardHistory->setChecked(config.value("clipboard/enableHistory", true).toBool());
    m_clipboardHistorySize->setValue(config.value("clipboard/historySize", 100).toInt());

    m_autoStart->setChecked(config.value("general/autoStart", false).toBool());
    m_minimizeToTray->setChecked(config.value("general/minimizeToTray", true).toBool());
}

void GeneralSettingsPage::saveSettings()
{
    ConfigManager& config = ConfigManager::instance();

    config.setValue("input/defaultChinese", m_enableChinese->isChecked());
    config.setValue("input/enableJianpin", m_enableJianpin->isChecked());
    config.setValue("input/autoSpace", m_enableAutoSpace->isChecked());
    config.setValue("input/candidateCount", m_candidateCount->value());

    config.setValue("input/toggleKey", m_toggleKey->currentIndex());

    config.setValue("clipboard/enableSync", m_enableClipboardSync->isChecked());
    config.setValue("clipboard/enableHistory", m_enableClipboardHistory->isChecked());
    config.setValue("clipboard/historySize", m_clipboardHistorySize->value());

    config.setValue("general/autoStart", m_autoStart->isChecked());
    config.setValue("general/minimizeToTray", m_minimizeToTray->isChecked());

    config.save();
}

void GeneralSettingsPage::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);

    // 输入设置
    QGroupBox* inputGroup = new QGroupBox(tr("输入设置"), this);
    QFormLayout* inputLayout = new QFormLayout(inputGroup);
    inputLayout->setSpacing(10);

    m_enableChinese = new QCheckBox(tr("默认使用中文输入"), this);
    inputLayout->addRow(m_enableChinese);

    m_enableJianpin = new QCheckBox(tr("启用简拼输入"), this);
    inputLayout->addRow(m_enableJianpin);

    m_enableAutoSpace = new QCheckBox(tr("选词后自动插入空格"), this);
    inputLayout->addRow(m_enableAutoSpace);

    m_candidateCount = new QSpinBox(this);
    m_candidateCount->setRange(3, 9);
    m_candidateCount->setSuffix(tr(" 个"));
    inputLayout->addRow(tr("每页候选词数："), m_candidateCount);

    m_toggleKey = new QComboBox(this);
    m_toggleKey->addItem(tr("Shift 键"));
    m_toggleKey->addItem(tr("Ctrl 键"));
    m_toggleKey->addItem(tr("Caps Lock 键"));
    inputLayout->addRow(tr("中英文切换键："), m_toggleKey);

    mainLayout->addWidget(inputGroup);

    // 剪贴板设置
    QGroupBox* clipboardGroup = new QGroupBox(tr("剪贴板设置"), this);
    QFormLayout* clipboardLayout = new QFormLayout(clipboardGroup);
    clipboardLayout->setSpacing(10);

    m_enableClipboardHistory = new QCheckBox(tr("启用剪贴板历史记录"), this);
    clipboardLayout->addRow(m_enableClipboardHistory);

    m_clipboardHistorySize = new QSpinBox(this);
    m_clipboardHistorySize->setRange(10, 1000);
    m_clipboardHistorySize->setSingleStep(10);
    m_clipboardHistorySize->setSuffix(tr(" 条"));
    clipboardLayout->addRow(tr("历史记录条数："), m_clipboardHistorySize);

    m_enableClipboardSync = new QCheckBox(tr("启用剪贴板同步（需配对设备）"), this);
    clipboardLayout->addRow(m_enableClipboardSync);

    mainLayout->addWidget(clipboardGroup);

    // 启动设置
    QGroupBox* generalGroup = new QGroupBox(tr("常规设置"), this);
    QFormLayout* generalLayout = new QFormLayout(generalGroup);
    generalLayout->setSpacing(10);

    m_autoStart = new QCheckBox(tr("开机自动启动"), this);
    generalLayout->addRow(m_autoStart);

    m_minimizeToTray = new QCheckBox(tr("关闭窗口时最小化到托盘"), this);
    generalLayout->addRow(m_minimizeToTray);

    mainLayout->addWidget(generalGroup);

    mainLayout->addStretch();
}

} // namespace uim
