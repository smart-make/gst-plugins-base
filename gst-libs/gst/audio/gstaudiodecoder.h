/* GStreamer
 * Copyright (C) 2009 Igalia S.L.
 * Author: Iago Toral Quiroga <itoral@igalia.com>
 * Copyright (C) 2011 Mark Nauwelaerts <mark.nauwelaerts@collabora.co.uk>.
 * Copyright (C) 2011 Nokia Corporation. All rights reserved.
 *   Contact: Stefan Kost <stefan.kost@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#ifndef _GST_AUDIO_DECODER_H_
#define _GST_AUDIO_DECODER_H_

#include <gst/gst.h>
#include <gst/audio/audio.h>
#include <gst/base/gstadapter.h>

G_BEGIN_DECLS

#define GST_TYPE_AUDIO_DECODER \
  (gst_audio_decoder_get_type())
#define GST_AUDIO_DECODER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_AUDIO_DECODER,GstAudioDecoder))
#define GST_AUDIO_DECODER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_AUDIO_DECODER,GstAudioDecoderClass))
#define GST_AUDIO_DECODER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS((obj),GST_TYPE_AUDIO_DECODER,GstAudioDecoderClass))
#define GST_IS_AUDIO_DECODER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_AUDIO_DECODER))
#define GST_IS_AUDIO_DECODER_CLASS(obj) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_AUDIO_DECODER))
#define GST_AUDIO_DECODER_CAST(obj) \
  ((GstAudioDecoder *)(obj))

/**
 * GST_AUDIO_DECODER_SINK_NAME:
 *
 * The name of the templates for the sink pad.
 *
 * Since: 0.10.36
 */
#define GST_AUDIO_DECODER_SINK_NAME    "sink"
/**
 * GST_AUDIO_DECODER_SRC_NAME:
 *
 * The name of the templates for the source pad.
 *
 * Since: 0.10.36
 */
#define GST_AUDIO_DECODER_SRC_NAME     "src"

/**
 * GST_AUDIO_DECODER_SRC_PAD:
 * @obj: base audio codec instance
 *
 * Gives the pointer to the source #GstPad object of the element.
 *
 * Since: 0.10.36
 */
#define GST_AUDIO_DECODER_SRC_PAD(obj)         (((GstAudioDecoder *) (obj))->srcpad)

/**
 * GST_AUDIO_DECODER_SINK_PAD:
 * @obj: base audio codec instance
 *
 * Gives the pointer to the sink #GstPad object of the element.
 *
 * Since: 0.10.36
 */
#define GST_AUDIO_DECODER_SINK_PAD(obj)        (((GstAudioDecoder *) (obj))->sinkpad)

#define GST_AUDIO_DECODER_STREAM_LOCK(dec)   g_rec_mutex_lock (&GST_AUDIO_DECODER (dec)->stream_lock)
#define GST_AUDIO_DECODER_STREAM_UNLOCK(dec) g_rec_mutex_unlock (&GST_AUDIO_DECODER (dec)->stream_lock)

/**
 * GST_AUDIO_DECODER_INPUT_SEGMENT:
 * @obj: base parse instance
 *
 * Gives the input segment of the element.
 *
 * Since: 0.10.36
 */
#define GST_AUDIO_DECODER_INPUT_SEGMENT(obj)   (GST_AUDIO_DECODER_CAST (obj)->input_segment)

/**
 * GST_AUDIO_DECODER_OUTPUT_SEGMENT:
 * @obj: base parse instance
 *
 * Gives the output segment of the element.
 *
 * Since: 0.10.36
 */
#define GST_AUDIO_DECODER_OUTPUT_SEGMENT(obj)   (GST_AUDIO_DECODER_CAST (obj)->output_segment)

typedef struct _GstAudioDecoder GstAudioDecoder;
typedef struct _GstAudioDecoderClass GstAudioDecoderClass;

typedef struct _GstAudioDecoderPrivate GstAudioDecoderPrivate;

/* do not use this one, use macro below */
GstFlowReturn _gst_audio_decoder_error (GstAudioDecoder *dec, gint weight,
                                        GQuark domain, gint code,
                                        gchar *txt, gchar *debug,
                                        const gchar *file, const gchar *function,
                                        gint line);

