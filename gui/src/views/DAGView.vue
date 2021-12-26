<template>
  <div id="main" style="margin-left:5%;margin-right:5%;margin-top:4%;margin-bottom:4%">
    <el-row>
        <el-card class="header-card">
          <el-button-group>
            <el-button type="primary" @click="clickReturn" round>
                返回程序流图
            </el-button>
            <el-button type="primary" @click="readLine" round>
                {{ReadNextButton}}
            </el-button>
            <el-button v-if="isActive" type="primary" @click="simplify" round>
                删除不活跃变量
            </el-button>
          </el-button-group>
        </el-card>
    </el-row>
    <el-row :gutter="5" style="margin-top:5px">
      <el-col :span="12">
        <el-card class="box-card" :body-style="{ padding: '0px' }">
          <div slot="header" class="clearfix">
            <span>基本块变量DAG</span>
          </div>
          <network v-if="hasGraph" class="in-card"
            ref="network"
            :nodes="this.nodes"
            :edges="this.edges"
            :options="options"
          ></network>
          <el-empty class="in-card" description="请点击按钮开始优化代码"></el-empty>
        </el-card>
      </el-col>
      <el-col :span="12">
          <el-row>
            <el-card class="small-card" :body-style="{ padding: '0px' }">
            <div slot="header" class="clearfix">
                <span>基本块传出活跃变量</span>
            </div>
            <mavon-editor class="small-in-card" v-model="active" :toolbarsFlag="false" :subfield="false" defaultOpen="preview"></mavon-editor>
            </el-card>
          </el-row>

          <el-row style="margin-top:5px">
            <el-card class="large-card" :body-style="{ padding: '0px' }">
            <div slot="header" class="clearfix">
                <span>基本块代码</span>
            </div>
            <div id="main">
                <mavon-editor class="large-in-card" v-model="code" :toolbarsFlag="false" :subfield="false" defaultOpen="preview"></mavon-editor>
            </div>
            </el-card>
          </el-row>
      </el-col>
    </el-row>
  </div>
</template>

<script>
import { Network } from "@vue2vis/network";
import "vis-network/styles/vis-network.css";

import { mavonEditor } from "mavon-editor";
import "mavon-editor/dist/css/index.css";

import { RecordActive,Simplify,ReadLine } from "@/api/code.js";

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
                    width: 2,//关系线宽度
                    // title: undefined,
                    // value: 20,
                    // widthConstraint: false,
                    arrows: {//箭头
                        to: {
                            enabled: true,//箭头是否显示、开启
                            scaleFactor: 1,//箭头的大小
                            type: 'arrow',//箭头的类型：circle,bar,arrows
                        },
                    },
                    // // shadow: true,//连接线阴影配置
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
                    length: 400,//关系线线长设置，数字较大最好以100位单位修改可看出差异
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
                //     shape: 'box', //形状
                //     widthConstraint: 250,
                //     margin: 10,
                       borderWidth: 3,//节点边框的宽度,单位为px
                       borderWidthSelected: 5, //节点被选中时边框的宽度，单位为px
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
            code:"",
            currentLine:-1,
            codes:[],
            actives:[],
            active:"",
            ReadNextButton:"开始读取代码",
            hasGraph:false,
            isActive:false
        }
    },
    created:function(){
        let code=String(this.$route.params.code)
        let codes=code.split("\n");
        this.codes=codes.slice(0, codes.length-1);
        if(codes.length>1)
        {
            this.active=codes[codes.length-1];
            let actives=this.active.split(" ");
            console.log(actives)
            if(actives.length>1)
            {
                this.actives=actives.slice(1);
                this.recordActive();
            }
        }
        this.updateCode();
    },
    methods:{
        clear()
        {
            this.nodes=[];
            this.edges=[];
            Simplify().then().catch(error=>{
                console.log(error);
            })
        },
        updateCode()
        {
            if(this.currentLine>=0&&this.currentLine<this.codes.length)
            {
                let str="";
                for(let i=0;i<this.codes.length;i++)
                {
                    if(i===0)
                    {
                        if(this.currentLine===i)
                        {
                            str+="<font color=#0099ff>"+this.codes[i]+"</font> **<--下一句将被优化的代码**"+"\n";
                            str+="```sql\n";
                        }
                        else
                        {
                            str+="```sql\n";
                            str+=this.codes[i]+"\n";
                        }
                    }
                    else if(i===this.codes.length-1)
                    {
                        if(this.currentLine===i)
                        {
                            str+="```\n";
                            str+="<font color=#0099ff>"+this.codes[i]+"</font> **<--下一句将被优化的代码**"+"\n";
                        }
                        else
                        {
                            str+=this.codes[i]+"\n";
                            str+="```\n";
                        }
                    }
                    else
                    {
                        if(i===this.currentLine)
                        {
                            str+="```\n";
                            str+="<font color=#0099ff>"+this.codes[i]+"</font> **<--下一句将被优化的代码**"+"\n";
                            str+="```sql\n";
                        }
                        else
                        {
                            str+=this.codes[i]+"\n";
                        }
                    }
                    
                }
                this.code=str;
                
            }
            else
            {
                let str="```sql\n";
                for(let i=0;i<this.codes.length;i++)
                {
                    str+=this.codes[i]+"\n";
                }
                this.code=str+"\n```";
            }
        },
        recordActive()
        {
            RecordActive(this.actives).then(response=>{
                console.log(response);
            }).catch(error=>{
                console.log(error);
            })
        },
        simplify()
        {
            this.isActive=false;
            Simplify().then(response=>{
                this.nodes=response.data.nodes;
                this.edges=response.data.edges;

            }).catch(error=>{
                console.log(error);
            })
        },
        readLine()
        {
            this.hasGraph=true;
            if(this.currentLine>=0&&this.currentLine<this.codes.length)//按下下一句
            {
                ReadLine(this.codes[this.currentLine]).then(response=>{
                
                    this.nodes=response.data.nodes;
                    this.edges=response.data.edges;

                }).catch(error=>{
                    console.log(error);
                })
                this.currentLine=this.currentLine+1;
                if(this.currentLine===this.codes.length)
                {
                    this.ReadNextButton="结束读取代码";
                }
            }
            else if(this.currentLine<0)//开始
            {
                this.clear();
                this.recordActive();
                this.currentLine=0;
                this.ReadNextButton="读取下一行代码"
                this.isActive=false;
            }
            else if(this.currentLine>=this.codes.length)//结束
            {
                this.currentLine=-1;
                this.ReadNextButton="开始读取代码"
                this.isActive=true;
            }
            this.updateCode();
        },
        clickReturn:function()
        {
            this.clear();
            this.$router.go(-1);
        }

    }
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
.v-note-wrapper {
    min-height: 80px;
}
.el-card{
    height:640px
}
.header-card{
    height:auto
}

.small-card{
    height: 140px;
}

.large-card{
    height: 493px;
}
.in-card{
    height: 580px;
}
.small-in-card{
    height: 80px;

}
.large-in-card{
    height: 433px;
}

</style>
