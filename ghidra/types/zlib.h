/*
 * zlib 1.1.4 (Jean-loup Gailly and Mark Adler, zlib license), as linked
 * into main.bin: the types of zlib.h, zconf.h, zutil.h, deflate.h,
 * inftrees.h, infutil.h, infcodes.c and inflate.c, applied by
 * ghidra/ApplyTypes.java. Field names and order are the originals.
 *
 * deflate.c and inflate.c each define their own struct internal_state;
 * Ghidra has one type namespace, so they are deflate_state (deflate.h's
 * own typedef name) and inflate_state here, and z_stream.state stays the
 * opaque pointer the public header has. The sizes are checked against
 * the binary: each init function allocates sizeof(...) of its state
 * (deflateInit2_ 0x16B8, inflateInit2_ 0x18, inflate_blocks_new 0x40,
 * inflate_codes_new 0x1C), and 0x38 is the stream_size the game passes
 * with "1.1.4".
 */

/* ---- zconf.h / zutil.h --------------------------------------------- */
typedef unsigned char Byte;
typedef unsigned int uInt;
typedef unsigned long uLong;
typedef Byte Bytef;
typedef char charf;
typedef int intf;
typedef uInt uIntf;
typedef uLong uLongf;
typedef void *voidpf;
typedef void *voidp;
/* uch is unsigned char in zutil.h; here Ghidra's byte, because Ghidra treats
 * unsigned char as a character type and the uch tables (_length_code,
 * _dist_code, bl_order, depth) would decompile as string literals. */
typedef byte uch;
typedef uch uchf;
typedef unsigned short ush;
typedef ush ushf;
typedef unsigned long ulg;

/* ---- zlib.h -------------------------------------------------------- */
typedef voidpf (*alloc_func)(voidpf opaque, uInt items, uInt size);
typedef void (*free_func)(voidpf opaque, voidpf address);

struct internal_state;

typedef struct z_stream_s {
    Bytef    *next_in;   /* next input byte */
    uInt     avail_in;   /* number of bytes available at next_in */
    uLong    total_in;   /* total nb of input bytes read so far */
    Bytef    *next_out;  /* next output byte should be put there */
    uInt     avail_out;  /* remaining free space at next_out */
    uLong    total_out;  /* total nb of bytes output so far */
    char     *msg;       /* last error message, NULL if no error */
    struct internal_state *state; /* not visible by applications */
    alloc_func zalloc;   /* used to allocate the internal state */
    free_func  zfree;    /* used to free the internal state */
    voidpf     opaque;   /* private data object passed to zalloc and zfree */
    int     data_type;   /* best guess about the data type: ascii or binary */
    uLong   adler;       /* adler32 value of the uncompressed data */
    uLong   reserved;    /* reserved for future use */
} z_stream;

typedef z_stream *z_streamp;

/* ---- zutil.h ------------------------------------------------------- */
typedef uLong (*check_func)(uLong check, const Bytef *buf, uInt len);

/* ---- deflate.h ----------------------------------------------------- */
typedef struct ct_data_s {
    union {
        ush  freq;       /* frequency count */
        ush  code;       /* bit string */
    } fc;
    union {
        ush  dad;        /* father node in Huffman tree */
        ush  len;        /* length of bit string */
    } dl;
} ct_data;

typedef struct static_tree_desc_s {
    const ct_data *static_tree;  /* static tree or NULL */
    const intf    *extra_bits;   /* extra bits for each code or NULL */
    int     extra_base;          /* base index for extra_bits */
    int     elems;               /* max number of elements in the tree */
    int     max_length;          /* max bit length for the codes */
} static_tree_desc;

typedef struct tree_desc_s {
    ct_data *dyn_tree;           /* the dynamic tree */
    int     max_code;            /* largest code with non zero frequency */
    static_tree_desc *stat_desc; /* the corresponding static tree */
} tree_desc;

typedef ush Pos;
typedef Pos Posf;
typedef unsigned IPos;

