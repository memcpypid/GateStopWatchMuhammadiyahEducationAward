<template>
  <div
    class="min-h-screen bg-gradient-to-r from-blue-500 via-white to-blue-500 flex flex-col justify-center items-center">
    <div class="fixed top-0 left-0 w-full flex justify-between items-center p-4 bg-white shadow-lg">
      <div class="flex items-center space-x-4">
        <img src="../assets/umm.png" alt="umm-logo" class="h-20 w-auto">
      </div>
      <h1 class="text-4xl font-bold text-gray-900 text-center">Muhammadiyah Education Award</h1>
      <img src="../assets/mealogo.png" alt="mealogo" class="h-20 w-auto">
    </div>

    <div class="mt-10">
      <h2 class="text-3xl font-semibold text-gray-800 mt-24">Line Follower Competition</h2>
    </div>

    <div
      class=" bg-white shadow-xl rounded-xl p-5 text-center w-3/4 mt-5  hover:scale-105 transition-transform duration-500">
      <h1 class="mb-6 text-4xl font-bold font-serif text-blue-600">Timer</h1>
      <div class="border-t-4 border-b-4 border-blue-600 py-6">
        <span class="time text-8xl font-mono text-blue-600">{{ data.time }}</span>
        <div class="flex justify-center space-x-12 mt-4 text-lg uppercase font-semibold text-gray-700">
          <span class="pl-6">Minutes</span>
          <span class="pl-6">Seconds</span>
          <span class="pl-6">Milliseconds</span>
        </div>
      </div>
    </div>

    <div
      class="bg-gray-100 shadow-xl rounded-xl p-5 text-center w-3/2 h-3/2  mt-5 hover:scale-105 transition-transform duration-500 border border-gray-500">
      <label class="text-2xl text-center font-serif text-gray-800">Nama Sekolah : </label>
      <select name=""
        class="text-2xl text-gray-800 mb-10  rounded-md p-2 transition duration-300 hover:border-gray-600 focus:border-gray-600 focus:outline-none cursor-pointer">
        <option value="">Sekolah A</option>
        <option value="">Sekolah B</option>
        <option value="">Sekolah C</option>
      </select>

      <h1 class="mb-5 text-center font-serif font-bold text-3xl text-gray-600">Time Lapse</h1>
      <div class="border-t-4 border-gray-600 py-6">
        <span class="time text-7xl font-mono text-gray-600">{{ timelapse.time }}</span>
        <div class="flex justify-center space-x-12  text-lg uppercase font-semibold text-gray-700">
          <span class="border-b-2 border-gray-600 ml-5 text-center hover:text-gray-600 cursor-pointer">Minutes</span>
          <span class="border-b-2 border-gray-600   text-center hover:text-gray-600 cursor-pointer">Seconds</span>
          <span
            class="flex border-b-2 border-gray-600 ml-5 text-center hover:text-gray-600 cursor-pointer">Millisec.</span>
        </div>
      </div>
    </div>

    <div class="flex justify-center mt-12 space-x-4 mb-10">
      <button @click="startPreparation"
        class="bg-gray-500 hover:bg-gray-600 text-white font-bold py-3 px-6 rounded-lg shadow-lg transform hover:scale-110 transition-transform duration-300">
        Waktu Persiapan
      </button>

      <button @click="start"
        class="bg-red-500 hover:bg-gray-600 text-white font-bold py-3 px-12 rounded-lg shadow-lg transform hover:scale-110 transition-transform duration-300">
        Start
      </button>
      <button @click="resetTimelapse"
        class="bg-red-500 hover:bg-gray-600 text-white font-bold py-3 px-6 rounded-lg shadow-lg transform hover:scale-110 transition-transform duration-300">
        Retry Finish
      </button>
      <button @click="detectFinish"
        class="bg-red-500 hover:bg-gray-600 text-white font-bold py-3 px-6 rounded-lg shadow-lg transform hover:scale-110 transition-transform duration-300">
        Deteksi Finish
      </button>
      <div class="status-indicator flex justify-center mt-4 ">
        <label class="pr-2 pt-1 font-serif font-bold">Detect Finish</label>
        <div v-if="Detectfinish" class="w-8 h-8 bg-green-500 rounded-full"></div>
        <div v-else class="w-8 h-8 bg-red-500 rounded-full"></div>
      </div>
    </div>
    <div class="pb-5">
      <button @click="showResetModal = true"
        class="bg-red-500 hover:bg-gray-600 text-white font-bold py-3 px-6 rounded-lg shadow-lg transform hover:scale-110 transition-transform duration-300">
        Reset
      </button>
    </div>
    <div class="flex justify-start mr-5">
      <div class="flex justify-start mr-20">
        <label class="text-left font-mono font-bold text-2xl mr-20">Status : {{ status }}</label>
      </div>
    </div>

    <div class="flex justify-center mt-2 space-x-4 mb-5">
      <label class="flex justify-center text-2xl">IP Address : </label>
      <input v-model="Ipaddres" type="text"
        class="bg-gray-200 border-b-3 pb-2 place-content-center placeholder-gray-700">
      <button @click="connect"
        class="bg-green-500 hover:bg-green-600 text-white font-bold py-3 px-6 rounded-lg shadow-lg transform hover:scale-110 transition-transform duration-300">Connect</button>
    </div>
    <div>
      <div v-if="showResetModal" class="fixed z-10 inset-0 overflow-y-auto">
        <div class="flex items-center justify-center min-h-screen">
          <div class="bg-white rounded-lg shadow-xl p-6">
            <h2 class="text-lg font-semibold mb-4">Apakah Anda yakin ingin mereset?</h2>
            <div class="flex justify-end space-x-4">
              <button @click="showResetModal = false" class="bg-gray-500 text-white font-bold py-2 px-4 rounded">
                No
              </button>
              <button @click="confirmReset" class="bg-red-500 text-white font-bold py-2 px-4 rounded">
                Yes
              </button>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>
