"use strict";

module.exports = {
	"env": {
		"browser": true,
		"es2021": true
	},
	"extends": [
		"eslint:recommended",
		"plugin:vue/vue3-essential"
	],
	"parser": "vue-eslint-parser",
	"parserOptions": {
		"ecmaVersion": 12,
		"sourceType": "module",
		"parser": "@babel/eslint-parser",
		"requireConfigFile": false
	},
	"plugins": [
		"vue"
	],
	"rules": {
		"linebreak-style": [
			"error",
			"unix"
		]
	}
};
