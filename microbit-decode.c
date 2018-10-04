#include <ctype.h>
#include <lzma.h>
#include <glib.h>

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

static GBytes *
decode_bytes (const gchar *text, int offset, int length)
{
    guint8 *data;

    data = malloc (length / 2);
    for (int i = 0; i < length; i += 2)
        data[i / 2] = decode_byte (text, offset + i);

    return g_bytes_new_take (data, length / 2);
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
decode_hex_file (const gchar *filename, gchar **header, GBytes **source)
{
    g_autofree gchar *contents = NULL;
    g_autoptr(GError) error = NULL;
    if (!g_file_get_contents (filename, &contents, NULL, &error)) {
        g_printerr ("Failed to load file: %s\n", error->message);
        return FALSE;
    }
   
    g_autofree gchar *json_header = NULL;
    guint32 json_header_address = 0;
    gsize json_header_length = 0;
    g_autofree guint8 *text = NULL;
    guint32 text_address = 0;
    gsize text_length = 0;   

    gchar *record = contents;
    guint32 address_offset = 0;
    guint32 start_address = 0;
    while (TRUE) {
       while (isspace (*record))
          record++;

       if (record[0] != ':') {
           g_printerr ("Unexpected character '%c'\n", isprint (record[0]) ? record[0] : '?');
           return FALSE;
       }
       if (strlen (record) < HEADER_LENGTH) {
           g_printerr ("No enough space for record\n");
           return FALSE;
       }
       int byte_count = decode_byte (record, 1);
       int record_length = HEADER_LENGTH + byte_count * 2 + TRAILER_LENGTH;
       if (strlen (record) < record_length) {
           g_printerr ("No enough space for record\n");
           return FALSE;
       }
       int address = address_offset + decode_integer_be (record, 3, 2);
       int record_type = decode_byte (record, 7);
       const gchar *record_data = record + HEADER_LENGTH;

       record += record_length;

       switch (record_type) {
       case 0:
           //g_print ("DATA %08X '%.*s'\n", address, byte_count * 2, record_data);

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
               g_printerr ("Extended linear address wrong size (%d, expecting 2)\n", byte_count);
              return FALSE;
           }
           address_offset = decode_integer_be (record_data, 0, 2) << 16;
           break;
       case 5:
           if (byte_count != 4) {
               g_printerr ("Start linear address wrong size (%d, expecting 4)\n", byte_count);
               return FALSE;
           }
           start_address = decode_integer_be (record_data, 0, 4);
           //g_print ("START %08X\n", start_address);
           break;
       default:
           g_printerr ("Unknown record type %d\n", record_type);
           break;
       }
    }
}

static GBytes *
lzma_decode (GBytes *input)
{
    lzma_stream stream = LZMA_STREAM_INIT;
    lzma_filter filters[2];
    lzma_options_lzma options;
    lzma_lzma_preset (&options, LZMA_PRESET_DEFAULT);
    filters[0].id = LZMA_FILTER_LZMA1;
    filters[0].options = &options;
    filters[1].id = LZMA_VLI_UNKNOWN;
    lzma_ret ret = lzma_raw_decoder (&stream, filters);
    if (ret != LZMA_OK) {
        g_printerr ("Failed to initialize LZMA decoder\n");
        return NULL;
    }
    gsize data_length;
    const uint8_t *data = g_bytes_get_data (input, &data_length);
    stream.next_in = data;
    stream.avail_in = data_length;
    g_autoptr(GByteArray) output = g_byte_array_sized_new (data_length * 2);
    while (TRUE) {
        guint8 buffer[1024];

        stream.next_out = buffer;
        stream.avail_out = 1024;
        int ret = lzma_code (&stream, LZMA_RUN);
        if (ret == LZMA_STREAM_END) {
            g_byte_array_ref (output);
            return g_byte_array_free_to_bytes (output);
        }
        if (ret != LZMA_OK) {
            g_printerr ("LZMA decoder error %d\n", ret);
            return NULL;
        }
        g_byte_array_append (output, buffer, 1024 - stream.avail_out);
    }
}

int
main (int argc, char **argv)
{
    if (argc < 2) {
        g_printerr ("%s [filename.hex]\n", argv[0]);
        return EXIT_FAILURE;
    }

    g_autofree gchar *header = NULL;
    g_autoptr(GBytes) source = NULL;
    if (!decode_hex_file (argv[1], &header, &source))
        return EXIT_FAILURE;

    g_file_set_contents ("output", g_bytes_get_data (source, NULL), g_bytes_get_size (source), NULL);

    g_autoptr(GBytes) decoded_source = lzma_decode (source);
    if (decoded_source == NULL)
        return EXIT_FAILURE;

    g_print ("%s\n", header);
    g_print ("%.*s\n", (int) g_bytes_get_size (decoded_source), (gchar *) g_bytes_get_data (decoded_source, NULL));

    return EXIT_SUCCESS;
}
