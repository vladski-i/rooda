# Rooda

Simple application built with [Vue](https://github.com/vuejs/vue), [Electron](https://github.com/electron/electron) and [Webpack](https://github.com/webpack/webpack).  
Boilerplate: [electron-vue-boilerplate](https://github.com/oliverfindl/electron-vue-boilerplate)
---

## Whats under the hood?

- Simple [Vue](https://github.com/vuejs/vue) app ([vue-router](https://github.com/vuejs/vue-router) and [vuex](https://github.com/vuejs/vuex) can be additionally installed).
- [Electron](https://github.com/electron/electron) with [electron-builder](https://github.com/electron-userland/electron-builder) package for building installers.
- [Webpack](https://github.com/webpack/webpack) server for [development](https://github.com/webpack/webpack-dev-server) with HMR enabled.
- Support for [Sass](https://github.com/sass/sass) out of box thanks to [node-sass](https://github.com/sass/node-sass) package.
- A C module made for [Jack](https://github.com/jackaudio/jack2) operations

## Install

```bash
# Clone repository from GitHub to <directory>
$ git clone git@github.com:RaduPirosca/rooda.git <directory>

# Switch to <directory>
$ cd <directory>

# Install all dependencies
$ yarn install

# Configure node-gyp
$ node-gyp configure

# Build the C module for electron
$ HOME=~/.electron-gyp node-gyp rebuild --target=11.0.0 --arch=x64 --dist-url=https://electronjs.org/headers

# [optional] Check and update all dependencies @Radu don't do this
$ yarn outdated
$ yarn update
```

## Usage

``` bash
# [terminal 1] Launch Webpack development server with Vue app loaded
$ yarn dev

# [terminal 2] Launch Electron app connected to Webpack development server
$ yarn app

# [terminal 3] Build installers for your current platform
$ yarn dist
```

## Scripts

```bash
# Alias for watch:vue-app
$ yarn dev

# Alias for start
$ yarn app

# Alias for build:electron-app and start:electron-app
$ yarn start

# Launch Electron app connected to Webpack development server
$ yarn start:electron-app

# Build Electron app and Vue app
$ yarn build

# Build Electron app with DevTools disabled
$ yarn build:electron-app

# Build electron app with DevTools enabled
$ yarn build:electron-app-dev

# Copy Electron app assets
$ yarn copy:electron-app-assets

# Build Vue app
$ yarn build:vue-app

# Launch Webpack development server with Vue app loaded
$ yarn watch:vue-app

# Pack Electron app (for development purposes only)
$ yarn pack

# Build installers for your current platform
$ yarn dist

# Build installers for all platforms
$ yarn dist:all

# Build installers for Windows
$ yarn dist:w

# Build installers for macOS
$ yarn dist:m

# Build installers for Linux
$ yarn dist:l

# [internal]
$ yarn postinstall
```

## App structure

```bash
.
├── binding.gyp # for building the C module
├── extract_linux.sh
├── LICENSE
├── package.json
├── postcss.config.js
├── README.md
├── src 
│   ├── c # Sources for the C module
│   │   ├── hello.c
│   │   ├── hello.h
│   │   └── hello_node.c
│   ├── electron-app # home directory for electron process
│   │   ├── assets
│   │   │   └── icon.png
│   │   └── main.js
│   └── vue-app # home directory for renderer process (vue)
│       ├── App.vue
│       ├── assets
│       │   └── logo.svg
│       ├── components
│       │   └── HelloWorld.vue
│       ├── index.html
│       ├── index.scss
│       ├── main.js
│       └── styles
│           ├── mixins.scss
│           └── variables.scss
├── stylelint.config.js
├── svgo.config.js
├── webpack-electron-app.config.js # webpack configs for electron
├── webpack-vue-app.config.js # webpack configs for renderer(vue)
└── yarn.lock
```

---