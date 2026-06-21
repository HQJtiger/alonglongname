#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include "../ClipboardTypes.h"

namespace uim {

/**
 * @brief 同步管理器
 *
 * 负责剪贴板内容的跨设备同步。
 * MVP 阶段：只搭框架，实现局域网发现和基础同步
 */
class SyncManager : public QObject {
    Q_OBJECT

public:
    explicit SyncManager(QObject* parent = nullptr);
    ~SyncManager() override;

    /**
     * @brief 初始化同步管理器
     */
    bool initialize();

    /**
     * @brief 关闭同步管理器
     */
    void shutdown();

    /**
     * @brief 启动同步
     */
    bool startSync();

    /**
     * @brief 停止同步
     */
    void stopSync();

    /**
     * @brief 获取同步状态
     */
    SyncStatus status() const;

    /**
     * @brief 获取已连接设备列表
     */
    QVector<DeviceInfo> connectedDevices() const;

    /**
     * @brief 发送剪贴板内容到其他设备
     * @param item 剪贴板项
     */
    void sendClipboardItem(const ClipboardItem& item);

    /**
     * @brief 设备配对
     * @param deviceId 设备 ID
     * @return 是否成功
     */
    bool pairDevice(const QString& deviceId);

    /**
     * @brief 取消设备配对
     * @param deviceId 设备 ID
     */
    void unpairDevice(const QString& deviceId);

    /**
     * @brief 获取已配对设备列表
     */
    QVector<DeviceInfo> pairedDevices() const;

    /**
     * @brief 设置加密密钥
     * @param key 密钥
     */
    void setEncryptionKey(const QString& key);

signals:
    /**
     * @brief 同步状态变化
     * @param status 新状态
     */
    void statusChanged(SyncStatus status);

    /**
     * @brief 收到远程剪贴板内容
     * @param item 剪贴板项
     */
    void remoteClipboardReceived(const ClipboardItem& item);

    /**
     * @brief 设备连接
     * @param device 设备信息
     */
    void deviceConnected(const DeviceInfo& device);

    /**
     * @brief 设备断开
     * @param deviceId 设备 ID
     */
    void deviceDisconnected(const QString& deviceId);

    /**
     * @brief 同步错误
     * @param error 错误信息
     */
    void syncError(const QString& error);

private slots:
    /**
     * @brief 处理收到的数据
     */
    void onDataReceived(const QByteArray& data);

    /**
     * @brief 设备发现
     */
    void onDeviceDiscovered(const DeviceInfo& device);

private:
    /**
     * @brief 启动局域网发现
     */
    bool startDiscovery();

    /**
     * @brief 停止局域网发现
     */
    void stopDiscovery();

    /**
     * @brief 加密数据
     */
    QByteArray encryptData(const QByteArray& data) const;

    /**
     * @brief 解密数据
     */
    QByteArray decryptData(const QByteArray& data) const;

    /**
     * @brief 生成本地设备 ID
     */
    QString generateDeviceId() const;

    SyncStatus m_status;               ///< 同步状态
    bool m_enabled;                    ///< 是否启用
    QString m_deviceId;                ///< 本地设备 ID
    QString m_deviceName;              ///< 设备名称
    QString m_encryptionKey;           ///< 加密密钥

    QVector<DeviceInfo> m_connectedDevices;  ///< 已连接设备
    QVector<DeviceInfo> m_pairedDevices;     ///< 已配对设备

    // MVP 阶段：只搭框架，具体实现后续补充
};

} // namespace uim
