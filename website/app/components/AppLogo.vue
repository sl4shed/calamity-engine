<script setup>
const colorMode = useColorMode();
</script>

<template>
  <img v-if="colorMode.value === 'dark'" src="../assets/short-full-logo.png" />
  <img v-else src="../assets/short-full-logo-light.png">
</template>