/**
 * GST_AUDIO_DECODER_ERROR:
 * @el:     the base audio decoder element that generates the error
 * @weight: element defined weight of the error, added to error count
 * @domain: like CORE, LIBRARY, RESOURCE or STREAM (see #gstreamer-GstGError)
 * @code:   error code defined for that domain (see #gstreamer-GstGError)
 * @text:   the message to display (format string and args enclosed in
 *          parentheses)
 * @debug:  debugging information for the message (format string and args
 *          enclosed in parentheses)
 * @ret:    variable to receive return value
 *
 * Utility function that audio decoder elements can use in case they encountered
 * a data processing error that may be fatal for the current "data unit" but
 * need not prevent subsequent decoding.  Such errors are counted and if there
 * are too many, as configured in the context's max_errors, the pipeline will
 * post an error message and the application will be requested to stop further
 * media processing.  Otherwise, it is considered a "glitch" and only a warning
 * is logged. In either case, @ret is set to the proper value to
 * return to upstream/caller (indicating either GST_FLOW_ERROR or GST_FLOW_OK).
 *
 * Since: 0.10.36
 */
#define GST_AUDIO_DECODER_ERROR(el, weight, domain, code, text, debug, ret) \
G_STMT_START {                                                              \
  gchar *__txt = _gst_element_error_printf text;                            \
  gchar *__dbg = _gst_element_error_printf debug;                           \
  GstAudioDecoder *dec = GST_AUDIO_DECODER (el);                   \
  ret = _gst_audio_decoder_error (dec, weight, GST_ ## domain ## _ERROR,    \
      GST_ ## domain ## _ERROR_ ## code, __txt, __dbg, __FILE__,            \
      GST_FUNCTION, __LINE__);                                              \
} G_STMT_END


/**
 * GST_AUDIO_DECODER_MAX_ERRORS:
 *
 * Default maximum number of errors tolerated before signaling error.
 *
 * Since: 0.10.36
 */
#define GST_AUDIO_DECODER_MAX_ERRORS     10

/**
 * GstAudioDecoder:
 *
 * The opaque #GstAudioDecoder data structure.
 *
 * Since: 0.10.36
 */
struct _GstAudioDecoder
{
  GstElement element;

  /*< protected >*/
  /* source and sink pads */
  GstPad         *sinkpad;
  GstPad         *srcpad;

  /* protects all data processing, i.e. is locked
   * in the chain function, finish_frame and when
   * processing serialized events */
  GRecMutex       stream_lock;

  /* MT-protected (with STREAM_LOCK) */
  GstSegment      input_segment;
  GstSegment      output_segment;

  /*< private >*/
  GstAudioDecoderPrivate *priv;

  gpointer       _gst_reserved[GST_PADDING_LARGE];
};

/**
 * GstAudioDecoderClass:
 * @element_class:  The parent class structure
 * @start:          Optional.
 *                  Called when the element starts processing.
 *                  Allows opening external resources.
 * @stop:           Optional.
 *                  Called when the element stops processing.
 *                  Allows closing external resources.
 * @set_format:     Notifies subclass of incoming data format (caps).
 * @parse:          Optional.
 *                  Allows chopping incoming data into manageable units (frames)
 *                  for subsequent decoding.  This division is at subclass
 *                  discretion and may or may not correspond to 1 (or more)
 *                  frames as defined by audio format.
 * @handle_frame:   Provides input data (or NULL to clear any remaining data)
 *                  to subclass.  Input data ref management is performed by
 *                  base class, subclass should not care or intervene,
 *                  and input data is only valid until next call to base class,
 *                  most notably a call to gst_audio_decoder_finish_frame().
 * @flush:          Optional.
 *                  Instructs subclass to clear any codec caches and discard
 *                  any pending samples and not yet returned decoded data.
 *                  @hard indicates whether a FLUSH is being processed,
 *                  or otherwise a DISCONT (or conceptually similar).
 * @sink_event:     Optional.
 *                  Event handler on the sink pad. Subclasses should chain up to
 *                  the parent implementation to invoke the default handler.
 * @src_event:      Optional.
 *                  Event handler on the src pad. Subclasses should chain up to
 *                  the parent implementation to invoke the default handler.
 * @pre_push:       Optional.
 *                  Called just prior to pushing (encoded data) buffer downstream.
 *                  Subclass has full discretionary access to buffer,
 *                  and a not OK flow return will abort downstream pushing.
 * @open:           Optional.
 *                  Called when the element changes to GST_STATE_READY.
 *                  Allows opening external resources. Since: 0.10.37.
 * @close:          Optional.
 *                  Called when the element changes to GST_STATE_NULL.
 *                  Allows closing external resources. Since: 0.10.37.
 *
 * Subclasses can override any of the available virtual methods or not, as
 * needed. At minimum @handle_frame (and likely @set_format) needs to be
 * overridden.
 *
 * Since: 0.10.36
 */
