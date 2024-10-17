import { createWebHistory, createRouter } from "vue-router";
import Stopwatch from "@/views/Stopwatch.vue";


const routes = [
  {
    path: "/",
    name: "Home",
    component: Stopwatch,
  },
];

const router = createRouter({
  history: createWebHistory(),
  routes,
});

export default router;