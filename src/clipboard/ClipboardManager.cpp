#include "ClipboardManager.h"
#include "core/common/Logger.h"
#include "config/ConfigManager.h"

#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QImage>
#include <QBuffer>
#include <QUuid>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QStandardPaths>
#include <QDir>

namespace uim {

ClipboardManager::ClipboardManager(QObject* parent)
    : QObject(parent)
    , m_maxHistorySize(100)
    , m_enabled(false)
    , m_ignoreNextChange(false)
{
}

ClipboardManager::~ClipboardManager()
{
    shutdown();
}

bool ClipboardManager::initialize()
{
    UIM_LOG_INFO("Initializing clipboard manager...");

    // 从配置读取最大历史记录数
    m_maxHistorySize = ConfigManager::instance()
        .value("clipboard/historySize", 100).toInt();

    // 加载历史记录
    loadHistory();

    // 监听系统剪贴板
    QClipboard* clipboard = QApplication::clipboard();
    connect(clipboard, &QClipboard::dataChanged,
            this, &ClipboardManager::onClipboardDataChanged);

    m_enabled = true;

    UIM_LOG_INFO(QString("Clipboard manager initialized, max history: %1")
                 .arg(m_maxHistorySize));
    return true;
}

void ClipboardManager::shutdown()
{
    if (!m_enabled) {
        return;
    }

    UIM_LOG_INFO("Shutting down clipboard manager...");

    // 保存历史记录
    saveHistory();

    // 断开剪贴板监听
    QClipboard* clipboard = QApplication::clipboard();
    disconnect(clipboard, &QClipboard::dataChanged,
               this, &ClipboardManager::onClipboardDataChanged);

    m_enabled = false;
    UIM_LOG_INFO("Clipboard manager shut down");
}

QVector<ClipboardItem> ClipboardManager::history() const
{
    return m_history;
}

int ClipboardManager::historyCount() const
{
    return m_history.size();
}

void ClipboardManager::clearHistory()
{
    m_history.clear();
    saveHistory();
    emit historyChanged();
}

void ClipboardManager::removeItem(const QString& id)
{
    for (int i = 0; i < m_history.size(); ++i) {
        if (m_history[i].id == id) {
            m_history.removeAt(i);
            saveHistory();
            emit historyChanged();
            break;
        }
    }
}

void ClipboardManager::setFavorite(const QString& id, bool favorite)
{
    for (auto& item : m_history) {
        if (item.id == id) {
            item.isFavorite = favorite;
            saveHistory();
            emit historyChanged();
            break;
        }
    }
}

void ClipboardManager::copyToClipboard(const ClipboardItem& item)
{
    QClipboard* clipboard = QApplication::clipboard();
    m_ignoreNextChange = true;

    switch (item.type) {
    case ClipboardContentType::Text:
        clipboard->setText(item.content);
        break;

    case ClipboardContentType::Image: {
        QImage image;
        if (image.loadFromData(item.binaryData)) {
            clipboard->setImage(image);
        }
        break;
    }

    case ClipboardContentType::Html: {
        QMimeData* mimeData = new QMimeData();
        mimeData->setHtml(item.content);
        mimeData->setText(item.content);
        clipboard->setMimeData(mimeData);
        break;
    }

    default:
        clipboard->setText(item.content);
        break;
    }
}

QVector<ClipboardItem> ClipboardManager::search(const QString& keyword) const
{
    QVector<ClipboardItem> results;

    if (keyword.isEmpty()) {
        return m_history;
    }

    for (const auto& item : m_history) {
        if (item.content.contains(keyword, Qt::CaseInsensitive) ||
            item.preview.contains(keyword, Qt::CaseInsensitive)) {
            results.append(item);
        }
    }

    return results;
}

bool ClipboardManager::loadHistory()
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString historyFile = dataDir + "/clipboard_history.json";

    QFile file(historyFile);
    if (!file.exists()) {
        return true;  // 没有历史文件是正常的
    }

    if (!file.open(QIODevice::ReadOnly)) {
        UIM_LOG_ERROR(QString("Failed to open clipboard history file: %1")
                      .arg(historyFile));
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isArray()) {
        return false;
    }

    QJsonArray array = doc.array();
    m_history.clear();