struct _GstAudioDecoderClass
{
  GstElementClass element_class;

  /*< public >*/
  /* virtual methods for subclasses */

  gboolean      (*start)              (GstAudioDecoder *dec);

  gboolean      (*stop)               (GstAudioDecoder *dec);

  gboolean      (*set_format)         (GstAudioDecoder *dec,
                                       GstCaps *caps);

  GstFlowReturn (*parse)              (GstAudioDecoder *dec,
                                       GstAdapter *adapter,
                                       gint *offset, gint *length);

  GstFlowReturn (*handle_frame)       (GstAudioDecoder *dec,
                                       GstBuffer *buffer);

  void          (*flush)              (GstAudioDecoder *dec, gboolean hard);

  GstFlowReturn (*pre_push)           (GstAudioDecoder *dec,
                                       GstBuffer **buffer);

  gboolean      (*sink_event)         (GstAudioDecoder *dec,
                                       GstEvent *event);
  gboolean      (*src_event)          (GstAudioDecoder *dec,
                                       GstEvent *event);

  gboolean      (*open)               (GstAudioDecoder *dec);
  
  gboolean      (*close)              (GstAudioDecoder *dec);

  /*< private >*/
  gpointer       _gst_reserved[GST_PADDING_LARGE-2];
};

GType             gst_audio_decoder_get_type (void);

gboolean          gst_audio_decoder_set_output_format  (GstAudioDecoder    * dec,
                                                        const GstAudioInfo * info);

GstFlowReturn     gst_audio_decoder_finish_frame (GstAudioDecoder * dec,
                                                  GstBuffer * buf, gint frames);

/* context parameters */
GstAudioInfo    * gst_audio_decoder_get_audio_info (GstAudioDecoder * dec);

void              gst_audio_decoder_set_plc_aware  (GstAudioDecoder * dec,
                                                    gboolean          plc);

gint              gst_audio_decoder_get_plc_aware  (GstAudioDecoder * dec);

void              gst_audio_decoder_set_estimate_rate  (GstAudioDecoder * dec,
                                                    gboolean          enabled);

gint              gst_audio_decoder_get_estimate_rate  (GstAudioDecoder * dec);

gint              gst_audio_decoder_get_delay      (GstAudioDecoder * dec);

void              gst_audio_decoder_set_max_errors (GstAudioDecoder * dec,
                                                   gint               num);

gint              gst_audio_decoder_get_max_errors (GstAudioDecoder * dec);

void              gst_audio_decoder_set_latency (GstAudioDecoder * dec,
                                                 GstClockTime      min,
                                                 GstClockTime      max);

void              gst_audio_decoder_get_latency (GstAudioDecoder * dec,
                                                 GstClockTime    * min,
                                                 GstClockTime    * max);

void              gst_audio_decoder_get_parse_state (GstAudioDecoder * dec,
                                                     gboolean        * sync,
                                                     gboolean        * eos);


/* object properties */
void              gst_audio_decoder_set_plc (GstAudioDecoder * dec,
                                             gboolean          enabled);

gboolean          gst_audio_decoder_get_plc (GstAudioDecoder * dec);

void              gst_audio_decoder_set_min_latency (GstAudioDecoder * dec,
                                                     gint64            num);

gint64            gst_audio_decoder_get_min_latency (GstAudioDecoder * dec);

void              gst_audio_decoder_set_tolerance   (GstAudioDecoder * dec,
                                                     gint64            tolerance);

gint64            gst_audio_decoder_get_tolerance   (GstAudioDecoder * dec);

void              gst_audio_decoder_set_drainable (GstAudioDecoder * dec,
                                                   gboolean enabled);

gboolean          gst_audio_decoder_get_drainable (GstAudioDecoder * dec);

void              gst_audio_decoder_set_needs_format (GstAudioDecoder * dec,
                                                      gboolean enabled);

gboolean          gst_audio_decoder_get_needs_format (GstAudioDecoder * dec);

void              gst_audio_decoder_merge_tags (GstAudioDecoder * dec,
                                                const GstTagList * tags, GstTagMergeMode mode);

G_END_DECLS

#endif /* _GST_AUDIO_DECODER_H_ */