typedef struct deflate_state {
    z_streamp strm;      /* pointer back to this zlib stream */
    int   status;        /* as the name implies */
    Bytef *pending_buf;  /* output still pending */
    ulg   pending_buf_size; /* size of pending_buf */
    Bytef *pending_out;  /* next pending byte to output to the stream */
    int   pending;       /* nb of bytes in the pending buffer */
    int   noheader;      /* suppress zlib header and adler32 */
    Byte  data_type;     /* UNKNOWN, BINARY or ASCII */
    Byte  method;        /* STORED (for zip only) or DEFLATED */
    int   last_flush;    /* value of flush param for previous deflate call */
    uInt  w_size;        /* LZ77 window size (32K by default) */
    uInt  w_bits;        /* log2(w_size)  (8..16) */
    uInt  w_mask;        /* w_size - 1 */
    Bytef *window;
    ulg window_size;
    Posf *prev;
    Posf *head;          /* Heads of the hash chains or NIL. */
    uInt  ins_h;         /* hash index of string to be inserted */
    uInt  hash_size;     /* number of elements in hash table */
    uInt  hash_bits;     /* log2(hash_size) */
    uInt  hash_mask;     /* hash_size-1 */
    uInt  hash_shift;
    long block_start;
    uInt match_length;   /* length of best match */
    IPos prev_match;     /* previous match */
    int match_available; /* set if previous match exists */
    uInt strstart;       /* start of string to insert */
    uInt match_start;    /* start of matching string */
    uInt lookahead;      /* number of valid bytes ahead in window */
    uInt prev_length;
    uInt max_chain_length;
    uInt max_lazy_match; /* also max_insert_length */
    int level;           /* compression level (1..9) */
    int strategy;        /* favor or force Huffman coding*/
    uInt good_match;
    int nice_match;      /* Stop searching when current match exceeds this */
    struct ct_data_s dyn_ltree[573];  /* literal and length tree (HEAP_SIZE) */
    struct ct_data_s dyn_dtree[61];   /* distance tree (2*D_CODES+1) */
    struct ct_data_s bl_tree[39];     /* Huffman tree for bit lengths (2*BL_CODES+1) */
    struct tree_desc_s l_desc;        /* desc. for literal tree */
    struct tree_desc_s d_desc;        /* desc. for distance tree */
    struct tree_desc_s bl_desc;       /* desc. for bit length tree */
    ush bl_count[16];    /* number of codes at each bit length (MAX_BITS+1) */
    int heap[573];       /* heap used to build the Huffman trees (2*L_CODES+1) */
    int heap_len;        /* number of elements in the heap */
    int heap_max;        /* element of largest frequency */
    uch depth[573];      /* Depth of each subtree (2*L_CODES+1) */
    uchf *l_buf;         /* buffer for literals or lengths */
    uInt  lit_bufsize;
    uInt last_lit;       /* running index in l_buf */
    ushf *d_buf;
    ulg opt_len;         /* bit length of current block with optimal trees */
    ulg static_len;      /* bit length of current block with static trees */
    uInt matches;        /* number of string matches in current block */
    int last_eob_len;    /* bit length of EOB code for last block */
    ush bi_buf;
    int bi_valid;
} deflate_state;

/* deflate.c */
typedef enum block_state {
    need_more,      /* block not completed, need more input or more output */
    block_done,     /* block flush performed */
    finish_started, /* finish started, need only more output at next deflate */
    finish_done     /* finish done, accept no more input or output */
} block_state;

typedef block_state (*compress_func)(deflate_state *s, int flush);

typedef struct config_s {
   ush good_length; /* reduce lazy search above this match length */
   ush max_lazy;    /* do not perform lazy search above this match length */
   ush nice_length; /* quit search above this match length */
   ush max_chain;
   compress_func func;
} config;

/* ---- inftrees.h ---------------------------------------------------- */
typedef struct inflate_huft_s {
  union {
    struct {
      Byte Exop;        /* number of extra bits or operation */
      Byte Bits;        /* number of bits in this code or subcode */
    } what;
    uInt pad;           /* pad structure to a power of 2 (4 bytes for */
  } word;               /*  16-bit, 8 bytes for 32-bit int's) */
  uInt base;            /* literal, length base, distance base, or table offset */
} inflate_huft;

/* ---- infcodes.c ---------------------------------------------------- */
typedef enum inflate_codes_mode { /* waiting for "i:"=input, "o:"=output, "x:"=nothing */
      START,    /* x: set up for LEN */
      LEN,      /* i: get length/literal/eob next */
      LENEXT,   /* i: getting length extra (have base) */
      DIST,     /* i: get distance next */
      DISTEXT,  /* i: getting distance extra */
      COPY,     /* o: copying bytes in window, waiting for space */
      LIT,      /* o: got literal, waiting for output space */
      WASH,     /* o: got eob, possibly still output waiting */
      C_END,    /* END in the source (infcodes.c's own enum; the game has a debug
                   enum with an END): x: got eob and all data flushed */
      BADCODE   /* x: got error */
} inflate_codes_mode;

