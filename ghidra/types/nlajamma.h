/*
 * nlajamma (the library's own name: "nlajamma Ver 1.01 Build:Apr  5 2004")
 * talks to the JAMMA / JVS I/O boards. Types as the game's code uses them,
 * applied by ghidra/ApplyTypes.java; names are ours, field offsets and
 * sizes are what the code reads and writes (each noted).
 */

/* A cursor over a reply and the commands that produced it, handed to each
 * reply handler (jvs_index_replies fills reply_left +0, cmd +8, reply +0xC;
 * jvs_reply_reset steps cmd and cmd_left +4). */
typedef struct jvs_reply_cursor {
  int reply_left;          /* reply bytes still to parse */
  int cmd_left;            /* command bytes still to parse */
  unsigned char *cmd;      /* next command byte */
  unsigned char *reply;    /* next reply byte */
} jvs_reply_cursor;

/* jvs_reply_handlers entry: parses one command's part of a reply and moves
 * the cursor past it; returns the report byte (1 = ok). */
typedef int (*jvs_reply_handler)(jvs_reply_cursor *c);

/* What a node reported about itself (0xAC bytes: jvs_nodes is indexed
 * node * 0xAC; the flags at +0x88 / +0x8C are set by the reply checks,
 * players / switches / switch bytes at +0x94, coin slots +0x97, analog
 * channels +0x98, as legacy_io_setup and the readers use them). */
typedef struct jvs_node_desc {
  int           node;
  char          id[128];            /* reply to command 0x10 */
  char          cmd_format_version; /* 0x11 */
  char          jvs_version;        /* 0x12 */
  char          comm_version;       /* 0x13 */
  int           reply_error;        /* set when a reply is missing or bad */
  int           report_error;       /* set when a reply cannot be parsed */
  int           coin_error;
  unsigned char switches[3];        /* players, switches per player, bytes per player */
  unsigned char coin_slots;
  unsigned char analog_channels;
  unsigned char rotary_channels;
  unsigned char keycode;
  unsigned char screen[3];
  unsigned char card;
  unsigned char hopper;
  unsigned char driver;
  unsigned char analog_out;
  unsigned char character[2];
  unsigned char backup;
  unsigned int  (*read)(void);
} jvs_node_desc;

/* One reply packet as the transport layer hands it over (the checks read
 * +2 from_node, +3 ack_status, +4 packet_len, +5 sync (0xE0), +7 length,
 * +8 status, +9 report; replies to two nodes follow each other at
 * packet_len + 3). */
typedef struct jvs_reply_packet {
  char request_id;
  char sense;
  char from_node;
  char ack_status;         /* 0 = ok */
  char packet_len;
  char sync;               /* 0xE0 */
  char node;
  char length;
  char status;             /* 1 = ok */
  char report;             /* first report byte; the data follows */
  char data;
} jvs_reply_packet;
