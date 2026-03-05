// ╭────────────────────────────────────────────────────────╮
// │                         SERIAL                         │
// ╰────────────────────────────────────────────────────────╯

#define SERIAL_BAUD 9600

// ╭────────────────────────────────────────────────────────╮
// │                         ZIGBEE                         │
// ╰────────────────────────────────────────────────────────╯

#define ZIGBEE_RX_PIN UART2_RX_PIN
#define ZIGBEE_TX_PIN UART2_TX_PIN
#define ZIGBEE_SERIAL_BAUD 9600
#define ZIGBEE_API_ENABLED "2"
#define ZIGBEE_COORDINATOR "1"
#define ZIGBEE_PAN_ID "666"
#define ZIGBEE_CHANNELS "D5E3" // Scan channels 11-26 (0x07FFF800)

// ╭────────────────────────────────────────────────────────╮
// │              DEVICE DISCOVERY & MANAGEMENT             │
// ╰────────────────────────────────────────────────────────╯

#define MAX_DEVICES 10
#define MAX_DEVICE_NAME 20
#define DISCOVERY_INTERVAL 30000  // 30 seconds
#define DEVICE_DISCOVERY_TIMEOUT 5000  // 5 seconds per discovery cycle

// Device types
#define DEVICE_TYPE_COORDINATOR 0x00
#define DEVICE_TYPE_ROUTER 0x01
#define DEVICE_TYPE_END_DEVICE 0x02

// Device status
#define DEVICE_STATUS_UNKNOWN 0x00
#define DEVICE_STATUS_ACTIVE 0x01
#define DEVICE_STATUS_CONFIGURED 0x02
#define DEVICE_STATUS_ERROR 0xFF

// Remote configuration cluster
#define CONFIG_CLUSTER_ID 0x1234
#define CONFIG_PROFILE_ID 0x0104  // Home Automation Profile
#define CONFIG_ENDPOINT 0x01