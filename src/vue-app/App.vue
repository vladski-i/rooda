<template>
	<div id="app">
		<h1> Rooda </h1>
		<div class="row">
			<div class="col">
				<h3> Balance </h3>
					This controls the proportion of the left and right channel (percentage of the left channel)
			</div>
		</div>
		<div class="row">
			<div class="col">
			<round-slider v-model="balance" 
				:update="updateBalance" 
				:min="0" 
				:max="100"
				start-angle="315"
				end-angle="+270"/>
			</div>
		</div>
		<div class="row">
			<div class="col">
				<h3> Volume </h3>
				<round-slider v-model="volume_value" 
					:update="updateVolume" 
					:min="0" 
					:max="100"
					start-angle="315"
					end-angle="+270"/>
			</div>
		</div>
		<div class="row">
			<div class="col">
				<div>
					<div>
						<button class="add_effect" @click="left_efects.push({id: left_counter, mode: 'ZERO_FILL', is_left: true});
							left_counter += 1;">Add an effect to the left window</button>
					</div>
					<div>
						You have {{left_counter}} effects here
					</div>
				</div>
				<div v-for="selector in left_efects" v-bind:key="selector.id">
					Choose the desired effect:
					<select v-model="selector.mode" v-on:change="updateMode(mode)">
						<option>ZERO_FILL</option>
						<option>NO_FILL</option>
					</select>
					<button class="show_ui" @click="showUI(selector)">Show UI</button>
				</div>
			</div>

			<div class="col">
				<div>
					<div>
						<button class="add_effect" @click="right_efects.push({id: right_counter, mode: 'ZERO_FILL', is_left: false});
							right_counter += 1;">Add an effect to the right window</button>
					</div>
					<div>
						You have {{right_counter}} effects here
					</div>
				</div>
				<div v-for="selector in right_efects" v-bind:key="selector.id">
					Choose the desired effect:
					<select v-model="selector.mode" v-on:change="updateMode(mode)">
						<option>ZERO_FILL</option>
						<option>NO_FILL</option>
					</select>
					<button class="show_ui" @click="showUI(selector)">Show UI</button>
				</div>
			</div>
		</div>
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
			RoundSlider: () => import("vue-round-slider"),
		},
		data() {
			return {
				value : 0,
				balance: 50,
				volume_value: 20,
				left_counter: 0,
				right_counter: 0,
				left_efects: [],
				right_efects: [],
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
			updateVolume: (v) => {
				log.debug("[Vue] updating volume_value to " + v);
				ipcRenderer.send("update-config", { volume_value: v });
				log.debug("[Vue] config updated");
			},
			updateBalance: (b) => {
				log.debug("[Vue] updating balance to " + b);
				ipcRenderer.send("update-config", { balance: b});
				log.debug("[Vue] config updated");
			},
			showUI: (selector) => {
				log.debug("[Vue] showing ui for selector effect " + selector.mode);
				ipcRenderer.send("show-ui", { id: selector.id, mode: selector.mode, is_left: selector.is_left});
				log.debug("[Vue] ui showed");	
			},
			updateMode: (m) => {
				log.debug("[Vue] updating mode to " + m);
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

	.row {
		display: flex;
		margin-left: auto;
		margin-right: auto;
		text-align: center;
		justify-content: center;
		margin-top: 20px;
	}

	button.add_effect {
	background-color: #7299F8; /* Green */
	border: none;
	color: white;
	padding: 15px 15px;
	border-radius: 10px;
	text-align: center;
	text-decoration: none;
	transition: 0.3s;
	display: inline-block;
	font-size: 16px;
	}

	button.show_ui {
	background-color: #7299F8; /* Green */
	border: none;
	color: white;
	padding: 6px 8px;
	margin-top: 10px;
	border-radius: 10px;
	text-align: center;
	text-decoration: none;
	transition: 0.3s;
	display: inline-block;
	font-size: 16px;
	}

	button:hover {
		background-color: #3c538c;
		border-radius: 0px;
	}

	.col {
		padding-right: 60px;
		h3 {
			margin-bottom: 20px;
		}
	}
</style>
