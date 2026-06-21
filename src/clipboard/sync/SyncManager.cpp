#include "SyncManager.h"
#include "core/common/Logger.h"
#include "config/ConfigManager.h"

#include <QUuid>
#include <QJsonDocument>
#include <QJsonObject>

namespace uim {

SyncManager::SyncManager(QObject* parent)
    : QObject(parent)
    , m_status(SyncStatus::Disconnected)
    , m_enabled(false)
    , m_deviceId()
    , m_deviceName()
    , m_encryptionKey()
{
}

SyncManager::~SyncManager()
{
    shutdown();
}

bool SyncManager::initialize()
{
    UIM_LOG_INFO("Initializing sync manager...");

    // 生成本地设备 ID
    m_deviceId = generateDeviceId();

    // 获取设备名称
    m_deviceName = ConfigManager::instance()
        .value("sync/deviceName", QString()).toString();
    if (m_deviceName.isEmpty()) {
        m_deviceName = "Unnamed Device";
    }

    // 加载已配对设备
    // TODO: 从配置文件加载

    UIM_LOG_INFO(QString("Sync manager initialized, device ID: %1")
                 .arg(m_deviceId));
    return true;
}

void SyncManager::shutdown()
{
    if (!m_enabled) {
        return;
    }

    UIM_LOG_INFO("Shutting down sync manager...");

    stopSync();

    m_enabled = false;
    UIM_LOG_INFO("Sync manager shut down");
}

bool SyncManager::startSync()
{
    if (m_enabled) {
        return true;
    }

    UIM_LOG_INFO("Starting sync...");

    // 检查是否启用了同步
    bool enableSync = ConfigManager::instance()
        .value("clipboard/enableSync", false).toBool();
    if (!enableSync) {
        UIM_LOG_INFO("Sync is disabled in config");
        return false;
    }

    m_status = SyncStatus::Connecting;
    emit statusChanged(m_status);

    // 启动局域网发现
    if (!startDiscovery()) {
        UIM_LOG_WARN("Failed to start device discovery");
    }

    m_enabled = true;
    m_status = SyncStatus::Connected;
    emit statusChanged(m_status);

    UIM_LOG_INFO("Sync started");
    return true;
}

void SyncManager::stopSync()
{
    if (!m_enabled) {
        return;
    }

    UIM_LOG_INFO("Stopping sync...");

    stopDiscovery();

    // 断开所有连接
    m_connectedDevices.clear();

    m_enabled = false;
    m_status = SyncStatus::Disconnected;
    emit statusChanged(m_status);

    UIM_LOG_INFO("Sync stopped");
}

SyncStatus SyncManager::status() const
{
    return m_status;
}

QVector<DeviceInfo> SyncManager::connectedDevices() const
{
    return m_connectedDevices;
}

void SyncManager::sendClipboardItem(const ClipboardItem& item)
{
    if (!m_enabled || m_connectedDevices.isEmpty()) {
        return;
    }

    UIM_LOG_DEBUG(QString("Sending clipboard item to %1 devices")
                  .arg(m_connectedDevices.size()));

    // 序列化为 JSON
    QJsonObject obj;
    obj["id"] = item.id;
    obj["type"] = static_cast<int>(item.type);
    obj["content"] = item.content;
    obj["timestamp"] = item.timestamp.toString(Qt::ISODate);
    obj["deviceId"] = m_deviceId;
    obj["preview"] = item.preview;

    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    // 加密
    QByteArray encryptedData = encryptData(data);

    // 发送给所有已连接设备
    // TODO: 实际发送逻辑

    Q_UNUSED(encryptedData);
}

bool SyncManager::pairDevice(const QString& deviceId)
{
    UIM_LOG_INFO(QString("Pairing device: %1").arg(deviceId));

    // 检查设备是否已连接
    for (const auto& device : m_connectedDevices) {
        if (device.deviceId == deviceId) {
            m_pairedDevices.append(device);
            // TODO: 保存到配置
            return true;
        }
    }

    UIM_LOG_WARN(QString("Device not found: %1").arg(deviceId));
    return false;
}

void SyncManager::unpairDevice(const QString& deviceId)
{
    UIM_LOG_INFO(QString("Unpairing device: %1").arg(deviceId));

    for (int i = 0; i < m_pairedDevices.size(); ++i) {
        if (m_pairedDevices[i].deviceId == deviceId) {
            m_pairedDevices.removeAt(i);
            // TODO: 从配置中删除
            break;
        }
    }
}

QVector<DeviceInfo> SyncManager::pairedDevices() const
{
    return m_pairedDevices;
}

void SyncManager::setEncryptionKey(const QString& key)
{
    m_encryptionKey = key;
    // TODO: 保存到配置
}

void SyncManager::onDataReceived(const QByteArray& data)
{
    // 解密
    QByteArray decryptedData = decryptData(data);

    // 解析 JSON
    QJsonDocument doc = QJsonDocument::fromJson(decryptedData);
    if (!doc.isObject()) {
        return;
    }

    QJsonObject obj = doc.object();

    ClipboardItem item;
    item.id = obj["id"].toString();
    item.type = static_cast<ClipboardContentType>(obj["type"].toInt(0));
    item.content = obj["content"].toString();
    item.timestamp = QDateTime::fromString(obj["timestamp"].toString(), Qt::ISODate);
    item.deviceId = obj["deviceId"].toString();
    item.preview = obj["preview"].toString();

    emit remoteClipboardReceived(item);
}

void SyncManager::onDeviceDiscovered(const DeviceInfo& device)
{
    UIM_LOG_DEBUG(QString("Device discovered: %1 (%2)")
                  .arg(device.deviceName).arg(device.ipAddress));

    // 检查是否已配对
    bool isPaired = false;
    for (const auto& paired : m_pairedDevices) {
        if (paired.deviceId == device.deviceId) {
            isPaired = true;
            break;
        }
    }

    if (isPaired) {
        // 自动连接已配对的设备
        // TODO: 建立连接
        m_connectedDevices.append(device);
        emit deviceConnected(device);
    }
}

bool SyncManager::startDiscovery()
{
    // MVP 阶段：只搭框架
    // TODO: 实现 UDP 广播发现

    UIM_LOG_INFO("Device discovery started (framework only)");
    return true;
}

void SyncManager::stopDiscovery()
{
    // TODO: 停止 UDP 广播发现

    UIM_LOG_INFO("Device discovery stopped");
}

QByteArray SyncManager::encryptData(const QByteArray& data) const
{
    // MVP 阶段：不加密，直接返回
    // TODO: 使用 AES-256-GCM 加密

    return data;
}

QByteArray SyncManager::decryptData(const QByteArray& data) const
{
    // MVP 阶段：不解密，直接返回
    // TODO: 使用 AES-256-GCM 解密

    return data;
}

QString SyncManager::generateDeviceId() const
{
    // 尝试从配置读取
    QString savedId = ConfigManager::instance()
        .value("sync/deviceId", QString()).toString();

    if (!savedId.isEmpty()) {
        return savedId;
    }

    // 生成新的
    QString newId = QUuid::createUuid().toString(QUuid::WithoutBraces);

    // 保存到配置
    ConfigManager::instance().setValue("sync/deviceId", newId);
    ConfigManager::instance().save();

    return newId;
}

} // namespace uim
