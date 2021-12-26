<template>
  <div id="main" style="margin-left:5%;margin-right:5%;margin-top:4%;margin-bottom:4%">
      <el-row>
          <el-col :span="24">
              <el-card class="header-card">
                    <el-button-group>
                        <el-button type="primary" @click="fetchCodeGraph" round>加载程序流图</el-button>
                    </el-button-group>
                </el-card>
          </el-col>
      </el-row>
      <el-row :gutter="5" style="margin-top:5px">
            <el-col :span="12">
                <el-card class="box-card" :body-style="{ padding: '0px' }">
                    <div slot="header" class="clearfix">
                        <span>程序流图</span>
                    </div>
                    <network class="in-card"  v-if="hasGraph" ref="network" 
                    :nodes="nodes" :edges="edges" :options="options"
                    :events="['click', 'changed']"
                    @click="clickCodeBlock"></network>
                    <el-empty class="in-card" v-else description="请输入代码并点击上方按钮以生成程序流图"></el-empty>
                </el-card>
            </el-col>
            <el-col :span="12">
                <el-row :gutter="5">
                    <el-col :span="12">
                        <el-card class="box-card" :body-style="{ padding: '0px' }">
                            <div slot="header" class="clearfix">
                                <span>代码编辑器</span>
                            </div>
                            <mavon-editor class="in-card" v-model="value" :toolbars="toolbars" :subfield="false" defaultOpen="edit"></mavon-editor>
                        </el-card>
                    </el-col>
                    <el-col :span="12">
                        <el-card class="box-card" :body-style="{ padding: '0px' }">
                            <div slot="header" class="clearfix">
                                <span>优化后代码</span>
                            </div>
                            <mavon-editor class="in-card" v-model="after" :toolbars="toolbars" :subfield="false" defaultOpen="preview"></mavon-editor>
                        </el-card>
                    </el-col>

                </el-row>

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
ADD     T16 J   N\n\
FAR     V   A   T1\n\
MUL     T17 T16 I\n\
SET     T18 T17 -\n\
ADD     I   I   1\n\
MUL     T2  4   I\n\
MUL     T19 T2 I\n\
SET     T20 5   -\n\
FAR     T3  A   T2\n\
JLT     (8)  T3  V\n\
ADD     T21 I   1\n\
ADD     T22 I   2\n\
SUB     J   J   1\n\
MUL     T4  4   J\n\
ADD     T23 I   3\n\
ADD     T24 I   4\n\
FAR     T5  A   T4\n\
JGT     (16)  T5  V\n\
JGE     (34)  I   J\n\
MUL     T6  4   I\n\
FAR     X   A   T6\n\
MUL     T7  4   I\n\
MUL     T8  4   J\n\
MUL     T22 5   T8\n\
MUL     T13 2   T7\n\
FAR     T9  A   T8\n\
TAR     A   T7  T9\n\
MUL     T10 4   J\n\
TAR     A   T10 X\n\
JMP     (8)  -   -\n\
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
                autoResize: true,
                height: '100%',
                width: '100%',
                locale: 'en', //默认语言
                clickToUse: false, //当配置点击，只有当激活时，它才会对鼠标和触摸事件做出反应
                // configure: {
                //     enabled: false, //打开或关闭配置界面。
                //     filter: 'nodes,edges', //String, Array, Boolean, Function 显示所有选项
                //     container: undefined, //这允许您将配置列表放在另一个HTML容器中，而不是在网络之下。
                //     showButton: true //显示配置器底部的生成选项按钮。
                // },
                edges: {//关系线控制
                     width: 5,//关系线宽度
                    // title: undefined,
                    // value: 20,
                    // widthConstraint: false,
                    arrows: {//箭头
                        to: {
                            enabled: true,//箭头是否显示、开启
                            scaleFactor: 2,//箭头的大小
                            type: 'arrow',//箭头的类型：circle,bar,arrows
                        },
                    },
                    shadow: true,//连接线阴影配置
                    // shadow: {
                    //     enabled: false,
                    //     color: 'rgba(0,0,0,0.5)',
                    //     size: 10,
                    //     x: 5,
                    //     y: 5
                    // },
                    // // smooth: true,//是否显示方向箭头
                    smooth: {
                        enabled: true, //打开和关闭平滑曲线。
                        type: "dynamic", //'dynamic', 'continuous', 'discrete', 'diagonalCross', 'straightCross', 'horizontal', 'vertical', 'curvedCW', 'curvedCCW', 'cubicBezier'
                        roundness: 0.5 //接受范围：(0-1.0)
                    },
                    // font: {
                    //     size: 5, // px
                    //     color: 'green',
                    //     face: 'arial', //标签文本的字体。
                    //     background: 'none',
                    //     strokeWidth: 2, // px 字体边线宽度。作为背景矩形的替代，可以在文本周围画出笔划。当提供高于0的值时，将画出笔划。
                    //     strokeColor: '#ffffff', //字体边线颜色。
                    //     align: 'horizontal', //字体位置。可能的选择：'horizontal','top','middle','bottom'。对齐方式决定标签在边缘上的对齐方式。默认值为horizontal和标签保持水平，不管边缘的方位如何。
                    //     vadjust: 0, //针对垂直位置进行调整。（正是下降）。
                    //     multi: false, //绘制标签的字体（false-基本字体绘制的纯文本，true或“HTML”-标签可以是多字体的，带有粗体、斜体和代码标记，'markdown'或'md'-多字体的，带有粗体、斜体和代码标记）
                    //     bold: {  //定义标签中粗体字体的样式
                    //         color: '#343434',
                    //         size: 14, // px
                    //         face: 'arial',
                    //         vadjust: 0,
                    //         mod: 'bold'
                    //     },
                    //     ital: { //定义标签中斜体字体的样式。
                    //         color: '#343434',
                    //         size: 14, // px
                    //         face: 'arial',
                    //         vadjust: 0,
                    //         mod: 'italic',
                    //     },
                    //     boldital: { //定义标签中粗体斜体字体的样式
                    //         color: '#343434',
                    //         size: 14, // px
                    //         face: 'arial',
                    //         vadjust: 0,
                    //         mod: 'bold italic'
                    //     },
                    //     mono: { //在标签文本中确定等宽字字体时，添加到字体的字符串和大小。
                    //         color: '#343434',
                    //         size: 15, // px
                    //         face: 'courier new',
                    //         vadjust: 2,
                    //         mod: ''
                    //     }
                    //},
                    // selfReferenceSize: 30, //参考尺寸
                    length: 1200,//关系线线长设置，数字较大最好以100位单位修改可看出差异
                    // dashes: false,//关系线虚线，false不是，true是
                    // arrowStrikethrough: true,//关系线与节点处无缝隙
                    // color: {
                    //     color: 'red',//关系线颜色设置
                    //     highlight: 'black',//点击关系线时的颜色
                    //     hover: '#848484',
                    //     inherit: 'from',
                    //     opacity: 1.0
                    // },
                    // background: {
                    //     enabled: true,
                    //     color: '#ff0000'
                    // },
                    // selectionWidth: 1
                },
                nodes: {//节点配置
                //     shape: 'circle', //形状
                //     widthConstraint: 250,
                //     margin: 10,
                     borderWidth: 3,//节点边框的宽度,单位为px
                     borderWidthSelected: 8, //节点被选中时边框的宽度，单位为px
                //     color: {
                //         border: '#2B7CE9',//节点边框颜色
                //         background: '#97C2FC',//节点背景颜色
                //         highlight: {//节点选中时状态颜色高亮
                //             border: '#2B7CE9',
                //             background: '#D2E5FF'
                //         },
                //         hover: {//节点鼠标滑过时状态颜色
                //             border: '#2B7CE9',
                //             background: '#D2E5FF'
                //         }
                //     },
                     font: {//字体配置
                         color: '#343434',//颜色
                         size: 20, // 大小，单位px
                         face: 'Verdana',//字体
                //         background: 'none',//背景
                         align: 'left',//位置left right center
                     },
                     shadow: true
                 },
                // groups: {},
                // layout: {
                //     randomSeed: 1,//配置每次生成的节点位置都一样，参数为数字1、2等
                //     hierarchical: {
                //         direction: 'UD',//UD:上下 DU:下上 LR:左右 RL:右左
                //         sortMethod: 'directed'
                //     }, //层级结构显示}
                // },
                // physics: {
                //     physics: {
                //         enabled: true, //自由布局或固定布局
                //         forceAtlas2Based: {
                //             gravitationalConstant: -200,
                //             centralGravity: 0.065,
                //             springLength: 150,
                //             springConstant: 1.9,
                //             avoidOverlap: 0.5
                //         },
                //         maxVelocity: 115,
                //         minVelocity: 3.03,
                //         solver: 'forceAtlas2Based',
                //         timestep: 0.16,
                //         stabilization: {
                //             iterations: 2000,
                //             onlyDynamicEdges: false,
                //             fit: true
                //         }
                //     },
                // }
            },
            value:testCode,
            toolbars: {
                bold: false, // 粗体
                italic: false, // 斜体
                header: false, // 标题
                underline: false, // 下划线
                strikethrough: false, // 中划线
                mark: false, // 标记
                superscript: false, // 上角标
                subscript: false, // 下角标
                quote: false, // 引用
                ol: false, // 有序列表
                ul: false, // 无序列表
                link: false, // 链接
                imagelink: false, // 图片链接
                code: false, // code
                table: false, // 表格
                fullscreen: true, // 全屏编辑
                readmodel: true, // 沉浸式阅读
                htmlcode: false, // 展示html源码
                help: false, // 帮助
                /* 1.3.5 */
                undo: true, // 上一步
                redo: true, // 下一步
                trash: true, // 清空
                save: false, // 保存（触发events中的save事件）
                /* 1.4.2 */
                navigation: false, // 导航目录
                /* 2.1.8 */
                alignleft: false, // 左对齐
                aligncenter: false, // 居中
                alignright: false, // 右对齐
                /* 2.2.1 */
                subfield: false, // 单双栏模式
                preview: false, // 预览
            },
            hasGraph:false
        }
    },
    created:function(){

    },
    methods:{
        fetchCodeGraph()
        {
            this.hasGraph=true;

            InitWithCode(this.value).then(response=>{

                this.nodes=response.data.nodes;
                this.edges=response.data.edges;
                this.after="```sql\n"+response.data.value+"\n```";

            }).catch(error=>{
                console.log(error);
            })
            
        },
        clickCodeBlock:function(params)
        {
            if(params.nodes.length>0)
            {
                let index=params.nodes[0];
                var node=this.nodes[index];

                console.log(node);
                console.log(node.label)
                this.$router.push({name:"DAGView",params:{code:node.label}});

            }
        }

    }
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
.el-card{
    height:640px
}
.header-card{
    height:auto
}
.in-card{
    height: 580px;
}


</style>
