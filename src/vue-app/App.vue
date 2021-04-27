<template>
	<img class="vue-logo" src="@/assets/logo.svg" alt="Vue logo" />
	<HelloWorld message="Hello Vue 3.0 + Webpack" />
</template>

<script>
/* eslint-disable indent */

	"use strict";

	import HelloWorld from "@/components/HelloWorld";
	// const { ipcRenderer } = window.require("electron"); // import the IPC module
	// console.log("Main said: " + JSON.stringify(ipcRenderer.sendSync("synchronous-message", "ping"))); // Send synchronous message to main(electron) process

	export default {
		components: { HelloWorld }
	};
</script>

<style lang="scss">
	@charset "utf-8";

	@import "@/styles/variables";

	.vue-logo {
		max-width: 10rem;
	}
</style>