<script>
export default {
  name: "StopwatchPage",
  data() {
    return {
      showResetModal: false, // Kontrol visibilitas modal
      timeBegan: null,
      timeStopped: null,
      stoppedDuration: 0,
      started: null,
      running: false,
      Ipaddres: '',
      status: 'Not Connect',
      timelapse: {
        time: '00:00:00'
      },
      data: {
        time: '00:00:00' // Tambahkan format untuk millisecond
      },
      preparationTime: '01:00:00', // Properti untuk waktu persiapan termasuk millisecond
      preparationInterval: null, // Interval untuk hitungan mundur
      totalPreparationTime: 60000, // Waktu persiapan total dalam millisecond (1 menit)
      ws: null, // Menambahkan properti untuk WebSocket
      Triggered: false,
      timeoutID: null,
      Detectfinish: false,
      startCount: false,
      finishRobot: false,
      startPersiapan: false
    };
  },
  methods: {
    confirmReset() {
      this.showResetModal = false;
      this.resetButton();
    },
    connect() {
      console.log(this.Ipaddres)
      this.setupWebSocket();
    },
    detectFinish() {
      if (this.running || this.Triggered) {
        this.Detectfinish = true;
      }
    },
    startTimer() {
      if (this.timeBegan === null) {
        this.reset();
        this.timeBegan = new Date();
      }
      if (this.timeStopped !== null) {
        this.stoppedDuration += (new Date() - this.timeStopped);
      }
      this.started = setInterval(this.clockRunning, 10);
      this.running = true;
    },
    start() {
      if (!this.startPersiapan) {
        this.startCount = true;
        if (this.running) return;
        this.timeoutID = setTimeout(() => {
          if (!this.Triggered) {
            this.startTimer();
          }
        }, 2000);
      }
    },
    stop() {
      this.running = false;
      this.timeStopped = new Date();
      clearInterval(this.started);
    },
    reset() {
      this.startPersiapan = false;
      this.running = false;
      clearInterval(this.started);
      this.stoppedDuration = 0;
      this.timeBegan = null;
      this.timeStopped = null;
      this.data.time = "00:00:00";
    },
    resetButton() {

      this.startPersiapan = false;
      this.running = false;
      clearInterval(this.started);
      clearInterval(this.preparationInterval);
      this.stoppedDuration = 0;
      this.timeBegan = null;
      this.timeStopped = null;
      this.data.time = "00:00:00";
      this.timelapse.time = "00:00:00";
      this.Triggered = false;
      this.timeoutID = null;
      this.Detectfinish = false;
      this.startCount = false;
      this.finishRobot = false;
      this.startPersiapan = false;

    },
    resetTimelapse() {
      this.timelapse.time = "00:00:00";
      this.finishRobot = false;
      this.Detectfinish = false;
    },
    finish() {
      if (!this.finishRobot) {
        this.timelapse.time = this.data.time;
        this.finishRobot = true;
      }
    },
    clockRunning() {
      const currentTime = new Date();
      const timeElapsed = new Date(currentTime - this.timeBegan - this.stoppedDuration);
      const min = timeElapsed.getUTCMinutes();
      const sec = timeElapsed.getUTCSeconds();
      const ms = timeElapsed.getUTCMilliseconds();

      this.data.time =
        this.zeroPrefix(min, 2) + ":" +
        this.zeroPrefix(sec, 2) + ":" +
        this.zeroPrefix(ms, 3);
    },
    zeroPrefix(num, digit) {
      return String(num).padStart(digit, '0');
    },
    setupWebSocket() {
      if (this.ws) {
        console.log("Resetting existing WebSocket connection...");
        this.ws.close(); 
      }
      this.ws = new WebSocket(`ws://${this.Ipaddres}:81`);
      this.ws.onopen = () => {
        console.log("WebSocket connection established");
        this.status = 'Connected';
      };
      this.ws.onmessage = (event) => {
        const message = event.data;
        console.log(message);
        if (message === "Trigger" && !this.running && this.startCount) {
          console.log('Start');
          this.Triggered = true;
          clearTimeout(this.timeoutID);
          this.startTimer();
        } else if (message === "Trigger" && this.running && this.Detectfinish) {
          console.log("Finish");
          this.finish();
        }
      };

      this.ws.onclose = () => {
        console.log("WebSocket connection closed");
        this.status = 'Not Connect';
        setTimeout(() => this.setupWebSocket(), 1000);
      };

      this.ws.onerror = (error) => {
        console.error("WebSocket error: ", error);
      };
    },
    // setupWebSocket() {
    //   this.ws = new WebSocket(`ws://${this.Ipaddres}:81`);
      
    //   this.ws.onmessage = (event) => {
    //     const message = event.data;
    //     console.log(message);
    //     if (message === "Trigger" && !this.running && this.startCount) {
    //       console.log('Start');
    //       this.Triggered = true;
    //       clearTimeout(this.timeoutID);
    //       this.startTimer();
    //     } else if (message === "Trigger" && this.running && this.Detectfinish) {
    //       console.log("Finish");
    //       this.finish();
    //     }
    //   };

    //   this.ws.onopen = () => {
    //     console.log("WebSocket connection established");
    //     this.status = 'Connected'
    //   };

    //   this.ws.onclose = () => {
    //     console.log("WebSocket connection closed");
    //     this.status = 'Not Connect'
    //     setTimeout(this.setupWebSocket, 1000);
    //   };

    //   this.ws.onerror = (error) => {
    //     console.error("WebSocket error: ", error);
    //   };
    // },
    startPreparation() {
      if (this.Triggered || this.running) { return; }
      this.startPersiapan = true;
      let totalTime = this.totalPreparationTime;
      this.preparationInterval = setInterval(() => {
        totalTime -= 10;
        const minutes = Math.floor(totalTime / 60000);
        const seconds = Math.floor((totalTime % 60000) / 1000);
        const milliseconds = totalTime % 1000;
        this.preparationTime = `${this.zeroPrefix(minutes, 2)}:${this.zeroPrefix(seconds, 2)}:${this.zeroPrefix(milliseconds, 2)}`;
        this.data.time = this.preparationTime;
        if (totalTime <= 0) {
          clearInterval(this.preparationInterval);
          this.preparationTime = '00:00:00';
          this.data.time = '00:00:00';
          alert('Waktu persiapan selesai!');
          this.startPersiapan = false;
        }
      }, 10);
    }
  },
  mounted() {
    // this.setupWebSocket();
  }
};
</script>



