{
  "targets": [
    {
      "target_name": "roo",
      "sources": [ "./src/c/roo/roo.c",
       "./src/c/roo/roo_node.c",
        "./src/c/roo/callbacks.c",
        "./src/c/roo/startup.c",
        "./src/c/roo/interface.c",
        "./src/c/roo/log.c",
        "./src/c/roo/plugin.c",
        "./src/c/roo/map.c",
        "./src/c/roo/lane.c"],
      "libraries": ["-ljack", "-lcarla_standalone2", "-llilv-0"],
      "include_dirs": ["/usr/include/carla", "/usr/include/carla/includes"]
    }
  ]
}