    for (const auto& value : array) {
        if (!value.isObject()) {
            continue;
        }

        QJsonObject obj = value.toObject();
        ClipboardItem item;

        item.id = obj["id"].toString();
        item.type = static_cast<ClipboardContentType>(obj["type"].toInt(0));
        item.content = obj["content"].toString();
        item.timestamp = QDateTime::fromString(obj["timestamp"].toString(), Qt::ISODate);
        item.deviceId = obj["deviceId"].toString();
        item.preview = obj["preview"].toString();
        item.isFavorite = obj["isFavorite"].toBool(false);

        // 二进制数据暂不加载（MVP 阶段简化）
        m_history.append(item);
    }

    UIM_LOG_INFO(QString("Loaded %1 clipboard history items")
                 .arg(m_history.size()));
    return true;
}

bool ClipboardManager::saveHistory()
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    QString historyFile = dataDir + "/clipboard_history.json";

    QJsonArray array;

    for (const auto& item : m_history) {
        QJsonObject obj;
        obj["id"] = item.id;
        obj["type"] = static_cast<int>(item.type);
        obj["content"] = item.content;
        obj["timestamp"] = item.timestamp.toString(Qt::ISODate);
        obj["deviceId"] = item.deviceId;
        obj["preview"] = item.preview;
        obj["isFavorite"] = item.isFavorite;
        // 二进制数据暂不保存（MVP 阶段简化）

        array.append(obj);
    }

    QJsonDocument doc(array);

    QFile file(historyFile);
    if (!file.open(QIODevice::WriteOnly)) {
        UIM_LOG_ERROR(QString("Failed to save clipboard history file: %1")
                      .arg(historyFile));
        return false;
    }

    file.write(doc.toJson(QJsonDocument::Compact));

    UIM_LOG_DEBUG(QString("Saved %1 clipboard history items")
                  .arg(m_history.size()));
    return true;
}

void ClipboardManager::onClipboardDataChanged()
{
    if (!m_enabled) {
        return;
    }

    if (m_ignoreNextChange) {
        m_ignoreNextChange = false;
        return;
    }

    ClipboardItem item = readFromClipboard();

    // 忽略空内容
    if (item.content.isEmpty() && item.binaryData.isEmpty()) {
        return;
    }

    // 检查是否与上一条重复
    if (!m_history.isEmpty()) {
        const ClipboardItem& last = m_history.first();
        if (last.content == item.content && last.type == item.type) {
            return;  // 重复内容，不添加
        }
    }

    // 添加到历史记录
    m_history.prepend(item);

    // 限制数量
    limitHistorySize();

    // 保存
    saveHistory();

    emit clipboardChanged(item);
    emit historyChanged();
}

ClipboardItem ClipboardManager::readFromClipboard() const
{
    ClipboardItem item;
    item.id = generateId();
    item.timestamp = QDateTime::currentDateTime();

    QClipboard* clipboard = QApplication::clipboard();
    const QMimeData* mimeData = clipboard->mimeData();

    if (mimeData->hasImage()) {
        item.type = ClipboardContentType::Image;
        QImage image = qvariant_cast<QImage>(mimeData->imageData());
        QByteArray data;
        QBuffer buffer(&data);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, "PNG");
        item.binaryData = data;
        item.preview = "[图片]";
    } else if (mimeData->hasHtml()) {
        item.type = ClipboardContentType::Html;
        item.content = mimeData->html();
        item.preview = mimeData->text().left(100);
    } else if (mimeData->hasText()) {
        item.type = ClipboardContentType::Text;
        item.content = mimeData->text();
        item.preview = item.content.left(100);
    } else {
        item.type = ClipboardContentType::Unknown;
    }

    return item;
}

QString ClipboardManager::generateId() const
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

void ClipboardManager::limitHistorySize()
{
    // 收藏的项目保留，其他的按数量限制
    // MVP 阶段简化：直接限制总数
    while (m_history.size() > m_maxHistorySize) {
        // 从末尾删除非收藏的项目
        bool removed = false;
        for (int i = m_history.size() - 1; i >= 0; --i) {
            if (!m_history[i].isFavorite) {
                m_history.removeAt(i);
                removed = true;
                break;
            }
        }

        if (!removed) {
            break;  // 全是收藏的，不删除了
        }
    }
}

} // namespace uim
