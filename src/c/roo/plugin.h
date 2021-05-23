bool init_carla_backend();
const char **get_lv2_uri_list(uint32_t *count);
void carla_cleanup();
bool add_plugin(const char *plugin_name);