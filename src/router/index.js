import { createWebHistory, createRouter } from "vue-router";
import Home from "@/views/Home.vue";
import Stopwatch from "@/views/Stopwatch.vue";
import Juri from "@/views/Juri.vue";

const routes = [
  {
    path: "/",
    name: "Home",
    component: Home,
  },
  {
    path: "/stopwatch",
    name: "Stopwatch",
    component: Stopwatch,
  },
  {
    path: "/juri",
    name: "Juri",
    component: Juri,
  },
];

const router = createRouter({
  history: createWebHistory(),
  routes,
});

export default router;
