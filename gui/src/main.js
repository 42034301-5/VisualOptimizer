import Vue from 'vue'
import ElementUI from 'element-ui'
import 'element-ui/lib/theme-chalk/index.css'
Vue.use(ElementUI);
Vue.config.productionTip = false

import router from '@/route'
import GraphView from '@/views/GraphView.vue'

new Vue({
    router,
    render: h => h(GraphView),
}).$mount('#app')
