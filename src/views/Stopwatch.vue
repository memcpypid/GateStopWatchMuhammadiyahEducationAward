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
      class=" bg-white shadow-xl rounded-xl p-10 text-center w-3/4 mt-5  hover:scale-105 transition-transform duration-500">
      <h1 class="mb-6 text-4xl font-bold font-serif text-blue-600">Timer</h1>
      <div class="border-t-4 border-b-4 border-blue-600 py-6">
        <span class="time text-8xl font-mono text-blue-600">{{ data.time }}</span>
        <div class="flex justify-center space-x-12 mt-4 text-lg uppercase font-semibold text-gray-700">
          <span>Minutes</span>
          <span>Seconds</span>
          <span>Milliseconds</span>
        </div>
      </div>
    </div>

    <div
      class="bg-gray-100 shadow-xl rounded-xl p-10 text-center w-3/2 h-3/2  mt-5 hover:scale-105 transition-transform duration-500 border border-gray-500">
      <h1 class="mb-5 text-center font-mono font-bold text-3xl text-gray-600">TIME LAPSE</h1>
      <label class="text-2xl text-center font-mono text-gray-800">Nama Sekolah : </label>
      <select name=""
        class="text-2xl font-bold text-gray-800 mb-10 border border-gray-400 rounded-md p-2 transition duration-300 hover:border-gray-600 focus:border-gray-600 focus:outline-none cursor-pointer">
        <option value="">Sekolah A</option>
        <option value="">Sekolah B</option>
        <option value="">Sekolah C</option>
      </select>
      <div>
        <div class="justify-start text-2xl pb-2 font-mono pl-10 flex">Nama : </div>
        <div class="justify-start text-2xl pb-2 font-mono pl-10 flex">Nama : </div>

      </div>

      <div class="border-t-4 border-gray-600 py-6">
        <span class="time text-8xl font-mono text-gray-600">{{ timelapse.time }}</span>
        <div class="flex justify-center space-x-12 mt-4 text-lg uppercase font-semibold text-gray-700">
          <span class="border-b-2 border-gray-600  mr-10 text-center hover:text-gray-600 cursor-pointer">Minutes</span>
          <span class="border-b-2 border-gray-600  mr-2 text-center hover:text-gray-600 cursor-pointer">Seconds</span>
          <span
            class="border-b-2 border-gray-600  mr-5 text-center hover:text-gray-600 cursor-pointer">Milliseconds</span>
        </div>
      </div>
    </div>

    <div class="flex justify-center mt-12 space-x-4 mb-10">
      <button @click="resetTimelapse"
        class="bg-gray-500 hover:bg-gray-600 text-white font-bold py-3 px-6 rounded-lg shadow-lg transform hover:scale-110 transition-transform duration-300">
        Reset Time Lapse
      </button>
      <button @click="reset"
        class="bg-red-500 hover:bg-gray-600 text-white font-bold py-3 px-6 rounded-lg shadow-lg transform hover:scale-110 transition-transform duration-300">
        Reset Counter
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

  </div>
</template>


<script>
export default {
  components: {
  },
  name: "stopwatchPage",
  data() {
    return {
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
        time: '00:00:00'
      },
      ws: null // Menambahkan properti untuk WebSocket
    };
  },
  methods: {
    connect() {
      console.log(this.Ipaddres)
      this.setupWebSocket();
    },
    start() {
      if (this.running) return;
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
    stop() {
      this.running = false;
      this.timeStopped = new Date();
      clearInterval(this.started);
    },
    async reset() {
      this.running = false;
      clearInterval(this.started);
      this.stoppedDuration = 0;
      this.timeBegan = null;
      this.timeStopped = null;
      this.data.time = "00:00:00";
      // this.timelapse.time = "00:00:00";
      //this.sendReset();
    },
    resetTimelapse() {
      this.timelapse.time = "00:00:00";
      this.sendReset();
    },
    finish() {
      this.timelapse.time = this.data.time;
    },
    clockRunning() {
      const currentTime = new Date();
      const timeElapsed = new Date(currentTime - this.timeBegan - this.stoppedDuration);
      // const hour = timeElapsed.getUTCHours();
      const min = timeElapsed.getUTCMinutes();
      const sec = timeElapsed.getUTCSeconds();
      const ms = timeElapsed.getUTCMilliseconds();

      this.data.time =
        // this.zeroPrefix(hour, 2) + ":" +
        this.zeroPrefix(min, 2) + ":" +
        this.zeroPrefix(sec, 2) + ":" +
        this.zeroPrefix(ms, 2);
    },
    zeroPrefix(num, digit) {
      return String(num).padStart(digit, '0'); // Menggunakan padStart untuk menambah nol di depan
    },
    sendReset() {
      if (this.ws && this.ws.readyState === WebSocket.OPEN) {
        this.ws.send('Reset');
      }
    },
    setupWebSocket() {
      this.ws = new WebSocket(`ws://${this.Ipaddres}:81`);
      this.ws.onmessage = (event) => {
        const message = event.data;
        // console.log(message);
        if (message === "Start") {
          this.start();
        } else if (message === "Finish") {
          this.finish();
        }
      };

      this.ws.onopen = () => {
        console.log("WebSocket connection established");
        this.status = 'Connected'
      };

      this.ws.onclose = () => {
        console.log("WebSocket connection closed");
        this.status = 'Not Connect'
        setTimeout(this.setupWebSocket, 1000);
      };

      this.ws.onerror = (error) => {
        console.error("WebSocket error: ", error);
      };
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