typedef struct inflate_codes_state {
  inflate_codes_mode mode;      /* current inflate_codes mode */
  uInt len;
  union {
    struct {
      inflate_huft *tree;       /* pointer into tree */
      uInt need;                /* bits needed */
    } code;             /* if LEN or DIST, where in tree */
    uInt lit;           /* if LIT, literal */
    struct {
      uInt get;                 /* bits to get for extra */
      uInt dist;                /* distance back to copy from */
    } copy;             /* if EXT or COPY, where and how much */
  } sub;                /* submode */
  Byte lbits;           /* ltree bits decoded per branch */
  Byte dbits;           /* dtree bits decoder per branch */
  inflate_huft *ltree;          /* literal/length/eob tree */
  inflate_huft *dtree;          /* distance tree */
} inflate_codes_state;

/* ---- infutil.h ----------------------------------------------------- */
typedef enum inflate_block_mode {
      TYPE,     /* get type bits (3, including end bit) */
      LENS,     /* get lengths for stored */
      STORED,   /* processing stored block */
      TABLE,    /* get table lengths */
      BTREE,    /* get bit lengths tree for a dynamic block */
      DTREE,    /* get length, distance trees for a dynamic block */
      CODES,    /* processing fixed or dynamic block */
      DRY,      /* output remaining window bytes */
      DONE,     /* finished last block, done */
      BAD       /* got a data error--stuck here */
} inflate_block_mode;

typedef struct inflate_blocks_state {
  inflate_block_mode  mode;     /* current inflate_block mode */
  union {
    uInt left;          /* if STORED, bytes left to copy */
    struct {
      uInt table;               /* table lengths (14 bits) */
      uInt index;               /* index into blens (or border) */
      uIntf *blens;             /* bit lengths of codes */
      uInt bb;                  /* bit length tree depth */
      inflate_huft *tb;         /* bit length decoding tree */
    } trees;            /* if DTREE, decoding info for trees */
    struct {
      inflate_codes_state *codes;
    } decode;           /* if CODES, current state */
  } sub;                /* submode */
  uInt last;            /* true if this block is the last block */
  uInt bitk;            /* bits in bit buffer */
  uLong bitb;           /* bit buffer */
  inflate_huft *hufts;  /* single malloc for tree space */
  Bytef *window;        /* sliding window */
  Bytef *end;           /* one byte after sliding window */
  Bytef *read;          /* window read pointer */
  Bytef *write;         /* window write pointer */
  check_func checkfn;   /* check function */
  uLong check;          /* check on output */
} inflate_blocks_state;

/* ---- inflate.c ----------------------------------------------------- */
typedef enum inflate_mode {
      METHOD,   /* waiting for method byte */
      FLAG,     /* waiting for flag byte */
      DICT4,    /* four dictionary check bytes to go */
      DICT3,    /* three dictionary check bytes to go */
      DICT2,    /* two dictionary check bytes to go */
      DICT1,    /* one dictionary check byte to go */
      DICT0,    /* waiting for inflateSetDictionary */
      BLOCKS,   /* decompressing blocks */
      CHECK4,   /* four check bytes to go */
      CHECK3,   /* three check bytes to go */
      CHECK2,   /* two check bytes to go */
      CHECK1,   /* one check byte to go */
      I_DONE,   /* DONE in the source (inflate.c's own enum): finished check, done */
      I_BAD     /* BAD in the source: got an error--stay here */
} inflate_mode;

typedef struct inflate_state {
  inflate_mode  mode;   /* current inflate mode */
  union {
    uInt method;        /* if FLAGS, method byte */
    struct {
      uLong was;                /* computed check value */
      uLong need;               /* stream check value */
    } check;            /* if CHECK, check values to compare */
    uInt marker;        /* if BAD, inflateSync's marker bytes count */
  } sub;        /* submode */
  int  nowrap;          /* flag for no wrapper */
  uInt wbits;           /* log2(window size)  (8..15, defaults to 15) */
  inflate_blocks_state *blocks; /* current inflate_blocks state */
} inflate_state;
