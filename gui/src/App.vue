<template>
  <div id="app">
      <div class="back" :style="conTop"></div>
      <router-view />
  </div>
</template>

<script>
export default {
  name: "App",
  created:function()
  {
      this.$router.push({name:"GraphView"});
  },
  data()
  {
      return {
          conTop:{
            backgroundImage:'url(' + require('@/assets/background.jpg') + ')',
            backgroundRepeat:'repeat',
            backgroundSize: "cover",
            
          }
      }
  }
};
</script>

<style>
#app {
  font-family: Helvetica,Verdana, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
}

.back{
    position: absolute;
    top:0px;
    height: 1000px;
    width: 100%;
    z-index: -10;
}
</style>