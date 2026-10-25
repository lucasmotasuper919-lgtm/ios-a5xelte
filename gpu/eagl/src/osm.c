// SPDX-License-Identifier: MIT
// Based on Bergamota's gpu/eagl/src/osm.c (ry-diffusion/Bergamota, MIT).
//
// The Galaxy S7's libOSMesa (gpu/mesa: Panfrost over HeroGPU) has no IOSurface import yet, so every
// osm_sweet_* call answers "not available" and eagl.m takes its CPU path: renderbuffers are read back
// with glReadPixels into the IOSurface. The GPU still does all the drawing.
// Mesa's on-disk shader cache is not built (it keys on an ELF build-id): shaders compile once per process.
#include <stddef.h>
#include <GL/osmesa.h>
#include "osm.h"

#define OSMESA_ES_PROFILE 0x36      // gpu/mesa/patch-mesa.py

osm_ctx osm_create(int es_api, osm_ctx share) {
    const int attr[] = {
        OSMESA_FORMAT, OSMESA_RGBA, OSMESA_DEPTH_BITS, 0, OSMESA_STENCIL_BITS, 0,
        OSMESA_PROFILE, OSMESA_ES_PROFILE,
        OSMESA_CONTEXT_MAJOR_VERSION, es_api, OSMESA_CONTEXT_MINOR_VERSION, 0, 0 };
    return (osm_ctx)OSMesaCreateContextAttribs(attr, (OSMesaContext)share);
}
void osm_destroy(osm_ctx c) { OSMesaDestroyContext((OSMesaContext)c); }
int osm_make_current(osm_ctx c, void *buf, int w, int h) {
    if (!c) { OSMesaMakeCurrent(NULL, NULL, GL_UNSIGNED_BYTE, 0, 0); return 1; }
    return OSMesaMakeCurrent((OSMesaContext)c, buf, GL_UNSIGNED_BYTE, w, h) ? 1 : 0;
}
void *osm_getproc(const char *name) { return (void *)OSMesaGetProcAddress(name); }

int osm_sweet_active(void) { return 0; }
int osm_sweet_blit(unsigned name, int is_texture, unsigned surface_id, int flip, int opaque) {
    (void)name; (void)is_texture; (void)surface_id; (void)flip; (void)opaque; return 0;
}
void osm_sweet_forget(unsigned surface_id) { (void)surface_id; }
void osm_sweet_stats(double *out, int n) { for (int i = 0; i < n; i++) out[i] = 0; }
int osm_sweet_attach_direct(unsigned name, unsigned surface_id) { (void)name; (void)surface_id; return 0; }
void osm_sweet_direct_sync(unsigned surface_id) { (void)surface_id; }
int osm_sweet_surface_alias(unsigned surface_id, void **base, unsigned *bpr, unsigned *w, unsigned *h) {
    (void)surface_id; (void)base; (void)bpr; (void)w; (void)h; return 0;
}
void osm_sweet_alias_sync(unsigned surface_id) { (void)surface_id; }
void osm_sweet_zero_texture(void) {}
int osm_sweet_wrap_texture(unsigned name, unsigned surface_id, int kind, unsigned w, unsigned h) {
    (void)name; (void)surface_id; (void)kind; (void)w; (void)h; return 0;
}
void osm_sweet_wrap_sync(unsigned surface_id) { (void)surface_id; }
