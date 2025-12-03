<template>
  <div class="min-h-screen bg-gradient-to-br from-slate-50 via-white to-slate-100 flex flex-col overflow-x-hidden">
    <!-- Header -->
    <header class="fixed top-0 left-0 right-0 bg-white border-b-2 border-red-600 shadow-lg z-50">
      <div class="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-4">
        <div class="flex items-center justify-between gap-4 lg:gap-8">
          <!-- Left Logo -->
          <div class="flex items-center gap-4 flex-shrink-0">
            <img src="../assets/umm.png" alt="umm-logo" class="h-14 sm:h-16 w-auto object-contain">
            <div class="hidden sm:block w-1 h-12 bg-gradient-to-b from-red-600 to-red-400"></div>
          </div>

          <!-- Center Title -->
          <div class="flex-1 text-center min-w-0">
            <h1 class="text-xl sm:text-2xl md:text-3xl font-bold text-red-600 truncate">
              Universitas Muhammadiyah Malang
            </h1>
            <p class="text-xs sm:text-sm text-gray-500 font-medium mt-1">
              Kontes Kapal Indonesia 2025
            </p>
          </div>

          <!-- Right Logo -->
          <div class="flex items-center gap-4 flex-shrink-0">
            <div class="hidden sm:block w-1 h-12 bg-gradient-to-b from-red-600 to-red-400"></div>
            <img src="../assets/maskot.png" alt="maskotlogo" class="h-20 sm:h-50 w-auto object-contain bg-white">
          </div>
        </div>
      </div>
    </header>

    <!-- Main Content -->
    <main class="flex-1 flex flex-col items-center justify-center px-4 sm:px-6 pt-10 sm:pt-10 pb-12">
      <!-- Container for Stopwatch -->
      <div class="w-full ">
        <div
          class="bg-white rounded-3xl shadow-2xl border-t-4 border-blue-600 overflow-hidden hover:shadow-3xl transition-shadow duration-300">
          <!-- Card Header -->
          <div class="bg-gradient-to-r from-blue-50 to-cyan-50 px-6 sm:px-8 py-6 border-b border-blue-200">
            <h3 class="text-2xl sm:text-3xl font-bold text-blue-700 text-center">Stopwatch</h3>
          </div>

          <!-- Timer Display Section -->
          <div class="px-6 sm:px-12 py-8 sm:py-12">
            <!-- Stopwatch Timer -->
            <div
              class="bg-gradient-to-br from-slate-50 to-slate-100 rounded-2xl border-2 border-slate-200 p-6 sm:p-10 mb-8 hover:border-blue-300 transition-colors duration-300">
              <div class="text-center">
                <div
                  class="font-mono font-black text-9xl sm:text-9xl lg:text-9xl text-blue-600 tracking-wider leading-none mb-4 select-none">
                  {{ stopwatchTime }}
                </div>
                <div
                  class="flex justify-center gap-36  mt-6 text-1xl  font-bold text-slate-600 uppercase tracking-widest">
                  <span class="px-3 py-1 bg-slate-200 rounded-full">Menit</span>
                  <span class="px-3 py-1 bg-slate-200 rounded-full">Detik</span>
                  <span class="px-3 py-1 bg-slate-200 rounded-full">MiliDetik</span>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </main>

    <!-- Reset Confirmation Modal -->

  </div>
</template>

<script>
const WS_URL = process.env.VUE_APP_WS_URL || 'ws://192.168.1.175:3000';

export default {
  name: "StopwatchPage",
  data() {
    return {
      showResetModal: false,
      stopwatchTime: '10:00:000',
      stopwatchRunning: false,
      ws: null,
      wsConnected: false,
      keyHandler: null,
    };
  },

  methods: {
    confirmReset() {
      this.showResetModal = false;
      this.sendTrigger("reset");
    },

    sendTrigger(triggerType) {
      if (!this.wsConnected) {
        console.warn("❌ WebSocket not connected!");
        return;
      }
      const message = JSON.stringify({ trigger: triggerType });
      this.ws.send(message);
      console.log("📡 Trigger sent:", triggerType);
    },

    zeroPrefix(num, digit) {
      return String(num).padStart(digit, '0');
    },

    formatTime(elapsedMs) {
      const minutes = Math.floor(elapsedMs / 60000);
      const seconds = Math.floor((elapsedMs % 60000) / 1000);
      const milliseconds = Math.floor(elapsedMs % 1000);
      return (
        this.zeroPrefix(minutes, 2) + ":" +
        this.zeroPrefix(seconds, 2) + ":" +
        this.zeroPrefix(milliseconds, 3)
      );
    },

    handleKeyPress(event) {
      const key = event.key.toUpperCase();

      switch (key) {
        case 'R':
          console.log("⌨️  Keyboard: R pressed → Resetting stopwatch");
          this.showResetModal = true;
          break;
      }
    },

    setupWebSocket() {
      try {
        this.ws = new WebSocket(`${WS_URL}/ws`);

        this.ws.onopen = () => {
          this.wsConnected = true;
          console.log("🟢 WebSocket Connected");
        };

        this.ws.onmessage = (event) => {
          try {
            const timerState = JSON.parse(event.data);
            console.log("📨 Received from backend:", timerState);
            this.stopwatchTime = this.formatTime(timerState.stopwatch_max_ms - timerState.stopwatch_elapsed_ms);
            if (timerState.stopwatch_running !== undefined) {
              this.stopwatchRunning = timerState.stopwatch_running;
            }
          } catch (error) {
            console.error("❌ Error parsing WebSocket message:", error);
            console.error("Raw data:", event.data);
          }
        };

        this.ws.onerror = (error) => {
          console.error("❌ WebSocket error:", error);
        };

        this.ws.onclose = () => {
          this.wsConnected = false;
          console.warn("🔴 WebSocket disconnected. Reconnecting in 2s...");
          setTimeout(this.setupWebSocket, 2000);
        };
      } catch (error) {
        console.error("❌ Failed to setup WebSocket:", error);
      }
    },
  },

  mounted() {
    console.log("🚀 Stopwatch Page Loaded");
    console.log("📡 WebSocket URL:", WS_URL);
    console.log("⌨️  Keyboard Shortcut: R=Reset");
    this.setupWebSocket();

    this.keyHandler = (event) => this.handleKeyPress(event);
    window.addEventListener('keydown', this.keyHandler);
  },

  beforeUnmount() {
    if (this.ws) {
      this.ws.close();
    }
    if (this.keyHandler) {
      window.removeEventListener('keydown', this.keyHandler);
    }
  },
};
</script>

<style scoped></style>
