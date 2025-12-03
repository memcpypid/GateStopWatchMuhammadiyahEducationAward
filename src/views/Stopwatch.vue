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
      <!-- Container for Timer and History -->
      <div class="w-full max-w-10xl flex gap-4 sm:gap-6 lg:gap-8">
        <!-- Stopwatch Timer (Left) -->
        <div class="hidden lg:block w-80 flex-shrink-0">
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
                    class="font-mono font-black text-2xl sm:text-2xl lg:text-2xl text-blue-600 tracking-wider leading-none mb-4 select-none">
                    {{ stopwatchTime }}
                  </div>
                  <div
                    class="flex justify-center gap-6 sm:gap-12 mt-6 text-xs sm:text-sm font-bold text-slate-600 uppercase tracking-widest">
                    <span class="px-3 py-1 bg-slate-200 rounded-full">MM</span>
                    <span class="px-3 py-1 bg-slate-200 rounded-full">SS</span>
                    <span class="px-3 py-1 bg-slate-200 rounded-full">ms</span>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>

        <!-- History Sidebar -->
        <!-- Timer Card Container -->
        <div class="w-full">
          <div
            class="bg-white rounded-3xl shadow-2xl border-t-4 border-red-600 overflow-hidden hover:shadow-3xl transition-shadow duration-300">
            <!-- Card Header -->
            <div class="bg-gradient-to-r from-red-50 to-pink-50 px-6 sm:px-8 py-6 border-b border-red-200">
              <h3 class="text-2xl sm:text-3xl font-bold text-red-700 text-center">Timer Lapse</h3>
            </div>

            <!-- Timer Display Section -->
            <div class="px-6 sm:px-12 py-8 sm:py-12">
              <!-- Main Timer -->
              <div
                class="bg-gradient-to-br from-slate-50 to-slate-100 rounded-2xl border-2 border-slate-200 p-6 sm:p-10 mb-8 hover:border-red-300 transition-colors duration-300">
                <div class="text-center">
                  <div
                    class="font-mono font-black text-7xl sm:text-8xl lg:text-9xl text-red-600 tracking-wider leading-none mb-4 select-none">
                    {{ data.time }}
                  </div>
                  <div
                    class="flex justify-center gap-6 sm:gap-12 mt-6 text-xs sm:text-sm font-bold text-slate-600 uppercase tracking-widest">
                    <span class="px-3 py-1 bg-slate-200 rounded-full">MM</span>
                    <span class="px-3 py-1 bg-slate-200 rounded-full">SS</span>
                    <span class="px-3 py-1 bg-slate-200 rounded-full">ms</span>
                  </div>
                </div>
              </div>
            </div>

            <!-- Control Buttons Section -->
            <div class="px-6 sm:px-8 py-8 bg-gradient-to-b from-white to-slate-50 border-t border-slate-200">
              <div class="flex flex-col sm:flex-row gap-3 sm:gap-4 justify-center">
                <!-- Start Button -->
                <!-- <button @click="start"
                  class="px-8 sm:px-12 py-3 sm:py-4 bg-gradient-to-br from-green-400 to-green-600 hover:from-green-500 hover:to-green-700 active:from-green-600 active:to-green-800 text-white font-bold text-base sm:text-lg rounded-xl shadow-lg hover:shadow-2xl active:shadow-md transform hover:scale-105 active:scale-95 transition-all duration-200 flex items-center justify-center gap-2 whitespace-nowrap">
                  <span class="text-xl"></span> Start
                </button> -->

                <!-- Finish Button -->
                <!-- <button @click="finish"
                  class="px-8 sm:px-12 py-3 sm:py-4 bg-gradient-to-br from-amber-400 to-amber-600 hover:from-amber-500 hover:to-amber-700 active:from-amber-600 active:to-amber-800 text-white font-bold text-base sm:text-lg rounded-xl shadow-lg hover:shadow-2xl active:shadow-md transform hover:scale-105 active:scale-95 transition-all duration-200 flex items-center justify-center gap-2 whitespace-nowrap">
                  <span class="text-xl"></span> Finish
                </button> -->

                <!-- Reset Button -->
                <button @click="showResetModal = true"
                  class="px-8 sm:px-12 py-3 sm:py-4 bg-gradient-to-br from-red-500 to-red-700 hover:from-red-600 hover:to-red-800 active:from-red-700 active:to-red-900 text-white font-bold text-base sm:text-lg rounded-xl shadow-lg hover:shadow-2xl active:shadow-md transform hover:scale-105 active:scale-95 transition-all duration-200 flex items-center justify-center gap-2 whitespace-nowrap">
                  <span class="text-xl"></span> Reset
                </button>
              </div>
            </div>
          </div>
        </div>
        <div class="hidden lg:flex flex-col w-64 flex-shrink-0 overflow-y-auto max-h-[600px]">
          <div class="bg-white rounded-2xl shadow-xl border-t-4 border-red-600 overflow-hidden h-full flex flex-col">
            <!-- History Header -->
            <div class="bg-gradient-to-r from-red-50 to-pink-50 px-6 py-4 border-b border-red-200 flex-shrink-0">
              <div class="flex items-center justify-between">
                <h3 class="text-lg font-bold text-red-700">📋 History</h3>
                <button @click="clearHistory" v-if="history.length > 0"
                  class="text-xs px-2 py-1 bg-red-200 hover:bg-red-300 text-red-700 rounded font-semibold transition-colors">
                  Clear
                </button>
              </div>
            </div>

            <!-- History List -->
            <div class="flex-1 overflow-y-auto p-4 space-y-2">
              <div v-if="history.length === 0" class="text-center py-8 text-gray-400">
                <p class="text-sm">History </p>
              </div>
              <div v-for="(entry, index) in history" :key="entry.id"
                class="bg-gradient-to-r from-slate-50 to-slate-100 p-3 rounded-lg border border-slate-200 hover:border-red-300 transition-colors">
                <p class="text-xs font-semibold text-red-600 mb-1">Time Lapse {{ index + 1 }}</p>
                <p class="text-lg font-mono font-bold text-gray-800">{{ formatHistoryTime(entry.elapsed_ms) }}</p>
                <p class="text-xs text-gray-500 mt-1">{{ formatTimestamp(entry.timestamp) }}</p>
              </div>
            </div>
          </div>
        </div>

      </div>
    </main>

    <!-- Reset Confirmation Modal -->
    <transition enter-active-class="transition ease-out duration-200" enter-from-class="opacity-0 scale-95"
      enter-to-class="opacity-100 scale-100" leave-active-class="transition ease-in duration-200"
      leave-from-class="opacity-100 scale-100" leave-to-class="opacity-0 scale-95">
      <div v-if="showResetModal"
        class="fixed inset-0 bg-black bg-opacity-50 z-50 flex items-center justify-center p-4 backdrop-blur-sm">
        <div class="bg-white rounded-2xl shadow-2xl p-6 sm:p-8 max-w-sm w-full border-t-4 border-red-600">
          <h2 class="text-2xl sm:text-3xl font-bold text-gray-900 mb-3">🔄 Konfirmasi Reset</h2>
          <p class="text-gray-600 mb-8 text-sm sm:text-base leading-relaxed">
            Apakah Anda yakin ingin mereset timer? Tindakan ini tidak dapat dibatalkan.
          </p>
          <div class="flex flex-col sm:flex-row gap-3 justify-end">
            <button @click="showResetModal = false"
              class="px-6 py-2 sm:py-3 bg-gradient-to-br from-gray-300 to-gray-400 hover:from-gray-400 hover:to-gray-500 text-gray-800 font-bold rounded-lg transition-all duration-200 hover:shadow-md active:scale-95">
              Batal
            </button>
            <button @click="confirmReset"
              class="px-6 py-2 sm:py-3 bg-gradient-to-br from-red-500 to-red-700 hover:from-red-600 hover:to-red-800 text-white font-bold rounded-lg transition-all duration-200 hover:shadow-md active:scale-95">
              Reset Sekarang
            </button>
          </div>
        </div>
      </div>
    </transition>
  </div>
