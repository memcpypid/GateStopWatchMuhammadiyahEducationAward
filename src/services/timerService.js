import { getConfig } from './config';

class TimerService {
  constructor() {
    this.config = getConfig();
    this.mode = this.config.COMM_MODE;
    this.baseURL = this.config.BACKEND_URL;
    this.wsURL = this.config.WS_URL;
    this.ws = null;
    this.listeners = [];
  }

  /**
   * Register a callback for timer events
   * @param {Function} callback - Function to call on events
   */
  subscribe(callback) {
    this.listeners.push(callback);
  }

  /**
   * Notify all subscribers of events
   * @param {Object} data - Event data
   */
  notifyListeners(data) {
    this.listeners.forEach((callback) => {
      try {
        callback(data);
      } catch (error) {
        console.error('Error in listener callback:', error);
      }
    });
  }

  /**
   * Initialize connection to backend
   * @param {String} address - IP address or URL (for WebSocket/HTTP)
   * @returns {Promise}
   */
  async connect(address = null) {
    if (address) {
      this.baseURL = `http://${address}:${this.config.HTTP_PORT}`;
      this.wsURL = `ws://${address}:${this.config.WS_PORT}`;
    }

    try {
      switch (this.mode.toLowerCase()) {
        case 'websocket':
          return await this.connectWebSocket();
        case 'http':
          return await this.testHTTPConnection();
        case 'udp':
          return await this.testUDPConnection();
        default:
          throw new Error(`Unknown communication mode: ${this.mode}`);
      }
    } catch (error) {
      console.error(`Failed to connect via ${this.mode}:`, error);
      throw error;
    }
  }

  /**
   * Connect via WebSocket
   */
  connectWebSocket() {
    return new Promise((resolve, reject) => {
      try {
        this.ws = new WebSocket(this.wsURL + '/ws');

        this.ws.onopen = () => {
          console.log('WebSocket connected');
          this.notifyListeners({ type: 'connected', mode: 'websocket' });
          resolve({ success: true, mode: 'websocket', message: 'Connected via WebSocket' });
        };

        this.ws.onmessage = (event) => {
          try {
            const data = JSON.parse(event.data);
            this.notifyListeners({ type: 'status_update', data });
          } catch (error) {
            console.error('Error parsing WebSocket message:', error);
          }
        };

        this.ws.onerror = (error) => {
          console.error('WebSocket error:', error);
          this.notifyListeners({ type: 'error', mode: 'websocket', error: error.message });
          reject(error);
        };

        this.ws.onclose = () => {
          console.log('WebSocket disconnected');
          this.notifyListeners({ type: 'disconnected', mode: 'websocket' });
        };
      } catch (error) {
        reject(error);
      }
    });
  }

  /**
   * Test HTTP connection
   */
  async testHTTPConnection() {
    try {
      const response = await fetch(`${this.baseURL}/health`);
      if (!response.ok) throw new Error(`HTTP ${response.status}`);
      const data = await response.json();
      this.notifyListeners({ type: 'connected', mode: 'http' });
      return { success: true, mode: 'http', message: 'Connected via HTTP', ...data };
    } catch (error) {
      console.error('HTTP connection failed:', error);
      throw error;
    }
  }

  /**
   * Test UDP connection (simulated - UDP doesn't have real connections)
   */
  async testUDPConnection() {
    // UDP is connection-less, but we can do a health check via HTTP
    try {
      const response = await fetch(`${this.baseURL}/health`);
      if (!response.ok) throw new Error(`HTTP ${response.status}`);
      const data = await response.json();
      this.notifyListeners({ type: 'connected', mode: 'udp' });
      return { success: true, mode: 'udp', message: 'Server ready for UDP', ...data };
    } catch (error) {
      console.error('UDP server check failed:', error);
      throw error;
    }
  }

  /**
   * Send trigger event (start or finish)
   * @param {String} triggerType - 'start' or 'finish'
   */
  async sendTrigger(triggerType) {
    const normalizedTrigger = triggerType.toLowerCase().trim();

    try {
      switch (this.mode.toLowerCase()) {
        case 'websocket':
          return this.sendTriggerWebSocket(normalizedTrigger);
        case 'http':
          return await this.sendTriggerHTTP(normalizedTrigger);
        case 'udp':
          return await this.sendTriggerUDP(normalizedTrigger);
        default:
          throw new Error(`Unknown communication mode: ${this.mode}`);
      }
    } catch (error) {
      console.error(`Failed to send trigger via ${this.mode}:`, error);
      this.notifyListeners({ type: 'error', trigger: triggerType, error: error.message });
      throw error;
    }
  }

  /**
   * Send trigger via WebSocket
   */
  sendTriggerWebSocket(triggerType) {
    return new Promise((resolve, reject) => {
      if (!this.ws || this.ws.readyState !== WebSocket.OPEN) {
        reject(new Error('WebSocket is not connected'));
        return;
      }

      try {
        const message = JSON.stringify({ trigger: triggerType });
        this.ws.send(message);
        console.log(`WebSocket trigger sent: ${triggerType}`);
        this.notifyListeners({ type: 'trigger_sent', trigger: triggerType, mode: 'websocket' });
        resolve({ success: true, trigger: triggerType, mode: 'websocket' });
      } catch (error) {
        reject(error);
      }
    });
  }

  /**
   * Send trigger via HTTP POST
   */
  async sendTriggerHTTP(triggerType) {
    const response = await fetch(`${this.baseURL}/trigger`, {
      method: 'POST',
      headers: { 'Content-Type': 'text/plain' },
      body: triggerType,
    });

    if (!response.ok) throw new Error(`HTTP ${response.status}`);

    const data = await response.json();
    console.log(`HTTP trigger sent: ${triggerType}`);
    this.notifyListeners({ type: 'trigger_sent', trigger: triggerType, mode: 'http' });
    return { success: true, trigger: triggerType, mode: 'http', ...data };
  }

  /**
   * Send trigger via UDP
   * Note: Real UDP requests can't be sent directly from browser due to security restrictions.
   * This sends via HTTP which will be forwarded to UDP internally or simulates UDP behavior.
   */
  async sendTriggerUDP(triggerType) {
    // For demonstration, we use HTTP to communicate with the backend
    // The backend internally handles it as UDP
    const response = await fetch(`${this.baseURL}/trigger`, {
      method: 'POST',
      headers: { 'Content-Type': 'text/plain' },
      body: triggerType,
    });

    if (!response.ok) throw new Error(`HTTP ${response.status}`);

    const data = await response.json();
    console.log(`UDP trigger sent (via HTTP): ${triggerType}`);
    this.notifyListeners({ type: 'trigger_sent', trigger: triggerType, mode: 'udp' });
    return { success: true, trigger: triggerType, mode: 'udp', ...data };
  }

  /**
   * Get current timer status
   */
  async getStatus() {
    try {
      const response = await fetch(`${this.baseURL}/status`);
      if (!response.ok) throw new Error(`HTTP ${response.status}`);
      return await response.json();
    } catch (error) {
      console.error('Failed to get status:', error);
      throw error;
    }
  }

  /**
   * Disconnect from backend
   */
  disconnect() {
    if (this.ws) {
      this.ws.close();
      this.ws = null;
    }
    this.notifyListeners({ type: 'disconnected' });
  }

  /**
   * Get current communication mode
   */
  getMode() {
    return this.mode;
  }

  /**
   * Change communication mode
   */
  setMode(newMode) {
    this.mode = newMode;
    this.disconnect();
  }
}

export default new TimerService();
