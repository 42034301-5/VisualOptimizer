<template>
  <div id="main">
      <el-row :gutter="20">
            <el-col :span="12">
                <el-card class="box-card">
                    <el-button-group>
                        <el-button type="primary" @click.native="fetchCodeGraph" round>加载程序流图</el-button>
                    </el-button-group>
                </el-card>

                <el-card class="box-card">
                    <div slot="header" class="clearfix">
                        <span>程序流图</span>
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
                        <mavon-editor style="height:500px" v-model="value"></mavon-editor>
                    </div>
                </el-card>
            </el-col>
        </el-row>

    
    

  </div>
</template>

<script>
import { Network } from "@vue2vis/network";
import "vis-network/styles/vis-network.css";

import { mavonEditor } from 'mavon-editor'
import 'mavon-editor/dist/css/index.css'

import {InitWithCode} from "@/api/code.js";


var testCode="\
SUB     I   M   1\n\
SET     J   N   -\n\
MUL     T1  4   N\n\
FAR     V   A   T1\n\
ADD     I   I   1\n\
MUL     T2  4   I\n\
FAR     T3  A   T2\n\
JLT     (5)  T3  V\n\
SUB     J   J   1\n\
MUL     T4  4   J\n\
FAR     T5  A   T4\n\
JGT     (9)  T5  V\n\
JGE     (23)  I   J\n\
MUL     T6  4   I\n\
FAR     X   A   T6\n\
MUL     T7  4   I\n\
MUL     T8  4   J\n\
FAR     T9  A   T8\n\
TAR     A   T7  T9\n\
MUL     T10 4   J\n\
TAR     A   T10 X\n\
JMP     (5)  -   -\n\
MUL     T11 4   I\n\
FAR     X   A   T11\n\
MUL     T12 4   I\n\
MUL     T13 4   N\n\
FAR     T14 A   T13\n\
TAR     A   T12 T14\n\
MUL     T15 4   N\n\
TAR     A   T15 X"

export default {
    components: { Network,mavonEditor},
    props:{
        
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
            value:"```verilog\n"+testCode+"\n```",
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
        }
    },
    created:function(){

    },
    methods:{
        fetchCodeGraph()
        {
            InitWithCode().then(response=>{
                this.nodes=[
                { id: 1, label: "T1" },
                 { id: 2, label: "T2"}
                ];
                this.edges=[
                    { id: 1,from: 1, to: 2 }
                ];
                console.log(response);

            }).catch(error=>{
                console.log(error);
            })
            
        }

    }
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>

</style>
