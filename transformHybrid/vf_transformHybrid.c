/**
 * @file
 * transformHybrid video filter
 */

#include "libavutil/imgutils.h"
#include "libavutil/internal.h"
#include "libavutil/avassert.h"
#include "avfilter.h"
#include "internal.h"
#include "video.h"

#include "transformHybrid/transformHybrid.h"

static int query_formats(AVFilterContext *ctx)
{
    AVFilterFormats *formats = NULL;
    int fmt;

    for (fmt = 0; av_pix_fmt_desc_get(fmt); fmt++) {
        const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(fmt);
        int ret;
        if (desc->flags & AV_PIX_FMT_FLAG_HWACCEL)
            continue;
        if ((ret = ff_add_format(&formats, fmt)) < 0)
            return ret;
    }
    return ff_set_common_formats(ctx, formats);
}

static int config_props(AVFilterLink *outlink)
{
    // AVFilterContext *ctx = outlink->src;
    AVFilterLink *inlink = outlink->src->inputs[0];

	int outputHeight = (float)inlink->h / 3 * 2;
    int outputWidth = inlink->w + outputHeight / 2;

    outlink->w = outputWidth;
    outlink->h = outputHeight;
    // int p;
    // AVFilterContext *ctx = inlink->dst;
    // EdgeDetectContext *edgedetect = ctx->priv;

    // edgedetect->nb_planes = inlink->format == AV_PIX_FMT_GRAY8 ? 1 : 3;
    // for (p = 0; p < edgedetect->nb_planes; p++) {
    //     struct plane_info *plane = &edgedetect->planes[p];

    //     plane->tmpbuf     = av_malloc(inlink->w * inlink->h);
    //     plane->gradients  = av_calloc(inlink->w * inlink->h, sizeof(*plane->gradients));
    //     plane->directions = av_malloc(inlink->w * inlink->h);
    //     if (!plane->tmpbuf || !plane->gradients || !plane->directions)
    //         return AVERROR(ENOMEM);
    // }

    return 0;
}

static inline void update_plane_sizes(
    AVPixFmtDescriptor* desc,
    int* in_w, int* in_h, int* out_w, int* out_h) {
    *in_w = FF_CEIL_RSHIFT(*in_w, desc->log2_chroma_w);
    *in_h = FF_CEIL_RSHIFT(*in_h, desc->log2_chroma_h);
    *out_w = FF_CEIL_RSHIFT(*out_w, desc->log2_chroma_w);
    *out_h = FF_CEIL_RSHIFT(*out_h, desc->log2_chroma_h);
}

static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterLink *outlink = inlink->dst->outputs[0];

    AVFrame *out = ff_get_video_buffer(outlink, outlink->w, outlink->h);

    if (!out) {
        av_frame_free(&in);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(out, in);

    uint8_t *in_data, *out_data;
    int out_map_plane;
    int in_w, in_h, out_w, out_h;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(outlink->format);
    for( int i = 0; i < av_pix_fmt_count_planes(outlink->format); ++i){
        // av_frame_copy(out, in);
        in_data = in->data[i];
        av_assert1(in_data);
        out_data = out->data[i];

        out_map_plane = (i == 1 || i == 2) ? 1 : 0;

        out_w = outlink->w;
        out_h = outlink->h;
        in_w = inlink->w;
        in_h = inlink->h;

        if (i >= 1) {
            update_plane_sizes(desc, &in_w, &in_h, &out_w, &out_h);
        }
        
        // av_log(NULL, AV_LOG_INFO, "Processing plane %d\n", i);
        transformVideoFrame(
            in_data, 
            out_data,
            in_w,
            in_h,
            in->linesize[i],
            out_w,
            out_h,
            out->linesize[i]);
    }

    av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}

static const AVFilterPad avfilter_vf_transformHybrid_inputs[] = {
    {
        .name         = "default",
        .type         = AVMEDIA_TYPE_VIDEO,
        .filter_frame = filter_frame,
    },
    { NULL }
};

static const AVFilterPad avfilter_vf_transformHybrid_outputs[] = {
    {
        .name = "default",
        .type = AVMEDIA_TYPE_VIDEO,
        .config_props = config_props,
    },
    { NULL }
};

AVFilter ff_vf_transformHybrid = {
    .name        = "transformHybrid",
    .description = NULL_IF_CONFIG_SMALL("TransformHybrid the input equirectangular-layout spherical video to a hybrid layout."),
    .inputs      = avfilter_vf_transformHybrid_inputs,
    .outputs     = avfilter_vf_transformHybrid_outputs,
    .query_formats = query_formats,
};

