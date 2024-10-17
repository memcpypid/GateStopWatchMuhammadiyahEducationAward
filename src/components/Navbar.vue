<template>
  <nav class="bg-gradient-to-r from-green-500 via-blue-500 to-gray-600 p-4 shadow-lg">
    <div class="container mx-auto flex items-center justify-between">
      <router-link to="/" class="text-white text-2xl font-bold transition-transform duration-300 hover:scale-105">
        Smart Data Logger
      </router-link>
      <ul class="flex space-x-6">
        <li>
          <router-link
            to="/"
            class="text-white hover:bg-green-700 px-4 py-2 rounded-lg transition-colors duration-300"
            active-class="bg-green-800"
          >
            Home
          </router-link>
        </li>
        <li>
          <router-link
            to="/history"
            class="text-white hover:bg-blue-700 px-4 py-2 rounded-lg transition-colors duration-300"
            active-class="bg-blue-800"
          >
            History
          </router-link>
        </li>
          <button @click="logout" class="flex items-center bottom-0 text-white hover:bg-red-700 px-4 py-2 rounded-lg transition-colors duration-300">
            <svg class="w-5 h-5 mr-2" fill="none" stroke="currentColor" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg">
              <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M16 12H4m12 0l-4-4m4 4l-4 4m8-10a2 2 0 00-2-2H6a2 2 0 00-2 2v12a2 2 0 002 2h12a2 2 0 002-2V6z" />
            </svg>
            Logout
          </button>
      </ul>
    </div>
  </nav>
</template>

<script>
export default {
  name: 'AppNavbar',
  methods: {
    async logout() {
      try {
        await this.$store.dispatch("logout");
        this.$toast.success("Successfully logged out.", { position: "bottom-left", duration: 1000 });
        await this.$router.push("/login");
      } catch (e) {
        console.log(e);
        this.$toast.error("Couldn't log out.", { position: "bottom-left", duration: 1000 });
      }
    },
  }
};
</script>

<style scoped>
/* Custom styling can be added here if needed */
</style>
