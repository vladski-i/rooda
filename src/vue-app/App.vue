<template>
  <div>
    <Slider v-model="value" class="lb-slider"/>
	<p>Value is {{value}}</p>
	<HelloWorld message="Hello Vue 3.0 + Webpack" />
  </div>
</template>

<script>
	import Slider from '@vueform/slider'
	import HelloWorld from "@/components/HelloWorld";
	import { ref } from "vue";

	export default {
	components: {
	Slider,
	HelloWorld
	},
	setup() {
	const value = ref(0);
	return { value };
	}
	}
</script>

<style src="./styles/slider.css"></style>