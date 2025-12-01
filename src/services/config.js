/**
 * Configuration module for Gate Timer
 * Loads from environment variables with fallbacks
 * Note: Vue CLI uses process.env.VUE_APP_* syntax (webpack)
 */

export function getConfig() {
  return {
    // Communication mode: 'websocket', 'http', or 'udp'
    COMM_MODE: process.env.VUE_APP_COMM_MODE || "websocket",

    // Backend base URL for HTTP requests
    BACKEND_URL: process.env.VUE_APP_BACKEND_URL || "http://192.168.1.175:3000",

    // WebSocket URL
    WS_URL: process.env.VUE_APP_WS_URL || "ws://192.168.1.175:3000",

    // HTTP port
    HTTP_PORT: process.env.VUE_APP_HTTP_PORT || 3000,

    // WebSocket port
    WS_PORT: process.env.VUE_APP_WS_PORT || 3000,

    // UDP port (for reference, not used in browser)
    UDP_PORT: process.env.VUE_APP_UDP_PORT || 9090,

    // Frontend port
    FRONTEND_PORT: process.env.VUE_APP_PORT || 8081,
  };
}

export function getModeDescription(mode) {
  const descriptions = {
    websocket: "WebSocket - Real-time bidirectional communication",
    http: "HTTP POST - REST API communication",
    udp: "UDP - Lightweight datagram protocol",
  };
  return descriptions[mode.toLowerCase()] || mode;
}
