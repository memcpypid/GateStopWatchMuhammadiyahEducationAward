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
      <div class="mb-8 sm:mb-12 text-center w-full max-w-4xl">
        <h2 class="text-3xl sm:text-4xl lg:text-5xl font-bold text-gray-900 mb-4 leading-tight">
          Kontes Kapal Indonesia
        </h2>
        <div
          class="h-1.5 w-20 sm:w-24 bg-gradient-to-r from-red-600 via-red-500 to-red-400 mx-auto rounded-full shadow-md">
        </div>
      </div>

      <!-- Juri Badge -->
      <!-- <div class="mb-8 px-6 py-3 bg-gradient-to-r from-purple-100 to-blue-100 border-2 border-purple-400 rounded-full">
        <p class="text-base sm:text-lg font-bold text-purple-700">
          Halaman Juri - Monitor Waktu
        </p>
      </div> -->

      <!-- Timer Card Container -->
      <div class="w-full max-w-4xl">
        <div
          class="bg-white rounded-3xl shadow-2xl border-t-4 border-red-600 overflow-hidden hover:shadow-3xl transition-shadow duration-300">
          <!-- Card Header -->
          <div class="bg-gradient-to-r from-red-50 to-pink-50 px-6 sm:px-8 py-6 border-b border-red-200">
            <h3 class="text-2xl sm:text-3xl font-bold text-red-700 text-center">Timer Kompetisi</h3>
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
                  <span class="px-3 py-1 bg-slate-200 rounded-full">Minutes (MM)</span>
                  <span class="px-3 py-1 bg-slate-200 rounded-full">Seconds (SS)</span>
                  <span class="px-3 py-1 bg-slate-200 rounded-full">Milliseconds (ms)</span>
                </div>
              </div>
            </div>

            <!-- Status Indicator -->
            <!-- <div class="flex justify-center">
              <div
                class="px-6 py-3 bg-gradient-to-r from-red-50 to-pink-50 border-2 border-red-200 rounded-full hover:border-red-400 transition-colors duration-200">
                <p class="text-sm font-semibold text-red-700">
                  Status: <span class="font-black text-red-600">Monitoring</span>
                </p>
              </div>
            </div> -->
          </div>

          <!-- Info Section -->
          <!-- <div class="px-6 sm:px-8 py-8 bg-gradient-to-b from-white to-slate-50 border-t border-slate-200">
            <div class="bg-blue-50 border-l-4 border-blue-500 p-4 rounded">
              <p class="text-sm text-blue-800">
                <span class="font-bold">Catatan Juri:</span> Halaman ini menampilkan waktu secara real-time dari server. Anda dapat memonitor durasi kompetisi dengan akurat.
              </p>
            </div>
          </div> -->
        </div>
      </div>

      <!-- Footer Note -->
      <!-- <div class="mt-12 text-center text-gray-500 text-sm">
        <p>Pembaruan otomatis setiap 50 milidetik</p>
      </div> -->
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
        this.zeroPrefix(milliseconds, 3)
      );
    },

    setupWebSocket() {
      try {
        this.ws = new WebSocket(`${WS_URL}/ws`);

        this.ws.onopen = () => {
          this.wsConnected = true;
          console.log("🟢 WebSocket Connected (Juri)");
        };

        this.ws.onmessage = (event) => {
          try {
            const timerState = JSON.parse(event.data);
            console.log("📨 Received from backend:", timerState);
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
  },

  beforeUnmount() {
    if (this.ws) {
      this.ws.close();
    }
  },
};
</script>

<style scoped>
/* Pure Tailwind CSS - No custom styles needed */
</style>
