import Vue from 'vue'
import VueRouter from 'vue-router'
Vue.use(VueRouter)

const originalPush = VueRouter.prototype.push

VueRouter.prototype.push = function push(location) {
    return originalPush.call(this, location).catch(err => err)
}

const routes = [
    {
        path: '/GraphView',
        name: 'GraphView',
        component: () =>import('@/views/GraphView.vue')
    },
    {
        path: '/DAGView',
        name: 'DAGView',
        component: () =>import('@/views/DAGView.vue')
    }
];


const router = new VueRouter({
    routes
});

export default router
