<template>
	<div id="app">
		<img class="vue-logo" src="@/assets/logo.svg" alt="Vue logo" />
		<HelloWorld message="Welcome to Your Vue.js App" />
		<round-slider v-model="value" 
			:update="updateWindowSize" 
			:min="2" 
			:max="200000"
			start-angle="315"
			end-angle="+270"/>
		<select v-model="mode" v-on:change="updateMode(mode)">
			<option>ZERO_FILL</option>
			<option>NO_FILL</option>
			<option>SINE</option>
		</select>
	</div>
</template>

<script>
/* eslint-disable indent */
/* eslint no-unused-vars: "off"*/
/* eslint vue/no-unused-components: "off"*/
	"use strict";
	const { ipcRenderer } = window.require("electron"); // import the IPC module
	import logger from 'console-log-level'
	const log = logger({level: "debug"}) // import logger for easier logging

	ipcRenderer.on('update-config', (event, arg) => {
		log.debug('[Vue] config updated confirmed');
	})

	ipcRenderer.on('instantiate-plugin', (event, arg) => {
		console.log('[Vue] plugin instantiated');
	})

	let plugins = ipcRenderer.sendSync('get-plugin-list');
	ipcRenderer.send('instantiate-plugin', {plugin_name : plugins[0], lane: 1})
	export default {
		name: "App",
		components: {
			HelloWorld: () => import("@/components/HelloWorld.vue"),
			RoundSlider: () => import("vue-round-slider"),
		},
		data() {
			return {
				value : 0,
				mode: "ZERO_FILL"
			};
		},
		methods : {
			updateWindowSize: (v) => {
				log.debug(v.value);
				ipcRenderer.send("update-config", { windowSize: v.value });
				log.debug("[Vue] config updated");
				return v;
			},
			updateMode: (m) => {
				log.debug("[Vue] updating to " + m);
				ipcRenderer.send("update-config", { mode: m });
				log.debug("[Vue] config updated");
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
