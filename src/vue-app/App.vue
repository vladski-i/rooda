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
	// const log = window.require('console-log-level')(({level: "info"}))
	import logger from 'console-log-level'
	// const log = window.require('electron').remote.require('console-log-level')
	const log = logger({level: "info"})
	console.log(log)

	ipcRenderer.on('update-config', (event, arg) => {
		log.debug('[Vue] config updated confirmed');
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
				log.debug(v.value);
				ipcRenderer.send("update-config", { windowSize: v.value });
				log.debug("[Vue] config updated");
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
