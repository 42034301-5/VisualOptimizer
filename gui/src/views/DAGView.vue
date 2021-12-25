<template>
  <div id="main">
    <el-card class="box-card">
      <div slot="header" class="clearfix">
        <span>基本块图</span>
      </div>
      <network
        style="height: 500px"
        ref="network"
        :nodes="nodes"
        :edges="edges"
        :options="options"
      ></network>
    </el-card>
  </div>
</template>

<script>
import { Network } from "@vue2vis/network";
import "vis-network/styles/vis-network.css";

export default {
  components: { Network },
  props: {
    nodes: [
      { id: 1, label: "Node 1" },
      { id: 2, label: "Node 2" },
      { id: 3, label: "Node 3" },
    ],
    edges: [
      { id: 1, from: 1, to: 3 },
      { id: 2, from: 1, to: 2 },
    ],
    options: {
      nodes: { shape: "circle" },
    },
  },
  data() {
    return {};
  },
  created: function () {},
  methods: {},
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped></style>
