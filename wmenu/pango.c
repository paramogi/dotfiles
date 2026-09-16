#include <cairo/cairo.h>
#include <math.h>
#include <pango/pangocairo.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pango.h"

int get_font_height(const char *fontstr) {
	PangoFontMap *fontmap = pango_cairo_font_map_get_default();
	PangoContext *context = pango_font_map_create_context(fontmap);
	PangoFontDescription *desc = pango_font_description_from_string(fontstr);
	PangoFont *font = pango_font_map_load_font(fontmap, context, desc);
	if (font == NULL) {
		pango_font_description_free(desc);
		g_object_unref(context);
		return -1;
	}
	PangoFontMetrics *metrics = pango_font_get_metrics(font, NULL);
	int height = pango_font_metrics_get_height(metrics) / PANGO_SCALE;
	pango_font_metrics_unref(metrics);
	g_object_unref(font);
	pango_font_description_free(desc);
	g_object_unref(context);
	return height;
}

/* Device scale of the target surface (the output scale). */
static double device_scale(cairo_t *cairo) {
	double sx, sy;
	cairo_surface_get_device_scale(cairo_get_target(cairo), &sx, &sy);
	return sx;
}

/*
 * Build a layout for drawing 1:1 in device pixels.
 * Caller must already have applied cairo_scale(1/ds), so user space == device
 * pixels. PangoCairo copies that CTM into the context and would multiply the
 * fontconfig pixelsize by it (32px -> 16px query), so reset it to identity.
 */
PangoLayout *get_pango_layout(cairo_t *cairo, const char *font,
		const char *text, double scale) {
	PangoLayout *layout = pango_cairo_create_layout(cairo);
	PangoContext *ctx = pango_layout_get_context(layout);

	cairo_font_options_t *fo = cairo_font_options_create();
	cairo_get_font_options(cairo, fo);
	cairo_font_options_set_hint_metrics(fo, CAIRO_HINT_METRICS_ON);
	pango_cairo_context_set_font_options(ctx, fo);
	cairo_font_options_destroy(fo);
	pango_context_set_matrix(ctx, NULL);
	pango_context_set_round_glyph_positions(ctx, TRUE);
	pango_layout_context_changed(layout);

	PangoAttrList *attrs = pango_attr_list_new();
	pango_layout_set_text(layout, text, -1);
	pango_attr_list_insert(attrs, pango_attr_scale_new(scale));
	PangoFontDescription *desc = pango_font_description_from_string(font);
	pango_layout_set_font_description(layout, desc);
	pango_layout_set_single_paragraph_mode(layout, 1);
	pango_layout_set_attributes(layout, attrs);
	pango_font_description_free(desc);
	pango_attr_list_unref(attrs);
	return layout;
}

void get_text_size(cairo_t *cairo, const char *font, int *width, int *height,
		int *baseline, double scale, const char *text) {
	double ds = device_scale(cairo);
	cairo_save(cairo);
	cairo_scale(cairo, 1.0 / ds, 1.0 / ds);
	PangoLayout *layout = get_pango_layout(cairo, font, text, scale * ds);
	int w, h;
	pango_layout_get_pixel_size(layout, &w, &h);
	if (width) {
		*width = ceil(w / ds);
	}
	if (height) {
		*height = ceil(h / ds);
	}
	if (baseline) {
		*baseline = pango_layout_get_baseline(layout) / PANGO_SCALE / ds;
	}
	g_object_unref(layout);
	cairo_restore(cairo);
}

int text_width(cairo_t *cairo, const char *font, const char *text) {
	int text_width;
	get_text_size(cairo, font, &text_width, NULL, NULL, 1, text);
	return text_width;
}

void pango_printf(cairo_t *cairo, const char *font, double scale,
		const char *text) {
	double ds = device_scale(cairo);
	double x, y;
	cairo_get_current_point(cairo, &x, &y);
	cairo_save(cairo);
	cairo_scale(cairo, 1.0 / ds, 1.0 / ds);
	cairo_move_to(cairo, round(x * ds), round(y * ds));
	PangoLayout *layout = get_pango_layout(cairo, font, text, scale * ds);
	pango_cairo_show_layout(cairo, layout);
	g_object_unref(layout);
	cairo_restore(cairo);
}
