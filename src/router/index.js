import { createWebHistory, createRouter } from "vue-router";
import Stopwatch from "@/views/Stopwatch.vue";
import Juri from "@/views/Juri.vue";

const routes = [
  {
    path: "/",
    name: "Home",
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