</template>

<script>
const WS_URL = process.env.VUE_APP_WS_URL || 'ws://192.168.1.175:3000';

export default {
  name: "StopwatchPage",
  data() {
    return {
      showResetModal: false,
      data: {
        time: '00:00:00'
      },
      history: [],
      ws: null,
      wsConnected: false,
      keyHandler: null,
      historyRefreshInterval: null,
      stopwatchTime: '10:00:000',
      stopwatchRunning: false,
      lastStatus: "Idle",
    };
  },

  methods: {
    confirmReset() {
      this.showResetModal = false;
      this.sendTrigger("reset");
    },

    start() {
      this.sendTrigger("start");
    },

    finish() {
      this.sendTrigger("finish");
      // setTimeout(() => this.loadHistory(), 500);
    },


    async loadHistory() {
      try {
        const baseURL = WS_URL.replace(/^ws/, 'http').split('/ws')[0];
        const response = await fetch(`${baseURL}/history`);
        if (response.ok) {
          const data = await response.json();
          this.history = data.data || [];
        }
      } catch (error) {
        console.error('Failed to load history:', error);
      }
    },

    async clearHistory() {
      if (!confirm('Are you sure you want to clear all history?')) {
        return;
      }
      try {
        const baseURL = WS_URL.replace(/^ws/, 'http').split('/ws')[0];
        const response = await fetch(`${baseURL}/history/clear`, {
          method: 'POST'
        });
        if (response.ok) {
          this.history = [];
        }
      } catch (error) {
        console.error('Failed to clear history:', error);
      }
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

    formatHistoryTime(elapsedMs) {
      const minutes = Math.floor(elapsedMs / 60000);
      const seconds = Math.floor((elapsedMs % 60000) / 1000);
      const milliseconds = Math.floor(elapsedMs % 1000);

      return (
        this.zeroPrefix(minutes, 2) + ":" +
        this.zeroPrefix(seconds, 2) + ":" +
        this.zeroPrefix(milliseconds, 3)
      );
    },

    formatTimestamp(timestamp) {
      const date = new Date(timestamp);
      const hours = this.zeroPrefix(date.getHours(), 2);
      const minutes = this.zeroPrefix(date.getMinutes(), 2);
      const seconds = this.zeroPrefix(date.getSeconds(), 2);
      return `${hours}:${minutes}:${seconds}`;
    },

    handleKeyPress(event) {
      const key = event.key.toUpperCase();

      switch (key) {
        case 'S':
          console.log("⌨️  Keyboard: S pressed → Starting timer");
          this.start();
          break;
        case 'F':
          console.log("⌨️  Keyboard: F pressed → Finishing timer");
          this.finish();
          break;
        case 'R':
          console.log("⌨️  Keyboard: R pressed → Resetting timer");
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
          this.loadHistory();
        };

        this.ws.onmessage = (event) => {
          try {
            const timerState = JSON.parse(event.data);
            console.log("📨 Received from backend:", timerState);
            if (timerState.status !== this.lastStatus) {
              this.loadHistory();
              this.lastStatus = timerState.status;
            }
            this.data.time = this.formatTime(timerState.elapsed_ms);
            this.stopwatchTime = this.formatTime(timerState.stopwatch_elapsed_ms);
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
    console.log("🚀 Timer Frontend Loaded");
    console.log("📡 WebSocket URL:", WS_URL);
    console.log("⌨️  Keyboard Shortcuts: S=Start, F=Finish, R=Reset");
    this.setupWebSocket();
    this.loadHistory();

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
