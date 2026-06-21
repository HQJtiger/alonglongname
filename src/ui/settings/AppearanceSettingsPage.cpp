#include "AppearanceSettingsPage.h"
#include "config/ConfigManager.h"

#include <QFontDatabase>

namespace uim {

AppearanceSettingsPage::AppearanceSettingsPage(QWidget* parent)
    : QWidget(parent)
    , m_themeCombo(nullptr)
    , m_followSystem(nullptr)
    , m_fontSize(nullptr)
    , m_fontFamily(nullptr)
    , m_opacitySlider(nullptr)
    , m_windowWidth(nullptr)
    , m_showPinyin(nullptr)
    , m_showIndex(nullptr)
    , m_candidateStyle(nullptr)
    , m_highlightFirst(nullptr)
{
    setupUi();
}

AppearanceSettingsPage::~AppearanceSettingsPage() = default;

void AppearanceSettingsPage::loadSettings()
{
    ConfigManager& config = ConfigManager::instance();

    int themeIndex = config.value("appearance/theme", 0).toInt();
    m_themeCombo->setCurrentIndex(themeIndex);

    m_followSystem->setChecked(config.value("appearance/followSystem", true).toBool());

    m_fontSize->setValue(config.value("appearance/fontSize", 14).toInt());

    QString fontFamily = config.value("appearance/fontFamily", QString()).toString();
    if (!fontFamily.isEmpty()) {
        int idx = m_fontFamily->findText(fontFamily);
        if (idx >= 0) {
            m_fontFamily->setCurrentIndex(idx);
        }
    }

    int opacity = config.value("appearance/opacity", 95).toInt();
    m_opacitySlider->setValue(opacity);

    m_windowWidth->setValue(config.value("appearance/windowWidth", 280).toInt());

    m_showPinyin->setChecked(config.value("appearance/showPinyin", true).toBool());
    m_showIndex->setChecked(config.value("appearance/showIndex", true).toBool());

    int styleIndex = config.value("appearance/candidateStyle", 0).toInt();
    m_candidateStyle->setCurrentIndex(styleIndex);

    m_highlightFirst->setChecked(config.value("appearance/highlightFirst", true).toBool());
}

void AppearanceSettingsPage::saveSettings()
{
    ConfigManager& config = ConfigManager::instance();

    config.setValue("appearance/theme", m_themeCombo->currentIndex());
    config.setValue("appearance/followSystem", m_followSystem->isChecked());
    config.setValue("appearance/fontSize", m_fontSize->value());
    config.setValue("appearance/fontFamily", m_fontFamily->currentText());
    config.setValue("appearance/opacity", m_opacitySlider->value());
    config.setValue("appearance/windowWidth", m_windowWidth->value());
    config.setValue("appearance/showPinyin", m_showPinyin->isChecked());
    config.setValue("appearance/showIndex", m_showIndex->isChecked());
    config.setValue("appearance/candidateStyle", m_candidateStyle->currentIndex());
    config.setValue("appearance/highlightFirst", m_highlightFirst->isChecked());

    config.save();
}

void AppearanceSettingsPage::onThemeChanged(int index)
{
    Q_UNUSED(index);
    // 可以在这里添加预览效果
}

void AppearanceSettingsPage::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);

    // 主题设置
    QGroupBox* themeGroup = new QGroupBox(tr("主题设置"), this);
    QFormLayout* themeLayout = new QFormLayout(themeGroup);
    themeLayout->setSpacing(10);

    m_themeCombo = new QComboBox(this);
    m_themeCombo->addItem(tr("浅色主题"));
    m_themeCombo->addItem(tr("深色主题"));
    m_themeCombo->addItem(tr("自动（跟随系统）"));
    connect(m_themeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AppearanceSettingsPage::onThemeChanged);
    themeLayout->addRow(tr("主题："), m_themeCombo);

    m_followSystem = new QCheckBox(tr("跟随系统主题自动切换"), this);
    themeLayout->addRow(m_followSystem);

    mainLayout->addWidget(themeGroup);

    // 字体设置
    QGroupBox* fontGroup = new QGroupBox(tr("字体设置"), this);
    QFormLayout* fontLayout = new QFormLayout(fontGroup);
    fontLayout->setSpacing(10);

    m_fontFamily = new QComboBox(this);
    m_fontFamily->addItems(QFontDatabase().families());
    m_fontFamily->setEditable(true);
    fontLayout->addRow(tr("字体："), m_fontFamily);

    m_fontSize = new QSpinBox(this);
    m_fontSize->setRange(10, 24);
    m_fontSize->setSuffix(tr(" px"));
    fontLayout->addRow(tr("字号："), m_fontSize);

    mainLayout->addWidget(fontGroup);

    // 窗口设置
    QGroupBox* windowGroup = new QGroupBox(tr("窗口设置"), this);
    QFormLayout* windowLayout = new QFormLayout(windowGroup);
    windowLayout->setSpacing(10);

    m_opacitySlider = new QSlider(Qt::Horizontal, this);
    m_opacitySlider->setRange(50, 100);
    m_opacitySlider->setTickPosition(QSlider::TicksBelow);
    m_opacitySlider->setTickInterval(10);
    windowLayout->addRow(tr("透明度："), m_opacitySlider);

    m_windowWidth = new QSpinBox(this);
    m_windowWidth->setRange(200, 500);
    m_windowWidth->setSuffix(tr(" px"));
    windowLayout->addRow(tr("窗口宽度："), m_windowWidth);

    m_showPinyin = new QCheckBox(tr("显示拼音输入"), this);
    windowLayout->addRow(m_showPinyin);

    m_showIndex = new QCheckBox(tr("显示候选词序号"), this);
    windowLayout->addRow(m_showIndex);

    mainLayout->addWidget(windowGroup);

    // 候选词样式
    QGroupBox* candidateGroup = new QGroupBox(tr("候选词样式"), this);
    QFormLayout* candidateLayout = new QFormLayout(candidateGroup);
    candidateLayout->setSpacing(10);

    m_candidateStyle = new QComboBox(this);
    m_candidateStyle->addItem(tr("横排（默认）"));
    m_candidateStyle->addItem(tr("竖排"));
    m_candidateStyle->addItem(tr("网格"));
    candidateLayout->addRow(tr("排列方式："), m_candidateStyle);

    m_highlightFirst = new QCheckBox(tr("高亮显示第一个候选"), this);
    candidateLayout->addRow(m_highlightFirst);

    mainLayout->addWidget(candidateGroup);

    mainLayout->addStretch();
}

} // namespace uim
