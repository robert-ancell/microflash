/*
 * Copyright © 2018 Robert Ancell
 * Author: Robert Ancell <robert.ancell@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version. See http://www.gnu.org/copyleft/gpl.html the full text of the
 * license.
 */

#include <ctype.h>
#include <json-glib/json-glib.h>

#include "mb-file.h"

struct _MbFile
{
    GObject parent_instance;

    GFile *file;
    gboolean loaded;
    JsonObject *header;
};

G_DEFINE_TYPE (MbFile, mb_file, G_TYPE_OBJECT)

void
mb_file_init (MbFile *self)
{
}

static void
mb_file_dispose (GObject *object)
{
    MbFile *self = MB_FILE (object);

    g_clear_object (&self->file);
    g_clear_pointer (&self->header, json_object_unref);

    G_OBJECT_CLASS (mb_file_parent_class)->dispose (object);
}

void
mb_file_class_init (MbFileClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->dispose = mb_file_dispose;
}

MbFile *
mb_file_new (GFile *file)
{
    MbFile *f = g_object_new (mb_file_get_type (), NULL);
    f->file = g_object_ref (file);
    return f;
}

GFile *
mb_file_get_file (MbFile *file)
{
    g_return_val_if_fail (MB_IS_FILE (file), NULL);
    return file->file;
}

static int
decode_nibble (const gchar c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    else if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    else
        return -1;
}

static int
decode_byte (const gchar *text, int offset)
{
    int n0 = decode_nibble (text[offset]);
    if (n0 < 0)
        return -1;
    int n1 = decode_nibble (text[offset + 1]);
    if (n1 < 0)
        return -1;
    return n0 << 4 | n1;
}

static int
decode_integer_be (const gchar *text, int offset, int length)
{
    int value = 0;

    for (int i = 0; i < length; i++) {
        int b = decode_byte (text, offset + i * 2);
        if (b < 0)
            return -1;
        value = (value << 8) + b;
    }
    return value;
}

static int
decode_integer_le (const gchar *text, int offset, int length)
{
    int value = 0;

    for (int i = length - 1; i >= 0; i--) {
        int b = decode_byte (text, offset + i * 2);
        if (b < 0)
            return -1;
        value = (value << 8) + b;
    }
    return value;
}

#define HEADER_LENGTH 9
#define TRAILER_LENGTH 2

static gboolean
decode_hex_file (GFile *file, GCancellable *cancellable, gchar **header, GBytes **source, GError **error)
{
    g_autofree gchar *contents = NULL;
    if (!g_file_load_contents (file, cancellable, &contents, NULL, NULL, error))
        return FALSE;

    g_autofree gchar *json_header = NULL;
    guint32 json_header_address = 0;
    gsize json_header_length = 0;
    g_autofree guint8 *text = NULL;
    guint32 text_address = 0;
    gsize text_length = 0;

    gchar *record = contents;
    guint32 address_offset = 0;
    while (TRUE) {
       while (isspace (*record))
          record++;

       if (record[0] != ':') {
           g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                        "Unexpected character '%c'", isprint (record[0]) ? record[0] : '?');
           return FALSE;
       }
       if (strlen (record) < HEADER_LENGTH) {
           g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                        "No enough space for record");
           return FALSE;
       }
       int byte_count = decode_byte (record, 1);
       int record_length = HEADER_LENGTH + byte_count * 2 + TRAILER_LENGTH;
       if (strlen (record) < record_length) {
           g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                        "No enough space for record");
           return FALSE;
       }
       int address = address_offset + decode_integer_be (record, 3, 2);
       int record_type = decode_byte (record, 7);
       const gchar *record_data = record + HEADER_LENGTH;

       record += record_length;

       switch (record_type) {
       case 0:
           /* PXT file record */
           if (byte_count == 16 && strncmp (record_data, "41140E2FB82FA2BB", 16) == 0) {
               json_header_address = address + 16;
               json_header_length = decode_integer_le (record_data, 16, 2);
               json_header = g_malloc0 (json_header_length + 1);
               text_address = json_header_address + json_header_length;
               text_length = decode_integer_le (record_data, 20, 4);
               text = g_malloc0 (text_length);
           }

           /* PXT file data */
           for (int i = 0; i < byte_count; i++) {
               int byte = decode_byte (record_data, i * 2);
               guint32 a = address + i;
               if (a >= json_header_address && a < json_header_address + json_header_length)
                   json_header[a - json_header_address] = byte;
               if (a >= text_address && a < text_address + text_length)
                   text[a - text_address] = byte;
           }
           break;
       case 1:
           if (header != NULL)
               *header = g_steal_pointer (&json_header);
           if (source != NULL)
               *source = g_bytes_new_take (g_steal_pointer (&text), text_length);
           return TRUE;
       case 4:
           if (byte_count != 2) {
               g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                            "Extended linear address wrong size (%d, expecting 2)", byte_count);
              return FALSE;
           }
           address_offset = decode_integer_be (record_data, 0, 2) << 16;
           break;
       case 5:
           if (byte_count != 4) {
               g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                            "Start linear address wrong size (%d, expecting 4)", byte_count);
               return FALSE;
           }
           //start_address = decode_integer_be (record_data, 0, 4);
           break;
       default:
           g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                        "Unknown record type %d", record_type);
           break;
       }
    }
}

static void
load_header (MbFile *file)
{
    if (file->loaded)
        return;
    file->loaded = TRUE;

    g_autofree gchar *header = NULL;
    g_autoptr(GError) error = NULL;
    if (!decode_hex_file (file->file, NULL, &header, NULL, &error)) {
        g_warning ("Failed to parse HEX file: %s", error->message);
        return;
    }

    g_autoptr(JsonParser) parser = json_parser_new ();
    if (!json_parser_load_from_data (parser, header, -1, &error)) {
        g_warning ("Failed to parse PXT header: %s", error->message);
        return;
    }

    JsonNode *root = json_parser_get_root (parser);
    if (!JSON_NODE_HOLDS_OBJECT (root)) {
        g_warning ("PXT header is not an object");
        return;
    }

    file->header = json_object_ref (json_node_get_object (root));
}

const gchar *
mb_file_get_name (MbFile *file)
{
    g_return_val_if_fail (MB_IS_FILE (file), NULL);
    load_header (file);
    if (file->header == NULL || !json_object_has_member (file->header, "name"))
        return NULL;
    return json_object_get_string_member (file->header, "name");
}
