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
            <img src="../assets/maskot.png" alt="mealogo" class="h-14 sm:h-16 w-auto object-contain">
          </div>
        </div>
      </div>
    </header>

    <!-- Main Content -->
    <main class="flex-1 flex flex-col items-center justify-center px-4 sm:px-6 pt-28 sm:pt-32 pb-12">
      <!-- Title Section -->
      <div class="mb-8 sm:mb-12 text-center w-full max-w-6xl">
        <h2 class="text-3xl sm:text-4xl lg:text-5xl font-bold text-gray-900 mb-4 leading-tight">
          Kontes Kapal Indonesia
        </h2>
        <div
          class="h-1.5 w-20 sm:w-24 bg-gradient-to-r from-red-600 via-red-500 to-red-400 mx-auto rounded-full shadow-md">
        </div>
      </div>

      <!-- Container for Timer and History -->
      <div class="w-full max-w-6xl flex gap-4 sm:gap-6 lg:gap-8">
        <!-- History Sidebar -->
        <div class="hidden lg:flex flex-col w-64 flex-shrink-0">
          <div
            class="bg-white rounded-2xl shadow-xl border-t-4 border-red-600 overflow-hidden h-full flex flex-col max-h-96">
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
                <p class="text-sm">No history yet</p>
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

        <!-- Timer Card Container -->
        <div class="w-full lg:max-w-3xl">
          <div
            class="bg-white rounded-3xl shadow-2xl border-t-4 border-red-600 overflow-hidden hover:shadow-3xl transition-shadow duration-300">
            <!-- Card Header -->
            <div class="bg-gradient-to-r from-red-50 to-pink-50 px-6 sm:px-8 py-6 border-b border-red-200">
              <h3 class="text-2xl sm:text-3xl font-bold text-red-700 text-center">⏱️ Timer Kompetisi</h3>
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
                    <span class="px-3 py-1 bg-slate-200 rounded-full">Menit (MM)</span>
                    <span class="px-3 py-1 bg-slate-200 rounded-full">Detik (SS)</span>
                    <span class="px-3 py-1 bg-slate-200 rounded-full">Milidetik (ms)</span>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </main>
  </div>
</template>

<script>
const WS_URL = process.env.VUE_APP_WS_URL || 'ws://localhost:3000';

export default {
  name: "JuriPage",
  data() {
    return {
      data: {
        time: '00:00:00'
      },
      history: [],
      ws: null,
      wsConnected: false,
    };
  },

  methods: {
    zeroPrefix(num, digit) {
      return String(num).padStart(digit, '0');
    },

    formatTime(elapsedMs) {
      const minutes = Math.floor(elapsedMs / 60000);
      const seconds = Math.floor((elapsedMs % 60000) / 1000);
      const milliseconds = elapsedMs % 1000;

      return (
        this.zeroPrefix(minutes, 2) + ":" +
        this.zeroPrefix(seconds, 2) + ":" +
        this.zeroPrefix(milliseconds, 2)
      );
    },

    formatHistoryTime(elapsedMs) {
      const minutes = Math.floor(elapsedMs / 60000);
      const seconds = Math.floor((elapsedMs % 60000) / 1000);
      const milliseconds = Math.floor(elapsedMs % 1000 / 10);

      return (
        this.zeroPrefix(minutes, 2) + ":" +
        this.zeroPrefix(seconds, 2) + ":" +
        this.zeroPrefix(milliseconds, 2)
      );
    },

    formatTimestamp(timestamp) {
      const date = new Date(timestamp);
      const hours = this.zeroPrefix(date.getHours(), 2);
      const minutes = this.zeroPrefix(date.getMinutes(), 2);
      const seconds = this.zeroPrefix(date.getSeconds(), 2);
      return `${hours}:${minutes}:${seconds}`;
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

    setupWebSocket() {
      try {
        this.ws = new WebSocket(`${WS_URL}/ws`);

        this.ws.onopen = () => {
          this.wsConnected = true;
          console.log("🟢 WebSocket Connected (Juri)");
          this.loadHistory();
        };

        this.ws.onmessage = (event) => {
          try {
            const timerState = JSON.parse(event.data);
            console.log("📨 Received from backend:", timerState);
            if (timerState.status === 'Finished' && !timerState.running) {
              this.loadHistory()
            }
            this.data.time = this.formatTime(timerState.elapsed_ms);
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
    console.log("🚀 Juri Page Loaded");
    console.log("📡 WebSocket URL:", WS_URL);
    this.setupWebSocket();
    this.loadHistory();
  },

  beforeUnmount() {
    if (this.ws) {
      this.ws.close();
    }
  },
};
</script>

<style scoped></style>
