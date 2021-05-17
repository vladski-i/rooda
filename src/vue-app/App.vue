<template>
	<div id="app">
		<img class="vue-logo" src="@/assets/logo.svg" alt="Vue logo" />
		<HelloWorld message="Welcome to Your Vue.js App" />
		<!-- <KnobControl v-model="value"/> -->
		<round-slider v-model="value" 
			:update="updateConfig" 
			:min="2" 
			:max="200"
			start-angle="315"
			end-angle="+270"/>
	</div>
</template>

<script>
/* eslint-disable indent */
/*eslint no-unused-vars: "off"*/
	"use strict";
	const { ipcRenderer } = window.require("electron"); // import the IPC module

	ipcRenderer.on('update-config', (event, arg) => {
		console.log('[Vue] config updated');
	})

	export default {
		name: "App",
		components: {
			HelloWorld: () => import("@/components/HelloWorld.vue"),
			// KnobControl: () => import("vue-knob-control"),
			RoundSlider: () => import("vue-round-slider")
		},
		data() {
			return {
				value : 0
			};
		},
		methods : {
			updateConfig: (v) => {
				console.log(v.value);
				ipcRenderer.send("update-config", {mode : "ZERO_FILL", windowSize: v.value });
				console.log("[Vue] config updated");
				return v;
			}
		}
	};
</script>

<style lang="scss">
	@charset "utf-8";

	@import "@/styles/variables";

	#app {
		font-family: $font-family;
		-webkit-font-smoothing: antialiased;
		-moz-osx-font-smoothing: grayscale;
		text-align: center;
		background-color: $background-color;
		color: $text-color;
		margin-top: 60px;
	}

	img.vue-logo {
		max-width: 200px;
	}
</style>