<style scoped>
body,
html {
  margin: 0;
  padding: 0;
}

.bg-gradient-to-r {
  background: linear-gradient(90deg, rgba(29, 78, 216, 1) 0%, rgba(255, 255, 255, 1) 50%, rgba(29, 78, 216, 1) 100%);
}
</style>

<!-- 
<style>
@import url('https://fonts.googleapis.com/css2?family=Share+Tech+Mono&display=swap');

html,
body {
  padding: 0;
  margin: 0;
  width: 100%;
  height: 100%;
}

.wrapper {
  display: flex;
  flex-direction: column;
  align-items: center;
}

body {
  background-color: #FFFF;
  font-family: 'Share Tech Mono', sans-serif;
  display: flex;
  flex-direction: row;
  justify-content: center;
  align-items: center;
}

#clock {
  color: #000000;
}

.time {
  font-size: 6.5em;
}

.btn-container {
  display: flex;
  margin-top: 15px;
}

.btn-container a {
  text-align: center;
  background: transparent;
  border: 3px solid rgb(200, 200, 200);
  color: rgb(200, 200, 200);
  padding: 10px 15px;
  margin: 0 10px;
  text-transform: uppercase;
  font-size: 2em;
  cursor: pointer;
  text-shadow: 0px 0px 10px rgb(200, 200, 200);
  flex-grow: 1;
  transition: color 0.1s ease-out;
}

.btn-container a:hover {
  color: white;
}
</style> -->
