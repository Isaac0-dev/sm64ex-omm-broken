// 0x0E000A40
const GeoLayout ttm_geo_000A40[] = {
    GEO_CULLING_RADIUS(900),
    GEO_OPEN_NODE(),
        GEO_RENDER_RANGE(-1900, 5000),
        GEO_OPEN_NODE(),
            GEO_DISPLAY_LIST(LAYER_OPAQUE, ttm_seg7_dl_07011C78),
            GEO_DISPLAY_LIST(LAYER_ALPHA, ttm_seg7_dl_07011D78),
        GEO_CLOSE_NODE(),
    GEO_CLOSE_NODE(),
    GEO_END(),
};
