<template>
    <div id="app">
        <el-row :gutter="20">
            <el-col :span="12">
                <el-card class="box-card">
                    <div slot="header" class="clearfix">
                        <span>节点图</span>
                    </div>
                    <network style="height:500px" ref="network" :nodes="nodes" :edges="edges" :options="options"></network>
                </el-card>

                    
            </el-col>
            <el-col :span="12">
                <el-card class="box-card">
                    <div slot="header" class="clearfix">
                        <span>代码编辑器</span>
                    </div>
                    <div id="main">
                    <mavon-editor style="height:500px" v-model="value" :ishljs="true"></mavon-editor>
                    </div>
                </el-card>

                

                
            </el-col>
        </el-row>
        <el-row :gutter="20">
            <el-col :span="10">
                
            </el-col>
            <el-col :span="10">
                
            </el-col>

        </el-row>
    </div>
</template>

<script>
import Vue from 'vue'
import { Network } from "@vue2vis/network";
import "vis-network/styles/vis-network.css";
import mavonEditor from 'mavon-editor';
import 'mavon-editor/dist/css/index.css'

Vue.use(mavonEditor)

export default {
    name: 'App',
    components: {
        Network
    },
    data() { 
        return { 
            nodes: [ 
                { id: 1, label: "Node 1" },
                { id: 2, label: "Node 2"},
                { id: 3, label: "Node 3" } 
                ], 
                
            edges: [ 
                { id: 1, from: 1, to: 3 }, 
                { id: 2,from: 1, to: 2 } 
                ], 
            options: 
            { 
                nodes: { shape: "circle" } 
            },
            value:"```cpp\n#include<iostream>\nusing namespace std;\nint main(){\nreturn 0;\n}\n```",
            toolbars: {
                bold: true, // 粗体
                italic: true, // 斜体
                header: true, // 标题
                underline: true, // 下划线
                strikethrough: true, // 中划线
                mark: true, // 标记
                superscript: true, // 上角标
                subscript: true, // 下角标
                quote: true, // 引用
                ol: true, // 有序列表
                ul: true, // 无序列表
                link: true, // 链接
                imagelink: true, // 图片链接
                code: true, // code
                table: true, // 表格
                fullscreen: true, // 全屏编辑
                readmodel: true, // 沉浸式阅读
                htmlcode: true, // 展示html源码
                help: true, // 帮助
                /* 1.3.5 */
                undo: true, // 上一步
                redo: true, // 下一步
                trash: true, // 清空
                save: true, // 保存（触发events中的save事件）
                /* 1.4.2 */
                navigation: true, // 导航目录
                /* 2.1.8 */
                alignleft: true, // 左对齐
                aligncenter: true, // 居中
                alignright: true, // 右对齐
                /* 2.2.1 */
                subfield: true, // 单双栏模式
                preview: true, // 预览
            }
        }; 
    }

}
</script>

<style>

</style>
