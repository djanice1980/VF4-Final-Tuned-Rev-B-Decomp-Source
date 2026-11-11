/*
 * Recovered data structures, applied by ghidra/ApplyTypes.java.
 * Every field says where it comes from. Names in quotes are Sega's (from
 * assert text or debug strings); others are inferred from how code uses
 * the field. Parsed by Ghidra's C parser: plain C, no preprocessor.
 */

/* ---- vectors ------------------------------------------------------- */

/* Three floats, copied and passed as a whole (12 bytes): the game copies
 * float data word by word with mov.l where C assigns a struct. */
typedef struct vec3 {
    float x;
    float y;
    float z;
} vec3;

/* A direction and a point, passed by value on the stack (24 bytes):
 * sand_puffs_emit's fifth argument. 0c0c7b40 builds d as the difference of
 * two points and copies p from the second. */
typedef struct ray3 {
    vec3 d;
    vec3 p;
} ray3;

/* A quotient and remainder returned together in r0:r1 (8 bytes): what
 * div returns (the C library's integer division with remainder);
 * validate_structure (0c1597c0) reads both halves. */
typedef struct div_t {
    int quot;
    int rem;
} div_t;

/* One row of a camera perspective table (16 bytes): replay_cam_pers_from_dist walks the
 * rows until tag is -1 or lo <= distance < hi, then eases the camera's
 * perspective value toward val over 30 frames. */
typedef struct pers_range {
    char  tag;   /* 0, or -1 on the terminating row */
    float lo;    /* distance range, inclusive below */
    float hi;    /* exclusive above (1000 on the last row) */
    float val;   /* the perspective value for that range */
} pers_range;

/* ---- vf4lib/libbase/src/mode.c ------------------------------------- */

/* A main- or sub-mode handler; mode_exec() calls them every frame. */
typedef void (*mode_handler)(void);

/* One state of the mode state machine (12 bytes). */
typedef struct mode_func {
    int   mode_asg;      /* "mode_asg" (assert mode.c:484): its own index    */
    char *name;          /* debug name; mode_update() prints "%s.%s"         */
    mode_handler func;   /* handler; mode_exec() calls sub, then main        */
} mode_func;

/* One main mode (0xCC bytes): its own handler, then its sub-modes. */
typedef struct mode_data_t {
    mode_func main_func;     /* "main_func" (assert mode.c:484)             */
    mode_func sub_func[16];  /* "sub_func" (assert mode.c:492); 16 fill 0xCC */
} mode_data_t;

/* per side, non-zero on a frame the side's coins or credits went up. */
typedef struct coin_added_sides {
    char side0;
    char side1;
} coin_added_sides;

/* per side, last frame's coin count from credit_copy. */
typedef struct coin_count_prev_sides {
    char side0;
    char side1;
} coin_count_prev_sides;

/* per side, last frame's credits from credit_copy. */
typedef struct credit_count_prev_sides {
    char side0;
    char side1;
} credit_count_prev_sides;

/* per side, rounds won in the current match. */
typedef struct rounds_won_sides {
    char side0;
    char side1;
} rounds_won_sides;

/* per side, the chosen costume (0 / 1) as an int. */
typedef struct side_costume_sides {
    undefined4 side0;
    undefined4 side1;
} side_costume_sides;

/* the game's copy of the credit status (credit_snapshot): four 16-byte rows {credits, coins, coins per credit, state} and two mode words. */
typedef struct credit_copy_rec {
    undefined field_0x0;
    undefined field_0x1;
    undefined field_0x2;
    undefined field_0x3;
    undefined field_0x4;
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
    undefined field_0x8;
    undefined field_0x9;
    undefined field_0xa;
    undefined field_0xb;
    undefined field_0xc;
    undefined field_0xd;
    undefined field_0xe;
    undefined field_0xf;
    undefined field_0x10;
    undefined field_0x11;
    undefined field_0x12;
    undefined field_0x13;
    undefined field_0x14;
    undefined field_0x15;
    undefined field_0x16;
    undefined field_0x17;
    undefined field_0x18;
    undefined field_0x19;
    undefined field_0x1a;
    undefined field_0x1b;
    undefined field_0x1c;
    undefined field_0x1d;
    undefined field_0x1e;
    undefined field_0x1f;
    undefined field_0x20;
    undefined field_0x21;
    undefined field_0x22;
    undefined field_0x23;
    undefined field_0x24;
    undefined field_0x25;
    undefined field_0x26;
    undefined field_0x27;
    undefined field_0x28;
    undefined field_0x29;
    undefined field_0x2a;
    undefined field_0x2b;
    undefined field_0x2c;
    undefined field_0x2d;
    undefined field_0x2e;
    undefined field_0x2f;
    undefined field_0x30;
    undefined field_0x31;
    undefined field_0x32;
    undefined field_0x33;
    undefined field_0x34;
    undefined field_0x35;
    undefined field_0x36;
    undefined field_0x37;
    undefined field_0x38;
    undefined field_0x39;
    undefined field_0x3a;
    undefined field_0x3b;
    undefined field_0x3c;
    undefined field_0x3d;
    undefined field_0x3e;
    undefined field_0x3f;
    undefined4 credit_mode;   /* +0x40: 1 = free play */
    undefined4 separate;      /* +0x44: non-zero: the second side has its own row */
} credit_copy_rec;

/* ---- vf4lib/libbase/src/input.c ------------------------------------ */

/* A button record as button_word_update keeps it: the bits now, the bits
 * the frame before, the bits that turned on and the bits that turned off
 * (input_merged, the per-board records). */
typedef struct button_word {
    uint now;
    uint old;
    uint on;                  /* +0x08: input_merged_pressed                 */
    uint off;                 /* +0x0c                                       */
} button_word;

/* The game's per-board button records, 24 bytes each (button_word and two more words), board 0 at +0 and board 1 at +0x18. */
typedef struct board_buttons_rec {
    undefined4 board0_now;    /* +0x0: board 0 bits now */
    undefined field_0x4;
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
    undefined field_0x8;
    undefined field_0x9;
    undefined field_0xa;
    undefined field_0xb;
    undefined field_0xc;
    undefined field_0xd;
    undefined field_0xe;
    undefined field_0xf;
    undefined field_0x10;
    undefined field_0x11;
    undefined field_0x12;
    undefined field_0x13;
    undefined field_0x14;
    undefined field_0x15;
    undefined field_0x16;
    undefined field_0x17;
    undefined4 board1_now;    /* +0x18: board 1 bits now */
} board_buttons_rec;

/* Per JVS node, frames counted by jvs_node_error_check. */
typedef struct jvs_check_frames_rec {
    undefined4 node0;
    undefined4 node1;
} jvs_check_frames_rec;

/* Per JVS node, frames with any ack error. */
typedef struct jvs_bad_frames_rec {
    undefined4 node0;
    undefined4 node1;
} jvs_bad_frames_rec;

/* Per JVS node, bad frames in a row. */
typedef struct jvs_bad_run_rec {
    undefined4 node0;
    undefined4 node1;
} jvs_bad_run_rec;

/* The frame counts and masks of player_input_tap_hold_repeat: repeat delay and rate, re-press window, long-hold frames, repeat and long-hold masks. */
struct input_repeat_cfg_rec {
    int repeat_delay;
    int repeat_rate;
    int repress_window;
    int long_hold;
    uint repeat_mask;
    uint long_hold_mask;
};

/* Two bytes input_init sets from the checked JVS board count: the count in use and whether there is a board at all. */
typedef struct jvs_board_status_rec {
    undefined1 boards_in_use; /* +0x0: 1 or 2, 0 when the count is out of range */
    undefined1 boards_present;/* +0x1: 1 with one or two boards */
} jvs_board_status_rec;

/* ---- vf4lib/libbase/src/camera.c ----------------------------------- */

/* The running screen shake inside the camera (12 bytes at camera +0x68);
 * camera_shake_step, camera_shake_begin and camera_shake_next_offset get
 * its address. */
typedef struct cam_shake {
    short x;             /* +0x0: offsets the view is turned back by (camera_task_frame) */
    short y;             /* +0x2                                                */
    short field_0x4;     /* +0x4: cleared by camera_reset, not read by name    */
    short step;          /* +0x6: index into the shake table (camera_shake_begin = 0) */
    unsigned char state; /* +0x8: 1 begin, 2 running, other values stop        */
    unsigned char kind;  /* +0x9: table (kind & 3), bit 2 swaps x and y        */
    unsigned char req;   /* +0xa: a pending request (camera_shake_request)     */
    unsigned char req_kind; /* +0xb: its kind; camera_shake_take_request moves both */
} cam_shake;

/* The 16-byte projection block {near, far, field of view, aspect}: the camera's own (camera.proj),
 * the default camera_proj_default (0.1, 1000, 45, 4/3) and the 16 saved ones of camera_proj_stack.
 * camera_proj_set copies one into the camera. */
typedef struct camera_proj_rec {
    float near_clip;     /* camera_reset: 0.1                                   */
    float far_clip;      /* camera_reset: 1000                                  */
    float pers;          /* "PERS": field of view, eased towards pers_req      */
    unsigned int aspect; /* 0x3FAAAAAB, 4/3 as a float; camera_proj_set passes a constant 4/3 */
} camera_proj_rec;

/* The camera record (0x74 bytes, at 0x0C370CE0), run by the "CAMERA" task
 * through camera_mode_funcs[mode]. Names in quotes are the debug menu's.
 * camera_reset and cam_mode_pointstage write every member. */
typedef struct camera_state {
    float field_0x0;     /* +0x0-+0x8: not read by name; zeroed by camera_reset, */
    float field_0x4;     /* +0x8 = 5.0 in cam_mode_pointstage                    */
    float field_0x8;
    vec3 eye;            /* +0xc: the view is a look-at from eye to target (camera_task_frame) */
    vec3 target;         /* +0x18                                               */
    vec3 goal;           /* +0x24: where the target eases to (rob_cam_target_update, cam_mode_replay) */
    unsigned int flags;  /* +0x30: 8 / 0x40 re-place, 0x10 field of view jumps, 0x20 hold;
                            camera_reset sets 0x18, camera_task_frame clears bits 3-4 */
    unsigned int hold_frames; /* +0x34: cam_mode_rob waits this long under flag 0x20 */
    short pitch;         /* +0x38: camera_angles_update, from the eye-target height */
    short yaw;           /* +0x3a: camera_angles_update, atan2 of eye minus target */
    short roll;          /* +0x3c: turned by in camera_task_frame               */
    unsigned char cam_rob_no; /* "CAM_ROB_NO": the rob STAY and SAT follow      */
    char sat_row;        /* +0x3f: row of cam_sat_offsets (cam_mode_sat); the replay steps it 3..5 */
    unsigned char mode;  /* "mode": index of camera_mode_funcs, enum e_mode_0c24318c */
    unsigned char mode_return; /* +0x41: the mode a SoftImage animation ends into */
    char mode_last;      /* +0x42: last frame's mode (camera_mode_update)       */
    unsigned char field_0x43;
    camera_proj_rec proj; /* +0x44-+0x50: the projection block camera_proj_set copies; proj.pers is the menu's "PERS" */
    float pers_req;      /* "PERS_req"                                          */
    float field_0x58;    /* +0x58-+0x64: zeroed by camera_reset, not read by name */
    float field_0x5c;
    float field_0x60;
    float field_0x64;
    cam_shake shake;     /* +0x68                                               */
} camera_state;

/* A copy of the camera eye (camera +0xc) taken when a SoftImage animation starts with flag 2. */
typedef struct cam_anim_start_eye_rec {
    undefined4 x;             /* +0x0: eye x */
    undefined4 y;             /* +0x4: eye y */
    undefined4 z;             /* +0x8: eye z */
} cam_anim_start_eye_rec;

/* A copy of the camera target (camera +0x18) taken when a SoftImage animation starts with flag 2. */
typedef struct cam_anim_start_target_rec {
    undefined4 x;             /* +0x0: target x */
    undefined4 y;             /* +0x4: target y */
    undefined4 z;             /* +0x8: target z */
} cam_anim_start_target_rec;

/* The eight channels of a SoftImage camera animation sampled this frame: eye xyz, target xyz, roll in degrees, field of view (eye y at +4 is not split out in the exported layout). */
typedef struct cam_anim_channels {
    float eye_x;              /* +0x0: channel 0 */
    undefined field_0x4;
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
    float eye_z;              /* +0x8: channel 2 */
    float target_x;           /* +0xc: channel 3 */
    float target_y;           /* +0x10: channel 4 */
    float target_z;           /* +0x14: channel 5 */
    float roll;               /* +0x18: degrees, to the camera roll */
    float fov;                /* +0x1c: to pers and pers_req */
} cam_anim_channels;

/* The eye position of the last SoftImage animation frame, in world space. */
typedef struct cam_anim_eye_rec {
    float x;
    float y;
    float z;
} cam_anim_eye_rec;

/* The target position of the last SoftImage animation frame, in world space. */
typedef struct cam_anim_target_rec {
    float x;
    float y;
    float z;
} cam_anim_target_rec;

/* Start of the rob camera's history block (0x248 bytes): the robs' kept distance, then two rows of 12 entries {rob id, position, +0xcc pair} (24 bytes each, from +4 and +0x124) and a frame counter at +0x244; only the head is laid out here. */
typedef struct rob_cam_history_head {
    float rob_gap;            /* +0x0: distance between the robs (fill: vec3_distance; eye update: ground distance) */
    undefined4 field_0x4;
    undefined4 field_0x8;
    undefined4 field_0xc;
    undefined4 field_0x10;
    undefined4 field_0x14;
    undefined4 field_0x18;
} rob_cam_history_head;

/* 20 bytes inside rob_cam_history from row a entry 10 (+0xfc) to the rob id of its oldest entry 11 (+0x10c). */
typedef struct rob_cam_hist_a_tail_rec {
    undefined field_0x0;
    undefined field_0x1;
    undefined field_0x2;
    undefined field_0x3;
    undefined field_0x4;
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
    undefined field_0x8;
    undefined field_0x9;
    undefined field_0xa;
    undefined field_0xb;
    undefined field_0xc;
    undefined field_0xd;
    undefined field_0xe;
    undefined field_0xf;
    undefined4 oldest_id;     /* +0x10: rob id of row a entry 11 */
} rob_cam_hist_a_tail_rec;

/* The position of row a's oldest entry (11) of rob_cam_history (+0x110). */
typedef struct rob_cam_hist_a_pos_rec {
    float x;
    undefined field_0x4;
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
    float z;
} rob_cam_hist_a_pos_rec;

/* The position of row b's oldest entry (11) of rob_cam_history (+0x230). */
typedef struct rob_cam_hist_b_pos_rec {
    float x;
    undefined field_0x4;
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
    float z;
} rob_cam_hist_b_pos_rec;

/* The last 12 bytes of rob_cam_history: row b entry 11's +0xcc pair (+0x23c) and the hold counter (+0x244). */
typedef struct rob_cam_history_end_rec {
    undefined field_0x0;
    undefined field_0x1;
    undefined field_0x2;
    undefined field_0x3;
    undefined field_0x4;
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
    int hold_frames;          /* +0x8: easing stops from 60 */
} rob_cam_history_end_rec;

/* One entry of the replay camera's history: the robs' midpoint and their distance on one frame. */
typedef struct replay_cam_hist_rec {
    vec3 mid;
    float dist;
} replay_cam_hist_rec;

/* The replay camera's record (0x160 bytes, at 0x0C41EF20): the 20-frame history, the last eye and
 * height choices, the shot's frame count and the perspective and line phases. */
typedef struct replay_cam_rec {
    replay_cam_hist_rec history[20]; /* +0x000: one entry a frame (cam_mode_replay), 20 entries */
    char history_pos;         /* +0x140: the next history slot */
    char last_eye;            /* +0x141: replay_cam_eye_pick's last eye row */
    char last_height;         /* +0x142: its last height column */
    char field_0x143;
    int shot_frames;          /* +0x144: frames of the current shot */
    int pers_phase;           /* +0x148: replay_cam_pers_from_dist's phase (1 find the row, 2 blend, 0 wait) */
    int pers_frames;
    int pers_row;
    int pers_row_last;
    int line_frames;          /* +0x158: frames spent looking along the fighters' line */
    int range_frames;         /* +0x15c: frames too far from or too close to the target */
} replay_cam_rec;

/* ---- vf4lib/libbase/src/console.c ---------------------------------- */

/* One side's network status note (32 bytes, two at net_status): the card
 * request a side sent and the server's answer, shown as an icon while the
 * timer runs (net_status_request_note, net_status_reply_note,
 * net_status_tick, net_status_icons_draw). */
typedef struct net_status_rec {
    char id[17];          /* the card id sent, strncpy 0x11 (net_status_request_note) */
    char command[6];      /* +0x11: the request's command word ("start" / "end")     */
    unsigned char status; /* +0x17: the reply's "db_stat"                             */
    unsigned int active;  /* +0x18: set by the reply, cleared when the timer ends     */
    unsigned int timer;   /* +0x1c: frames left (180 from the reply); dropped below 0 */
} net_status_rec;

/* Where console_frame_draw puts the two players' card icons. */
typedef struct card_icon_pos_rec {
    float p1_x;
    float p1_y;
    float p2_x;
    float p2_y;
} card_icon_pos_rec;

/* The state byte of each of the three game-message animation slots (with mes_anim_timer); 0 off, 1 starts, the drawers step it. */
typedef struct mes_anim_slots {
    undefined1 slot0;
    undefined1 slot1;
    undefined1 slot2;
} mes_anim_slots;

/* Per side, which quest message follows: 0xff none shown yet, 0 shown, 1 the success text is waiting for the send text. */
typedef struct quest_mes_phase_rec {
    undefined1 side0;
    undefined1 side1;
} quest_mes_phase_rec;

/* The card panel's three row offsets on the select screen (6, 25, 42; the middle one at +4 not split out). */
typedef struct card_panel_sel_rows_rec {
    float team_y;             /* +0x0: 6.0 */
    undefined field_0x4;
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
    float kyu_y;              /* +0x8: 42.0 */
} card_panel_sel_rows_rec;

/* The character each side fights with this match, one word per side. */
typedef struct player_chara_pair {
    undefined4 side0;
    undefined4 side1;
} player_chara_pair;

/* Per side, a bit per character whose side texture (char_side_textures) is registered in bank 6. */
typedef struct char_side_tex_mask_rec {
    undefined4 side0;
    undefined4 side1;
} char_side_tex_mask_rec;

/* The prize 2D animation: which one and its frame. */
typedef struct prize_anim_rec {
    undefined4 kind;          /* +0x0: 0-3 */
    float frame;              /* +0x4: 0-180 */
} prize_anim_rec;

/* ---- vf4lib/libnetwork/src/network.c ------------------------------ */

/* The network state block (0x44 bytes, at net_info; network.c's assert
 * names the object and auth_stat). net_init clears it, net_info_get hands
 * it out. nam_step runs the NAM state machine on nam_state; each
 * nam_*_enter routine sets its own wait, or none when its flag asks. */
typedef struct net_info_t {
    unsigned char on;         /* network on: net_init, net_is_on               */
    int error_level;          /* +0x04: error_frames graded 0..3 (net_reply_dispatch:
                                 to 179, to 599, to 1799, above)                */
    int error_frames;         /* +0x08: frames with http_error_code set; cleared by a
                                 reply and by net_auth_result_poll             */
    int busy;                 /* +0x0c: more than 4 requests queued, or level 3 */
    unsigned char hours_open; /* +0x10: net_service_hours_check's open flag     */
    unsigned int service_state; /* +0x14: 0 off, 1 open, 2 finish the VS game, 3 ending
                                 (net_service_state_update, net_service_active) */
    int end_frames;           /* +0x18: state 3 counts this down (0xF0 from
                                 net_game_end_send)                            */
    int nam_state;            /* +0x1c: NAM_NORMAL 0 .. as nam_state_name names them
                                 (nam_state_set; net_init sets 2)              */
    int auth_stat;            /* +0x20: 1 NAS_OK (net_auth_result_store), 2 timed out */
    int timeout;              /* +0x24: a request's time-out, from agg_0c376424 +4 */
    int timeout_left;         /* +0x28: counted down by nam_step                */
    unsigned char http_no_wait;      /* +0x2c: nam_retry_wait_http_enter waits 0 */
    int http_wait;            /* +0x30: else 3600 frames (600 with net_debug_enabled) */
    unsigned char auth_no_wait;      /* +0x34: nam_retry_wait_auth_enter waits 0
                                        (set by net_init and after REQ_HTTP)     */
    int auth_wait;            /* +0x38: else from net_serial_hash               */
    unsigned char game_config_received; /* +0x3c: set at the end of
                                           net_power_on_reply_parse          */
    unsigned char game_init_no_wait;    /* +0x3d: set once auth_stat is NAS_OK */
    int game_init_wait;       /* +0x40: nam_retry_wait_game_init_enter, from
                                 net_serial_hash                               */
} net_info_t;

/* net_game_config from +0x40: the four rare numbers, the syuchu text and its three numbers. */
typedef struct net_rare_syuchu_rec {
    undefined field_0x0;
    undefined field_0x1;
    undefined field_0x2;
    undefined field_0x3;
    undefined field_0x4;
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
    undefined field_0x8;
    undefined field_0x9;
    undefined field_0xa;
    undefined field_0xb;
    undefined field_0xc;
    undefined field_0xd;
    undefined field_0xe;
    undefined field_0xf;
    undefined field_0x10;
    undefined field_0x11;
    undefined field_0x12;
    undefined field_0x13;
    undefined field_0x14;
    undefined field_0x15;
    undefined field_0x16;
    undefined field_0x17;
    undefined field_0x18;
    undefined field_0x19;
    undefined field_0x1a;
    undefined field_0x1b;
    undefined field_0x1c;
    undefined field_0x1d;
    undefined field_0x1e;
    undefined field_0x1f;
    undefined field_0x20;
    undefined field_0x21;
    undefined field_0x22;
    undefined field_0x23;
    undefined field_0x24;
    undefined field_0x25;
    undefined field_0x26;
    undefined field_0x27;
    undefined field_0x28;
    undefined field_0x29;
    undefined field_0x2a;
    undefined field_0x2b;
    undefined field_0x2c;
    undefined field_0x2d;
    undefined field_0x2e;
    undefined field_0x2f;
    undefined4 syuchu_value0;
    undefined4 syuchu_value1;
    undefined4 syuchu_value2; /* +0x38: the third syuchu number (the uncalled net_syuchu_value2_get) */
} net_rare_syuchu_rec;

/* The power_on reply's values (net_game_config, 0x0C4223C0), filled by
 * net_power_on_reply_parse; the rows at +0x57c and +0x5bc were GCC split
 * bases inside it (build/structs/split_rows.py). */
typedef struct net_game_config_rec {
    undefined1 field_0x0[64];
    net_rare_syuchu_rec rare_syuchu; /* +0x40: the four rare numbers, the syuchu text and numbers */
    undefined1 field_0x7c[1024];
    int kumite_values[75]; /* +0x47c: the kumite numbers, 0-terminated (net_kumite_value_find) */
    undefined1 field_0x5a8[16];
    undefined4 nushi_on; /* +0x5b8: 1 when the reply's nushi was "1" (net_nushi_is_on) */
    undefined1 field_0x5bc[16];
    undefined4 masa_on; /* +0x5cc: 1 when the reply's masa was "1" (net_masa_is_on) */
    undefined1 field_0x5d0[16];
    undefined4 ryuko_on; /* +0x5e0: 1 when the reply's ryuko was "1" (net_ryuko_is_on) */
    char struggle_revise_text[128]; /* +0x5e4: the reply's struggle_revise text (net_power_on_reply_parse copies 0x80 bytes) */
    uint struggle_revise[17]; /* +0x664: its 17 numbers (net_struggle_revise_get); the record ends where net_message_text begins */
} net_game_config_rec;

/* The authentication client configuration: a tag word, a zero word, then a copy of auth_client_settings. */
typedef struct auth_client_config_rec {
    undefined4 tag;           /* +0x0: 0x41424920 */
    undefined4 field_0x4;
} auth_client_config_rec;

/* The settings block alab_start_auth fills and copies into auth_client_config: small constants, the router address and the game id, version and serial strings. */
typedef struct auth_client_settings_rec {
    undefined4 field_0x0;
    undefined4 field_0x4;
    undefined4 field_0x8;
    undefined4 field_0xc;
    undefined4 field_0x10;
    undefined field_0x14;
    undefined field_0x15;
    undefined field_0x16;
    undefined field_0x17;
    undefined field_0x18;
    undefined field_0x19;
    undefined field_0x1a;
    undefined field_0x1b;
    undefined field_0x1c;
    undefined field_0x1d;
    undefined field_0x1e;
    undefined field_0x1f;
    undefined field_0x20;
    undefined field_0x21;
    undefined field_0x22;
    undefined field_0x23;
    undefined4 game_id;       /* +0x24: net_game_id_get's text */
    undefined4 version;       /* +0x28: net_version_text_get's text */
    undefined4 serial;        /* +0x2c: net_serial_get's text */
} auth_client_settings_rec;

/* The two reply words of a finished authentication. */
typedef struct auth_reply_word_pair {
    undefined4 word0;
    undefined4 word1;
} auth_reply_word_pair;

/* The place id of a finished authentication: a short and a second word. */
typedef struct auth_place_id_rec {
    undefined2 id;
    undefined field_0x2;
    undefined field_0x3;
    undefined4 id_word2;
} auth_place_id_rec;

/* ---- vf4lib/libbase/src/adv_demo.c --------------------------------- */

/* One rob's pending request from the attract demo's script (16 bytes, two
 * at demo_rob_req): commands 1 and 2 fill it, demo_rob_update applies it
 * to the rob's +0x18 state word and motion and clears flags. */
typedef struct demo_rob_request {
    unsigned int and_mask;  /* flags bit 0: state &= and_mask (command 1, op 0) */
    unsigned int or_mask;   /* bit 1: state |= or_mask (op 1)                   */
    unsigned int set;       /* bit 2: state = set (op 2)                        */
    unsigned short motion;  /* bit 3: start this motion (command 2)             */
    unsigned char flags;    /* which of the four are pending                    */
    unsigned char pad;
} demo_rob_request;

/* the color the ADVERTISE demo moves the stage lights towards */
typedef struct adv_light_color {
    float r;                  /* +0x0: red */
    float g;                  /* +0x4: green */
    float b;                  /* +0x8: blue */
} adv_light_color;

/* ---- vf4lib/librobdisp/src/rob_disp.c ---------------------------- */

/* The item test's state (0x20 bytes, at itmtst_state): itmtst_start fills
 * it from rob 0 and steps every item of that character's row of
 * chara_item_table through itmtst_step. */
typedef struct itmtst_rec {
    struct rob_rec *rob0;               /* rob_get(0); its +1 / +2 give chara and costume */
    struct rob_rec *rob1;               /* +0x04: rob_get(1)                             */
    unsigned int chara;       /* +0x08: the row of chara_item_table            */
    unsigned int costume;     /* +0x0c: byte offset into each 12-byte item entry */
    unsigned int items;       /* +0x10: the row's item entries                 */
    int item_count;           /* +0x14: the row's item count                   */
    unsigned int saved_pause_flags; /* +0x18: pause_flags, kept by itmtst_display_setup */
    char step;                /* +0x1c: itmtst_step's state; 0 idle            */
} itmtst_rec;

/* One best-of table of the item test: per item slot kind (itmtst_item_slot_get)
 * the highest value seen and the item that set it. */
typedef struct itmtst_best {
    float val[4];
    int who[4];               /* +0x10: item number; -1 none                 */
} itmtst_best;

/* The item test's statistics (0x1a4 bytes, at itmtst_stats). itmtst_step
 * shows every item for 120 frames and stores a sample every 30 after that
 * (12 rows); itmtst_item_record averages them into the log and the best-of
 * tables, and itmtst_summary_write lists the tables by mode. */
typedef struct itmtst_stats_rec {
    int item;                 /* the item under test; -1 before the first     */
    unsigned int frame;       /* +0x04                                        */
    short mode;               /* +0x08: the summary step, 1-9                 */
    unsigned int sample;      /* +0x0c: the sample row, 0-11                  */
    float cpu[12];            /* +0x10: itmtst_sample_store's columns: the CPU */
    float render[12];         /* +0x40: perf_render_usec                      */
    float ta[12];             /* +0x70: perf_ta_usec                          */
    itmtst_best best[8];      /* +0xa0: [2 * m + v]: m 0 the CPU average, 1 the
                                 render peak, 2 altex KB, 3 obj KB; v 1 for a
                                 slot-0 item with flag 4, else 0, and a v 0
                                 entry of kind 2-3 also sets v 1's who       */
    char *log;                /* +0x1a0: the text itmtst_item_record appends to */
} itmtst_stats_rec;

/* One 24-byte osage node parameter as kept in j_ex_00: five parameter words and the collision list. */
typedef struct osage_param {
    ushort rot_type;          /* +0x00: osage_node_init / osage_node_calc */
    short limit;              /* +0x02 */
    float stiffness;          /* +0x04 */
    float damping;            /* +0x08 */
    float gravity;            /* +0x0c */
    short rest_ry;            /* +0x10 */
    short rest_rz;            /* +0x12 */
    undefined4 coli_list;     /* +0x14: the collision list (osage_chain_coli_attach, osage_chains_add_params) */
} osage_param;

/* The two trail node object ids item_zanzou_trails_init sets up the item afterimage trails with. */
typedef struct item_zanzou_trail_id_pair {
    undefined4 item_trail;    /* +0x0: object id of trails 0-7 */
    undefined4 fixed_trail;   /* +0x4: object id of trails 8-9 */
} item_zanzou_trail_id_pair;

/* A point on the bone (x, y, z) for item trail 8. */
typedef struct zanzou_fixed_point_8_vec {
    undefined4 x;
    undefined4 y;
    undefined4 z;
} zanzou_fixed_point_8_vec;

/* A point on the bone (x, y, z) for item trail 9, the mirror of trail 8's. */
typedef struct zanzou_fixed_point_9_vec {
    undefined4 x;
    undefined4 y;
    undefined4 z;
} zanzou_fixed_point_9_vec;

/* A direction (x, y, z) for item trail 8. */
typedef struct zanzou_fixed_dir_8_vec {
    undefined4 x;
    undefined4 y;
    undefined4 z;
} zanzou_fixed_dir_8_vec;

/* A direction (x, y, z) for item trail 9, the mirror of trail 8's. */
typedef struct zanzou_fixed_dir_9_vec {
    undefined4 x;
    undefined4 y;
    undefined4 z;
} zanzou_fixed_dir_9_vec;

/* Per rob the microseconds spent on its bone chains and osage chains this frame. */
typedef struct rob_usec_pair {
    float rob0;               /* +0x0: rob 0's time */
    float rob1;               /* +0x4: rob 1's time */
} rob_usec_pair;

/* ---- vf4lib/libsprite/src/sprite.c ------------------------------- */

/* What sprite_put adds to every queued sprite (0x28 bytes, at
 * sprite_put_state), set by the sprite_*_set / sprite_*_reset pairs. */
typedef struct sprite_put_rec {
    vec3 offset;              /* sprite_offset_set; z on top of sprite_base_z  */
    float scale_x;            /* +0x0c: sprite_scale_set, multiplies the width  */
    float scale_y;            /* +0x10: multiplies the height                  */
    float alpha;              /* +0x14: sprite_alpha_set, multiplies the entry's factor */
    unsigned int col_override;   /* +0x18: replaces the color unless 0xFFFFFF  */
    unsigned int offset_col_override; /* +0x1c: replaces the second (offset) color word unless
                                    0xFF000000 (sprite_colors_set)            */
    unsigned int flags_override; /* +0x20: replaces the flags when non-zero
                                    (sprite_flags_override_set)              */
    short angle_base;         /* +0x24: added to each sprite's rotation angle  */
} sprite_put_rec;

/* One clip rectangle (6 bytes, sprite_clip_rects[20]), stored by
 * sprite_clip_push and applied by sprite_clip_apply. Typed as a record, not
 * char [20][6]: both functions match only with field accesses (a field at a
 * constant offset is added to the symbol before the variable offset). */
typedef struct sprite_clip_rect {
    char x0;                  /* +0: clip_rect_per_list_bits' first edge       */
    char y0;                  /* +1                                            */
    char x1;                  /* +2                                            */
    char y1;                  /* +3                                            */
    char mode;                /* +4: pcw_user_clip_mode_put's mode             */
    char hdr;                 /* +5: hdr_or_user_clip's argument               */
} sprite_clip_rect;

/* A color as three unit floats (red, green, blue). */
typedef struct unit_rgb {
    float r;
    float g;
    float b;
} unit_rgb;

/* ---- vf4lib/libsound/src/sound.c --------------------------------- */

/* The sound layer's state record (0x60 bytes, at sound_state; the word at
 * +0x74, the attract sound setting, stays its own global for now: every
 * reader loads it through its own pool word, which a member past mov.l's
 * reach would also get). sound_state_get
 * hands it out for the flags. Members no routine reads by name keep their
 * offsets. */
typedef struct sound_rec {
    unsigned int flags;       /* request and load bits: 0 stage music, 1 other
                                 music, 12-14 stage hooks, 16 queued load
                                 running, 17 queue started (sound_state_get) */
    unsigned int field_0x4;
    int boot_result;          /* +0x08: aica_driver_boot; sound_system_init
                                 hangs unless 0                             */
    unsigned int field_0xc;
    unsigned int field_0x10;
    unsigned int cmd_result;  /* +0x14: the last snd_cmd_a0_post (all off ...) */
    unsigned int play_result; /* +0x18: the last sound_request_post of a
                                 sound_play_id / SOUND_TEST request          */
    unsigned int field_0x1c;
    unsigned int field_0x20;
    unsigned int field_0x24;
    unsigned int field_0x28;
    short req_peak;           /* +0x2c: most requests in one frame           */
    short req_count;          /* +0x2e: requests this frame (sound_request_post) */
    short req_last;           /* +0x30: last frame's count (sound_frame_update) */
    short field_0x32;
    int dtpk_free;            /* +0x34: dtpk_space_left, kept by sound_test_update */
    void *libsnd_build;       /* +0x38: libsnd_build_text                    */
    unsigned int field_0x3c;  /* sound_test_update counts a second frame
                                 counter while it is 1                      */
    unsigned int last_request; /* +0x40                                     */
    unsigned int field_0x44;
    unsigned char field_0x48; /* +0x48 / +0x49: cleared by sound_state_clear */
    unsigned char field_0x49;
    short field_0x4a;
    int drv_e8;               /* +0x4c: the driver's word 0x008000e8, read every frame */
    int drv_ec;               /* +0x50: the driver's word 0x008000ec          */
    int drv_ec_peak;          /* +0x54: its peak; sound_test_open clears it   */
    unsigned int ring_result; /* +0x58: aica_ring_push's answer               */
    unsigned int fx_result;   /* +0x5c: sound_effect_defaults_post's first post */
} sound_rec;

/* ---- vf4lib/libcard/src/card.c ----------------------------------- */

/* Card reception (8 bytes, at card_service_state; card_service_state_get,
 * card_service_mode_get). */
typedef struct card_service_rec {
    unsigned char open;       /* reception open: net_service_hours_check, the
                                 debug force open / close                   */
    int mode;                 /* +0x04: card_service_state_update: 1 open, then
                                 3 (play_style 1, card_close_phase_set(1)) or
                                 2 (play_style 2) once it closes; -1 closed */
} card_service_rec;

/* The card's bitfield words. The code reads each one three ways: as the stored bytes or
 * halfwords, as the whole word (mask tests such as & 0x300, |= 0xf) and through uint / ushort
 * bitfields. GCC 3.2.3 takes a bitfield's access width from the containing object's
 * alignment (get_best_mode, SLOW_BYTE_ACCESS), so the byte and halfword reads the game also
 * does need byte and halfword members beside the bitfields: hence unions, not bitfield
 * members alone (tested both ways, 2026-11-04). */

/* header +0x1A. ushort bitfields: a uint member would make card_header_rec 4-aligned and turn
 * the accessors' halfword read-modify-writes into word ones. Through a card_record_rec
 * (4-aligned) GCC widens a store to the word at +0x18 (card_mode_entry_read). */
typedef struct card_ftf_bits {
    ushort : 1;            /* bit 0 unused, not stored */
    ushort ex_ver : 2;     /* FTF_EX_VER: format 3 + this when F_VERSION is 3 (card_format_get) */
    ushort ticket : 2;     /* FTF_TICKET */
    ushort skill_sel : 2;  /* FTF_SKILL_SEL */
    ushort recover : 1;    /* FTF_RECOVER */
    ushort : 8;            /* unused */
} card_ftf_bits;

typedef union card_ftf_half {
    uchar b[2];            /* the low byte as card_unpack, card_format_get and
                              card_mode_entry_read read and write it */
    ushort h;
    card_ftf_bits f;
} card_ftf_half;

/* The 28-byte card header (card image +0x00, card record +0x00 and its backup +0x64).
 * Packed to 24 bytes by card_pack (+0x05, +0x17 and the top byte of +0x1A are not stored;
 * packed byte 18 = use_count bit 8 | ftf low byte & 0xFE). */
typedef struct card_header_rec {
    short crc;             /* +0x00 CRC16 of the packed header block (card_format*_check, card_write_start) */
    uchar flags;           /* +0x02 bits 0-1 F_CODE, 2 F_NET_TOUCH, 3 F_B_NET_ACCESS, 4-5 F_VERSION
                              (card_format_field_get: format 0-3), 6 F_HAIKI (card_haiki_get/set),
                              7 F_NOW_GAME (set at game start, cleared by the result write) */
    uchar char_num;        /* +0x03 low nibble CHAR_NUM (13 = bad), high nibble the quest counter
                              (card_hdr_quest_count_get/set) */
    uchar level;           /* +0x04 LEVEL (dan; 0xFF = no card, >= 0xEE = bad header); "*1P_LEVEL" */
    undefined1 field_0x5;  /* +0x05 never stored on the card */
    ushort all_win;        /* +0x06 ALL_WIN, saturates at 0xFFFF; upload "win_%s" */
    ushort all_lose;       /* +0x08 ALL_LOSE, saturates at 0xFFFF; upload "lose_%s" */
    uchar item_have[8];    /* +0x0A ITEM_HAVE0-7: one bit per item (card_hdr_item_test/put) */
    short use_count;       /* +0x12 USE_COUNT, 9 bits on the card (& 0x1FF); plays left, 500 on a new or
                              renewed card, decremented per play; > 0 tested signed */
    uchar card_id[3];      /* +0x14 CARD_ID, 24-bit little-endian (card_hdr_card_id_get/set) */
    undefined1 field_0x17; /* +0x17 never stored on the card */
    ushort nickname;       /* +0x18 NICKNAME: 2 bits per nickname mark (nickname_card_marks_load/save) */
    card_ftf_half ftf;     /* +0x1A FTF flags */
} card_header_rec;                                             /* 0x1C */

/* body +0x1C: CURSE and EMBLEM. ushort bitfields; in the 4-aligned body GCC widens their
 * accesses to the word at +0x1C, as the game does (card_unpack_blocks, card_colors_to_packed). */
typedef struct card_curse_bytes {
    char curse;            /* CURSE as a byte: the slot-lock reads (item_slot_lock_test, ...) */
    uchar emblem;          /* EMBLEM: emblem sprite number + 1 */
} card_curse_bytes;

typedef struct card_curse_bits {
    ushort slot_lock : 4;     /* one lock bit per item slot ("item_curse_%dp") */
    ushort color_1p_hi0 : 1;  /* bit 4 of color_1p value 0 (card_colors_to_packed) */
    ushort color_1p_hi1 : 1;
    ushort color_1p_hi2 : 1;
    ushort color_1p_hi3 : 1;
    ushort emblem : 8;
} card_curse_bits;

typedef union card_curse_half {
    card_curse_bytes b;
    ushort h;
    card_curse_bits f;
} card_curse_half;

/* body +0x38: EMBLEM_DAN and SYUUTYUU. */
typedef struct card_syu_bytes {
    uchar emblem_dan;      /* high nibble EMBLEM_DAN, low nibble quest year (card_body_dan_low) */
    undefined1 field_0x39; /* not stored on the card */
    ushort syuutyuu;       /* SYUUTYUU as stored */
} card_syu_bytes;

typedef struct card_syu_bits {
    uint quest_year : 4;
    uint emblem_dan : 4;
    uint : 8;
    uint mission_day : 5;  /* syuutyuu bits 0-4 */
    uint prize_misses : 6; /* bits 5-10, max 30 */
    uint revise : 2;       /* bits 11-12 */
    uint user_class : 3;   /* bits 13-15 (card_syu_bits_get, net_reply_user_apply) */
} card_syu_bits;

typedef union card_syu_word {
    card_syu_bytes b;
    uint w;
    card_syu_bits f;
} card_syu_word;

/* body +0x3C: QUEST0 and QUEST1. quest0_b0_get reads the halfword (the game's mov.w). */
typedef struct card_quest_halves {
    ushort quest0;
    ushort quest1;
} card_quest_halves;

typedef struct card_quest_bits {
    uint state : 3;        /* QUEST0 bits 0-2 (quest_state_get) */
    uint kind : 4;         /* bits 3-6 */
    uint limit_day : 5;    /* bits 7-11 */
    uint limit_month : 4;  /* bits 12-15 */
    uint param_a : 6;      /* QUEST1 bits 0-5 */
    uint param_b : 4;      /* bits 6-9 */
    uint param_c : 4;      /* bits 10-13 */
    uint category : 2;     /* bits 14-15 */
} card_quest_bits;

typedef union card_quest_word {
    card_quest_halves b;
    uint w;
    card_quest_bits f;
} card_quest_word;

/* body +0x40: MISC0-MISC2. Read as one word for mask tests (& 0x300, & 0x800, & 0xc) and
 * |= 0xf (game_judge_init, card_cpu_play_end_side set bits 0-3 on a network play). */
typedef struct card_misc_bits {
    uint equip_flag0 : 1;  /* equip flags for costume 0/1 (item_equip_if_flagged, cleared by item_equip) */
    uint equip_flag1 : 1;
    uint color_flag0 : 1;  /* color flags for costume 0/1 (cleared by costume_colors_set) */
    uint color_flag1 : 1;
    uint net_colors : 1;   /* colors from the network: blocks the level colors */
    uint color_2p_hi0 : 1; /* bit 4 of color_2p values 0-2 */
    uint color_2p_hi1 : 1;
    uint color_2p_hi2 : 1;
    uint tenpo_value2 : 2; /* MISC1 bits 0-1 */
    uint color_2p_hi3 : 1; /* MISC1 bit 2: bit 4 of color_2p value 3 */
    uint foul_first : 1;   /* MISC1 bits 3-4: foul */
    uint foul_second : 1;
    uint disp : 3;         /* MISC1 bits 5-7: card panel type */
    uint lottery_count : 8;/* MISC2: item lottery counter, saturates at 0xFF */
    uint : 8;              /* +0x43, not stored on the card */
} card_misc_bits;

typedef union card_misc_word {
    uchar b[4];            /* MISC0-MISC2 as stored (the card_pack / card_unpack loops) */
    uint w;
    card_misc_bits f;
} card_misc_word;

/* A card color word: four 5-bit costume color values (bits 0-19). card_format1_check copies
 * its two low bytes one by one (byte loads at +0, +1), so the stored bytes are a member. */
typedef union card_color_word {
    uchar b[4];            /* the stored bytes (card_format1_check) */
    uint w;                /* the value */
} card_color_word;

/* The low nibbles of a color word (card_colors_to_packed). card_format2_check and
 * card_format34_check copy the two bytes one by one. */
typedef union card_color_half {
    uchar b[2];            /* the stored bytes (card_format2_check, card_format34_check) */
    ushort h;              /* the value */
} card_color_half;

/* The 72-byte card body (card image +0x1C, card record +0x1C). Packed to 60 bytes by card_pack
 * (+0x06/07, +0x08..0x0F as words, +0x39 and +0x43 not stored: the color words are rebuilt
 * from color_*_packed plus flag bits by card_unpack_blocks). */
typedef struct card_body_rec {
    short crc;             /* +0x00 CRC16 of the packed body block */
    ushort play_id;        /* +0x02 PLAY_ID: lcg_next15() written at every card write; the record and
                              image copies must agree (card_play_id_matches) */
    uchar exp;             /* +0x04 below level 10 EXP (0-99, level*100+exp); from level 10
                              RENSHO << 4 | RENPAI (card_body_rensho/renpai_get/set) */
    char point;            /* +0x05 WIN_LOSE, printed as "POINT %d"; signed, clamped to +-127 */
    undefined1 field_0x6[2]; /* +0x06 */
    card_color_word color_1p;  /* +0x08 COLOR1P_0-3: w the four 5-bit costume color values,
                                  b the bytes card_format1_check stores one by one */
    card_color_word color_2p;  /* +0x0C COLOR2P_0-3, as color_1p */
    card_color_half color_1p_packed; /* +0x10 the low nibbles of color_1p (card_colors_to_packed):
                                        h the value, b the bytes card_format34_check stores */
    card_color_half color_2p_packed; /* +0x12 the low nibbles of color_2p, as color_1p_packed */
    uchar equip_1p[4];     /* +0x14 EQUIP1P_0-3: item id per slot, costume 0 (item_slot_get) */
    uchar equip_2p[4];     /* +0x18 EQUIP2P_0-3: item id per slot, costume 1 */
    card_curse_half curse; /* +0x1C CURSE, EMBLEM */
    char name[20];         /* +0x1E card (ring) name, not NUL-terminated at 20 */
    char other_col;        /* +0x32 OTHER_COL: per costume eye (bits 0-1 / 4-5) and hair (2-3 / 6-7) color */
    char temp;             /* +0x33 TEMP: bits 0-1 quest minute band, bits 2-7 quest hour */
    ushort my_tenpo_id;    /* +0x34 MY_TENPO_ID: the shop id (net_card_place_matches) */
    ushort money;          /* +0x36 MONEY: bits 0-5, 6-11 and 12-15 (card_money_add / _reply_apply);
                              those two read it as a short, four other readers as unsigned */
    card_syu_word syu;     /* +0x38 EMBLEM_DAN, SYUUTYUU */
    card_quest_word quest; /* +0x3C QUEST0, QUEST1 */
    card_misc_word misc;   /* +0x40 MISC0-MISC2 */
    ushort rank_pnt;       /* +0x44 RANK_PNT ("ranking_point_%s") */
    ushort tenpo_pnt;      /* +0x46 TENPO_PNT ("my_tenpo_point_%s") */
} card_body_rec;                                               /* 0x48 */

/* card_images[2]: the copy of the card the game plays with (written back at the result). */
typedef struct card_image_rec {
    card_header_rec header;  /* +0x00 */
    card_body_rec body;      /* +0x1C */
    int access_key;          /* +0x64 copy of the record's access_key (card_record_to_image);
                                card_access_code_make turns it into the 16-digit access code */
} card_image_rec;                                              /* 0x68 */

/* The format-2 (older card) header as card_unpack_format2 stores it at record +0x90 / +0xE6.
 * Same offsets as card_header_rec; +0x12 is a full 16 bits and +0x18 / +0x1A hold what
 * card_format2_upgrade moves into the body's rank_pnt / tenpo_pnt (inferred from that move). */
typedef struct card_fmt2_header_rec {
    short crc;             /* +0x00 */
    uchar flags;           /* +0x02 */
    uchar char_num;        /* +0x03 */
    uchar level;           /* +0x04 */
    undefined1 field_0x5;  /* +0x05 */
    short all_win;         /* +0x06 */
    short all_lose;        /* +0x08 */
    uchar item_have[8];    /* +0x0A */
    short use_count;       /* +0x12 (card_format2_check tests a[9] < 0) */
    uchar card_id[3];      /* +0x14 */
    undefined1 field_0x17; /* +0x17 */
    short rank_pnt;        /* +0x18 -> body.rank_pnt on upgrade */
    ushort tenpo_pnt;      /* +0x1A -> body.tenpo_pnt on upgrade */
} card_fmt2_header_rec;                                        /* 0x1C */

/* The format-2 body (card_unpack_format2 rec_b, card_format2_upgrade savent). */
typedef struct card_fmt2_body_rec {
    short crc;             /* +0x00 */
    short play_id;         /* +0x02 */
    uchar exp;             /* +0x04 */
    uchar point;           /* +0x05 */
    card_color_half color_1p_packed; /* +0x06 h -> body.color_1p_packed (+ color_1p rebuilt,
                                        4-bit values) by card_format2_upgrade; b stored by
                                        card_format2_check */
    card_color_half color_2p_packed; /* +0x08 as color_1p_packed */
    uchar equip_1p[4];     /* +0x0A */
    uchar equip_2p[4];     /* +0x0E */
    uchar curse;           /* +0x12 (& 15 on upgrade) */
    uchar emblem;          /* +0x13 */
    char name[20];         /* +0x14 */
    uchar other_col;       /* +0x28 */
    uchar temp;            /* +0x29 */
    short my_tenpo_id;     /* +0x2A */
    short money;           /* +0x2C */
    uchar emblem_dan;      /* +0x2E */
    undefined1 field_0x2f; /* +0x2F */
    short syuutyuu;        /* +0x30 not carried over: the upgrade zeroes mid6/b11/top3 */
    short quest0;          /* +0x32 */
    short quest1;          /* +0x34 */
    uchar misc[3];         /* +0x36 MISC0-MISC2 as stored: card_unpack_format2's loop fills
                              misc[i]; card_format2_upgrade reads misc[0] & 0x1F, misc[1] & 0xFB,
                              misc[2] */
    undefined1 field_0x39; /* +0x39 */
} card_fmt2_body_rec;                                          /* 0x3A */

/* card_records[2]: the working record of a side. card_renew_backup[2] (0x0C430FBC) holds the
 * same records but stays char[2][480]: typed, card_mode_renew's memcpy calls compile inline. */
typedef struct card_record_rec {
    card_header_rec header;            /* +0x000 */
    card_body_rec body;                /* +0x01C */
    card_header_rec header2;           /* +0x064 backup copy of header (card_write_start; the format checks
                                          restore one from the other) */
    uint card_word0;                   /* +0x080 card header word 0 (big-endian); top nibble must be 1.
                                          card_mode_renew reads access_key as word 1 from here:
                                          the four words may have been one array or record */
    int access_key;                    /* +0x084 card header word 1 (big-endian): the card's key / serial;
                                          card log "card_id", discarded_code_take, access code */
    uint unit_key;                     /* +0x088 card_keys[unit] at read time */
    uint card_word3;                   /* +0x08C card header word 3 (big-endian); high half must be 0x046C */
    card_fmt2_header_rec fmt2_header;  /* +0x090 format-2 unpack area (cleared by card_format2_area_clear) */
    card_fmt2_body_rec fmt2_body;      /* +0x0AC */
    card_fmt2_header_rec fmt2_header2; /* +0x0E6 */
    undefined1 field_0x102[2];         /* +0x102 never accessed */
    int in_use;                        /* +0x104 0 no card in play, 1 card accepted for play;
                                          rob_color_build and rob_item read only its low byte */
    uchar write_buf[108];              /* +0x108 card_pack output queued to the card (words 5-31) */
    uchar raw[108];                    /* +0x174 the 27 data words as read (card_read_finish) */
} card_record_rec;                                             /* 0x1E0 */

/* ---- vf4lib/libeffect/src/effect_snow.c --------------------------- */

/* The snow's wind (0x34 bytes, at snow_wind): snow_wind_update sets
 * wind = base + dir * amp * sin(phase + amp2 * sin(phase2)) each frame,
 * and the flakes drift by wind.x / wind.z. */
typedef struct snow_wind_rec {
    vec3 base;                /* snow_wind_init: (-0.7, 0, 0.2)              */
    vec3 dir;                 /* +0x0c: (0.3, 0, 0.3)                        */
    vec3 wind;                /* +0x18: this frame's wind                    */
    short phase;              /* +0x24                                       */
    short phase2;             /* +0x26: bends phase                          */
    short phase_step;         /* +0x28: 0x60 a frame                         */
    short phase2_step;        /* +0x2a: 0x130 a frame                        */
    float amp;                /* +0x2c: 1.0                                  */
    float amp2;               /* +0x30: 30000.0, in angle units              */
} snow_wind_rec;

/* ---- vf4lib/libeffect/src/effect_leaf.c --------------------------- */

/* A leaf of the LEAF stage effect (0x44 bytes): leaf_init allocates
 * 560000 bytes of them (leaf_records); cfg +0x0C leaves rest on the
 * ground, the first cfg +0x14 can fly. */
typedef struct leaf_rec {
    float pos[3];
    vec3 vel;                 /* +0x0c                                       */
    float nrm[3];             /* +0x18                                       */
    float spin;               /* +0x24                                       */
    float spin_max;           /* +0x28                                       */
    float floor_y;            /* +0x2c                                       */
    int packed_nrm;           /* +0x30: ORed into the ground vertex word     */
    short ang[3];             /* +0x34                                       */
    short cell;               /* +0x3a: its leaf_grid cell; -1 none (retried
                                 by leaf_grid_rescan_step), below -2 off
                                 the floor                                   */
    short id;                 /* +0x3c: its index                            */
    unsigned char mode;       /* +0x3e: 0 resting, 2 just spawned            */
    char source;              /* +0x3f: -1 none                              */
    unsigned char shape;      /* +0x40: i & 3, picks its UVs                 */
    char pad41[3];
} leaf_rec;

/* ---- vf4lib/libeffect/src/effect_mot.c --------------------------- */

struct bone_work_rec;      /* a bone's work record, defined with the motion types below */

/* One motion path object (mot_path_slots, three of them): the bone record
 * and motion record mot_paths_bind carves out of its block (0x6c and 0xc0
 * bytes), kept as the addresses the rob bone calls take. */
typedef struct mot_path_slot {
    struct bone_work_rec *bone; /* the bone record (mot_path_eval evaluates it;
                                 the effects read its position and angles) */
    int mot;                  /* +0x04: the motion record                    */
} mot_path_slot;

/* ---- vf4lib/librobdisp/src/rob_disp.c --------------------------- */

/* An osage node (44 bytes, osage_nodes[2][100]): one joint of a hanging
 * chain (hair, cloth), handed out by osage_chain_build through
 * osage_node_next; rob_osage_init marks the matrix and link 0xDEADDEAD. */
typedef struct osage_node {
    int num;                  /* its number; osage_node_by_index by it       */
    float *mtx;               /* +0x04: its matrix                           */
    float pos[3];             /* +0x08                                       */
    float vel[3];             /* +0x14                                       */
    int link;                 /* +0x20: a node of another chain it is held
                                 to (osage_node_link_set)                    */
    float link_len;           /* +0x24: that link's length                   */
    short ang[2];                  /* +0x28                                       */
} osage_node;

/* One item trail's item (0x38 bytes, item_zanzou_items[2][10]): set up by
 * item_zanzou_setup from the costume's entry, placed each frame by
 * item_zanzou_calc, drawn by item_zanzou_disp. */
typedef struct zanzou_item {
    int on;
    int vis;                  /* +0x04: drawn this frame                     */
    int f8;
    int bone;                 /* +0x0c: the bone whose matrix places it      */
    float size;               /* +0x10                                       */
    float point[3];           /* +0x14: the point on the bone                */
    float dir[3];             /* +0x20: its direction                        */
    float screen[3];          /* +0x2c: the placed point                     */
} zanzou_item;

/* ---- vf4lib/librobdisp/src/rob_color.c --------------------------- */

/* A source color entry (16 bytes): the first of a costume part's three
 * lists, copied into texcol_scratch_a (at most 15 and the -1 end mark) and
 * turned into color entries by texcol_lists_combine. */
typedef struct texcol_src {
    int id;                   /* the entry's id; -1 ends the list            */
    float a;                  /* +0x04: stored as the entry's first byte     */
    float b;                  /* +0x08: and its second                       */
    unsigned short c;         /* +0x0c: scaled by 180 / 32768                */
    short pad;
} texcol_src;

/* The first entry of the third color scratch list (8-byte entries), after texcol_scratch_a (16-byte) and the 12-byte list at 0x0C4921B0; costume_color_lists_copy fills it and ends it with two -1 words. */
typedef struct texcol_pair_entry {
    undefined4 id;            /* +0x0: -1 ends the list (costume_color_lists_copy) */
    undefined4 field_0x4;
} texcol_pair_entry;

/* ---- vf4lib/libeffect/src/stage_screen.c --------------------------- */

/* A rolling bar of a stage screen's noise (0x414 bytes): its offsets are
 * added to the screen's 257 line offsets from the bar's line on
 * (noise_roll_bar_add). */
typedef struct screen_noise_bar {
    int line;                 /* the line the bar starts at, moved on one line
                                 every speed frames (mod 257)                */
    int speed;                /* +0x04: frames per line (5)                  */
    float offsets[257];       /* +0x08: the bar's offset at each line from
                                 its start, until it leaves 0..1             */
    float c;                  /* +0x40c: 0.03                                */
    float d;                  /* +0x410: 0.02                                */
} screen_noise_bar;

/* One side's screen noise (0x420 bytes, screen_noise_records[2]), set up by
 * screen_noise_init and applied by screen_noise_apply. */
typedef struct screen_noise_rec {
    int x;
    float level;              /* +0x04: 0..0.02                              */
    int angle;                /* +0x08: 0..179                               */
    screen_noise_bar bars[1]; /* +0x0c                                       */
} screen_noise_rec;

/* A stage screen's projection factors: x / y scale and the x / y terms per unit of depth. */
typedef struct screen_proj_factors {
    float x_scale;
    float y_scale;
    float x_shift;            /* +0x8: times depth */
    float y_shift;            /* +0xc: times depth */
} screen_proj_factors;

/* A vector water_grid_build copies and negates but never uses. */
typedef struct water_spare_vec3 {
    undefined4 x;
    undefined4 y;
    undefined4 z;
} water_spare_vec3;

/* ---- vf4lib/librobdisp/src/rob_item.c --------------------------- */

/* A card item the debug part test gives a rob (rob_items_from_part_test),
 * one per item slot and side: an envelope (24 bytes, rob_item_envelopes) or
 * a model (20 bytes, rob_item_models). The item slot points at it. */
typedef struct rob_item_env {
    char a;
    unsigned char r;          /* +0x01: 0xFF                                 */
    unsigned char g;
    unsigned char b;
    unsigned int val;         /* +0x04: the item value << 16 | the slot's item */
    int x;                    /* +0x08: 0                                    */
    int y;
    int z;
    int w;                    /* +0x14                                       */
} rob_item_env;

typedef struct rob_item_model {
    char a;
    char r;                   /* +0x01: -1                                   */
    char g;
    char b;
    unsigned int val;         /* +0x04: the item value << 16 | the slot's item */
    int part;                 /* +0x08: the slot's part                      */
    int m;                    /* +0x0c: -1                                   */
    int n;                    /* +0x10                                       */
} rob_item_model;

/* The card mark's motion constants (16 bytes; card_mark_models follows): phase steps, follow rate, lift above the head and radius. */
typedef struct card_mark_motion_rec {
    short circle_step;        /* +0x0: added to the mark's circling angle each frame (0x140) */
    short bob_step;           /* +0x2: added to its bobbing angle each frame (0x180) */
    float rate;               /* +0x4: the fraction of the way moved each frame (0.1) */
    float lift;               /* +0x8: height above the head matrix (0.5) */
    float amp;                /* +0xc: radius of the circling and height of the bobbing (0.05) */
} card_mark_motion_rec;

/* Player 1's four item slot name lists for the ITEM_TEST windows. */
typedef struct item_test_names_1p_rec {
    undefined4 slot0;
    undefined4 slot1;
    undefined4 slot2;
    undefined4 slot3;
} item_test_names_1p_rec;

/* Player 2's four item slot name lists for the ITEM_TEST windows. */
typedef struct item_test_names_2p_rec {
    undefined4 slot0;
    undefined4 slot1;
    undefined4 slot2;
    undefined4 slot3;
} item_test_names_2p_rec;

/* The lengths of player 1's four item slot name lists for the ITEM_TEST windows. */
typedef struct item_test_counts_1p_rec {
    undefined4 slot0;
    undefined4 slot1;
    undefined4 slot2;
    undefined4 slot3;
} item_test_counts_1p_rec;

/* The lengths of player 2's four item slot name lists for the ITEM_TEST windows. */
typedef struct item_test_counts_2p_rec {
    undefined4 slot0;
    undefined4 slot1;
    undefined4 slot2;
    undefined4 slot3;
} item_test_counts_2p_rec;

/* ---- vf4lib/libsprite/src/sprite.c --------------------------- */

/* A sprite set (12 bytes, sprite_sets[81]): its file, the texture index
 * its textures start at, and its sprite list (16-byte entries, -1 at +4
 * ends it). Set 0x33 ("spr_fire.bin") is the L-TOP animation that
 * ltop_frame_read streams. */
typedef struct sprite_set {
    char *name;               /* "spr_comm.bin.gz" (0) .. "spr_emb.bin" (0x50) */
    int tex_base;             /* +0x04: 0, 120, 240 .. 9600                  */
    int sprites;              /* +0x08: the sprite list                      */
} sprite_set;

/* The sprite queue's count and marks (sprite_queue_state, 0x0C4DD82C):
 * one mark byte per queued sprite, set for the next sprite by
 * sprite_next_blend_set / sprite_next_blend_normal (blend bits 0-2) and
 * sprite_clip_push (clip bit 3), read and cleared by sprite_task_exec. The
 * code reaches the marks from this address (+2 + count). */
typedef struct sprite_queue_state_rec {
    short count;              /* entries in sprite_queue (at most 0x2ff)     */
    char marks[0x300];        /* +0x02                                       */
} sprite_queue_state_rec;

/* ---- vf4lib/libmission/src/msnm_text.c --------------------------- */

/* One inline image of the mission text (msn_text_images, 25 of them):
 * "P", "K", "G", the directions "1"-"9" and "R1"-"R9", "YOGA".. found by
 * name for an <IMG_name> tag (msn_text_image_tag). */
typedef struct msn_text_image {
    int sprite;               /* the sprite drawn                            */
    char *name;               /* +0x04: the tag's name                       */
    float w;                  /* +0x08: the width the text advances by       */
    float h;                  /* +0x0c                                       */
    float x;                  /* +0x10: offset from the text position        */
    float y;                  /* +0x14                                       */
    short angle;              /* +0x18: 0x2000 steps for the diagonals       */
    short pad1a;
    int flags;                /* +0x1c: the sprite's flip bits               */
    int color;                /* +0x20: the sprite color pair               */
    int color2;               /* +0x24                                       */
} msn_text_image;

/* ---- vf4lib/libmission/src/msnm_disp.c --------------------------- */

/* The challenge map display (0x164 bytes, at msnm_disp_state). msnm_disp_init
 * clears it and sets both handle arrays to -1 in two loops (31 and 49).
 * Every handle below +0x7c is an animated object (aet_obj_start / _stop),
 * every one from +0x7c a still (aet_still_start / _stop). */
typedef struct msnm_disp_rec {
    int objs[31];             /* among them: 0 the map base, 6 the current
                                 plate, 7-14 the plates (e_chlg_map_plate_names),
                                 27 MAP_SELROUTE                            */
    int stills[49];           /* +0x7c; among them: 27 MATCH_BASE_FLASH,
                                 34-41 the ranks (e_chlg_map_rank_names),
                                 47 on the round lamps (PARTS_KURO_LAMP)     */
    int plate_wait;           /* +0x140: 10 when a plate starts; the next waits
                                 for 0                                       */
    int sel;                  /* +0x144: the selection the display was built for
                                 (the mission work's +0x800)                */
    int plate;                /* +0x148: indexes the plate name and color tables */
    unsigned int flags;       /* +0x14c: 1 current plate started, 2 set by init,
                                 8 MATCH_BASE_FLASH showing, 0x10 its flash done */
    int selroute_frames;      /* +0x150: MAP_SELROUTE's end frame, counted down
                                 (chlg_selroute_layer_done); msn_panel_draw
                                 also runs it 0..60 as a blink               */
    int flash_frames;         /* +0x154: 12 frames of MATCH_BASE_FLASH        */
    int coin_frames;          /* +0x158: chlg_coin_prize_step's coin countdown */
    int box_frames;           /* +0x15c: its box countdown, down to 60        */
    char hidden;              /* +0x160: chlg_map_hidden_set's hide flag; no
                                 new part is started while it is set         */
} msnm_disp_rec;

/* Top-left x, y of the course total screen's three columns (stage number, stage score, rank marks). */
typedef struct chlg_total_column_xy {
    float stage_x;            /* +0x0: stage number column */
    undefined stage_y;
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
    float score_x;            /* +0x8: stage score column; rank marks follow */
    float score_y;
} chlg_total_column_xy;

/* A mission text box as msn_text_box_draw takes it: position, size, padding, line height, depth, alpha and color. */
typedef struct msn_text_box {
    undefined4 x;
    undefined4 y;
    undefined4 box_width;
    undefined4 box_height;
    undefined4 pad_x;
    undefined4 pad_y;
    undefined4 line_height;
    undefined4 depth;
    undefined4 field_0x20;
    undefined4 alpha;
    undefined4 color;
} msn_text_box;

/* Screen position of the COIN GET text. */
typedef struct chlg_coin_get_pos {
    float x;
    float y;
} chlg_coin_get_pos;

/* ---- vf4lib/libeffect/src/effect_rain.c -------------------------- */

/* The rain settings (at rain_params; at least 0xbc bytes, since
 * rain_params_init stores +0xb8 through its base, and snow_bodies follows at
 * +0xd8). Every pair is per streak set: rain_streaks_scatter places streaks
 * 0-20 in box 0 and 21-35 in box 1, and rain_streak_setup gives a streak
 * set 1's velocity, length and shape when its index is above 20. */
typedef struct rain_params_rec {
    float box[2][3];          /* x half-width, then the z range: set 0 spans
                                 [-[2], [1]], set 1 [-[2], -[1]]            */
    int count[2];             /* +0x18: 2 and 2; not read by name            */
    float weight[2];          /* +0x20: the streaks' gravity weight (0.7, 1.4) */
    float field_0x28;         /* +0x28-+0x30: not written by rain_params_init */
    float field_0x2c;
    float field_0x30;
    vec3 vel[2];              /* +0x34: the fall velocity                    */
    vec3 shape[2][3];         /* +0x4c: the streak's three corner offsets, from
                                 the table at 0x0C1A0FB4; rain_streak_setup
                                 mirrors corner 2's x into corner 1          */
    char field_0x94;          /* +0x94-+0xa8: set by rain_params_init (1, 0.3,
                                 (0, 0, -3)); no reader found               */
    float field_0x98;
    float field_0x9c;
    vec3 field_0xa0;
    unsigned int exec_usec;   /* +0xac: stopwatch_usec at the start of rain_exec */
    unsigned int field_0xb0;
    unsigned int field_0xb4;
    int field_0xb8;           /* 1 from rain_params_init; no reader found    */
} rain_params_rec;

/* ---- vf4lib/libeffect/src/effect_etc.c --------------------------- */

/* The one fish that swims to the edge of the camera's view and towards a
 * fighter (0x5c bytes, at fish_seeker): fish_seeker_init, fish_seeker_update
 * and its helpers, fish_seeker_draw, fish_seeker_rob_landed. */
/* fish_seeker's parameters (0x1c bytes, fish_seeker_params). */
typedef struct fish_seeker_prm {
    void *field_0x0;
    int field_0x4;
    float keep;               /* +0x08: how much of the old heading
                                 fish_seeker_steer keeps (0.96)              */
    float cruise;             /* +0x0c: the starting and resting speed       */
    float fast;               /* +0x10: the speed while far from the view edge */
    float close;              /* +0x14: the most speed closing on a rob      */
    float step;               /* +0x18: the speed change per frame           */
} fish_seeker_prm;

typedef struct fish_seeker_rec {
    unsigned char flags;      /* 2 drawn, 4 in the camera's view
                                 (fish_seeker_view_check), 8 heads for rob 0
                                 rather than rob 1 (state 2)                */
    char state;               /* +0x01: 1 swim to the view edge, 2 close on a
                                 rob, 3 the path motion, 4 the turn after a
                                 landing (fish_seeker_rob_landed)           */
    short field_0x2;          /* +0x02: 60 from fish_seeker_init             */
    short field_0x4;
    short rot_y;              /* +0x06: fish_seeker_draw turns by y, x, z     */
    short rot_x;
    short rot_z;              /* +0x0a: state 4 takes it from fish_seeker_turn_table */
    float field_0xc;
    float rate;               /* +0x10: spd / 0.1 while swimming; 0 on a landing */
    float spd;                /* +0x14: eased towards the params' speeds      */
    float t;                  /* +0x18: the path time (state 3), the turn's frame (state 4) */
    fish_seeker_prm *prm;     /* +0x1c: fish_seeker_params                    */
    vec3 pos;                 /* +0x20                                        */
    vec3 goal;                /* +0x2c: the view-edge point, clamped to the tank */
    vec3 edge;                /* +0x38: the view-edge point itself
                                 (fish_seeker_target_update)                */
    vec3 heading;             /* +0x44: fish_seeker_steer blends it towards want;
                                 fish_seeker_move moves pos along it         */
    vec3 want;                /* +0x50: the direction to the goal or the rob  */
} fish_seeker_rec;

/* One of the four camera-facing models of NOH STAGE's ETC effect (0x14 bytes): turn, scale and position. */
typedef struct noh_etc_obj {
    short turn;               /* +0x0: y angle */
    short field_0x2;
    float size;               /* +0x4: scale */
    float x;                  /* +0x8: position */
    float y;
    float z;
} noh_etc_obj;

/* A 0x18-byte bird box like the ones bird_flock_member_init picks positions in (max x, y, z then min x, y, z). */
typedef struct bird_spawn_box {
    undefined field_0x0;
    undefined field_0x1;
    undefined field_0x2;
    undefined field_0x3;
    undefined field_0x4;
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
    float max_z;
    undefined field_0xc;
    undefined field_0xd;
    undefined field_0xe;
    undefined field_0xf;
    undefined field_0x10;
    undefined field_0x11;
    undefined field_0x12;
    undefined field_0x13;
    float min_z;
} bird_spawn_box;

/* Where a fighter last hit the breaking floor (x, y, z). */
typedef struct break_floor_impact_pos {
    float x;
    undefined4 y;
    float z;
} break_floor_impact_pos;

/* NYC's two beam-glow light records (0x44 bytes each, copied from light_anim_lights): position at +8, glow scale at +0x30. */

/* The unused looping model effect's record: frame 0..63 and position. */
typedef struct loop64_model_rec {
    short frame;              /* +0x0: 0..63 */
    undefined field_0x2;
    undefined field_0x3;
    float x;                  /* +0x4: position */
    float y;
    float z;
} loop64_model_rec;

/* One of the six looping objects of GARDEN STAGE's OTHER effect (8 bytes): frame and time. */
typedef struct garden_other_obj {
    undefined2 frame;         /* +0x0: 0..79 */
    undefined field_0x2;
    undefined field_0x3;
    float time;
} garden_other_obj;

/* One of HBR's ten objects bobbing on the water (0x14 bytes): three phases, the tilts and height they give, and the parameter row. */
typedef struct hbr_float_obj {
    short bob_phase;
    short roll_phase;
    short pitch_phase;
    undefined roll;           /* +0x6: short: z tilt */
    undefined field_0x7;
    undefined pitch;          /* +0x8: short: x tilt */
    undefined field_0x9;
    undefined field_0xa;
    undefined field_0xb;
    undefined bob;            /* +0xc: float: height offset */
    undefined field_0xd;
    undefined field_0xe;
    undefined field_0xf;
    pointer row;              /* +0x10: parameter row (speeds, amplitudes, yaw +8, model +0xC, position +0x1C) */
} hbr_float_obj;

/* UMI STAGE's floating object (0x18 bytes): bob phases and tilts, a counter, two part spins, height offset and parameter row. */
typedef struct umi_float_rec {
    short bob_phase;
    short roll_phase;
    short pitch_phase;
    short roll;               /* +0x6: z tilt */
    short pitch;              /* +0x8: x tilt */
    short counter;            /* +0xa: 0..47 */
    short spin_a;             /* +0xc: first part's angle */
    short spin_b;             /* +0xe: second part's angle */
    float bob;                /* +0x10: height offset */
    char *row;                /* +0x14: parameter row (speeds, amplitudes, yaw +0xA, position +0x1C, part offsets +0x28 / +0x34) */
} umi_float_rec;

/* ---- vf4lib/libeffect/src/effect_sky.c --------------------------- */

/* The sky flash (0x1c bytes, at sky_flash) of the unreferenced
 * sky_flash_init / _exec / _draw / _lights_set. */
typedef struct sky_flash_rec {
    unsigned char flags;      /* 1 on, 2 flashing, 8 draw the variant's pieces,
                                 0x10 draw the stored piece matrices        */
    unsigned char pieces;     /* +0x01: piece matrices stored by sky_flash_draw */
    unsigned short frame;     /* +0x02: frames into the flash                */
    short frames;             /* +0x04: its length, 60-119 (lcg_next15)      */
    short delay;              /* +0x06: frames until sound 0x38D             */
    unsigned int model;       /* +0x08: the object drawn; 0x1B05C0 idle      */
    int *list;                /* +0x0c: walked one id a frame to -1          */
    float time_prev;          /* +0x10                                       */
    float time;               /* +0x14: light_anim_play's time; the flash ends
                                 when it falls back                         */
    unsigned int variant;     /* +0x18: the sky_flash_variants entry         */
} sky_flash_rec;

/* Light set 0's ambient color (r, g, b) as sky_flash_draw last saw it. */
typedef struct sky_flash_ambient_rgb {
    float r;
    float g;
    float b;
} sky_flash_ambient_rgb;

/* ---- vf4lib/libeffect/src/effect_sand.c (file name ours) --------- */

/* The sand's two colors (0x18 bytes, at sand_colors), one word per
 * channel; sand_colors_default_set sets them, and each apply routine packs
 * one color as 0xFF000000 | r << 16 | g << 8 | b into its models' +0x6c. */
typedef struct sand_colors_rec {
    unsigned int grain_r;     /* sand_grain_models_color_apply: the grains   */
    unsigned int grain_g;
    unsigned int grain_b;
    unsigned int cloud_r;     /* +0x0c: sand_model_color_set: the cloud models
                                 (sand_cloud_model_ids)                     */
    unsigned int cloud_g;
    unsigned int cloud_b;
} sand_colors_rec;

/* The sand puff configuration (12 words): grain count, grain and cloud colors, shrink and gravity steps, turn scale and the size, distance and speed ranges. */
typedef struct sand_puff_config_rec {
    undefined4 grain_count;   /* +0x0: 80 */
    uint grain_color;         /* +0x4: ARGB of the grain models */
    uint cloud_color;         /* +0x8: ARGB of the cloud models */
    undefined4 shrink;        /* +0xc: taken from a grain's size each frame */
    undefined4 gravity;       /* +0x10: taken from a grain's y speed each frame */
    undefined4 turn_scale;    /* +0x14: scales the random y turn */
    undefined4 size_min;
    undefined4 size_max;
    undefined4 dist_min;      /* +0x20: start distance */
    undefined4 dist_max;
    undefined4 speed_min;
    undefined4 speed_max;
} sand_puff_config_rec;

/* The working copy of sand_puff_config (same 12 words), which nothing reads. */
typedef struct sand_puff_config_copy_rec {
    undefined4 grain_count;
    undefined4 grain_color;
    undefined4 cloud_color;
    undefined4 shrink;
    undefined4 gravity;
    undefined4 turn_scale;
    undefined4 size_min;
    undefined4 size_max;
    undefined4 dist_min;
    undefined4 dist_max;
    undefined4 speed_min;
    undefined4 speed_max;
} sand_puff_config_copy_rec;

/* ---- vf4lib/libcard/src/card_item.c ----------------------------- */

/* The waiting-item mark (at item_pending_mark): item_pending_mark_init keeps
 * the sprite and where item_pending_mark_draw puts it. */
typedef struct item_pending_mark_rec {
    unsigned int sprite;
    float x;                  /* +0x04 */
    float y;                  /* +0x08 */
} item_pending_mark_rec;

/* One pending item (12 bytes): when it was added, the card it is for and a count. */
typedef struct item_pending_entry_rec {
    int time;                 /* item_pending_time_now when added */
    int card_id;              /* +0x04 */
    int count;                /* +0x08: 1 when added */
} item_pending_entry_rec;

/* the card log in the backup image (user_back_data +0x243C): ten {stamp, card id, count} entries, the count and the last card seen. */
typedef struct item_pending_table_rec {
    item_pending_entry_rec entries[10];
    int count;                /* +0x78: entries in use */
    undefined4 last_card_id;  /* +0x7c: -1 when none */
    int last_card_hits;       /* +0x80: item_pending_redeem calls for last_card_id */
} item_pending_table_rec;

/* ---- vf4lib/libnetwork/src/network.c: the "cmd=select" answer ------ */

/* One player's "cmd=select" answer (0x39 bytes, no padding): the reply
 * parsers fill net_select_scratch, net_select_reply_store copies the parts
 * the mask names into net_select_pending[player], and
 * net_select_pending_apply applies it once the character is loaded. */
typedef struct net_select_rec {
    unsigned char valid;      /* net_select_pending: kept until applied      */
    unsigned char mask;       /* +0x01: 1 code, 2 equip, 4 items, 8 / 0x10
                                 emblems, 0x20 colors                       */
    char access_code[17];     /* +0x02: compared with card_access_code_make's */
    unsigned char equip[8];   /* +0x13: sets 0/1 x slots 0-3                 */
    unsigned char color[12];  /* +0x1b: costume_colors_apply's values        */
    unsigned char items[16];  /* +0x27: item numbers, 0 ends                 */
    unsigned char emblem;     /* +0x37: card_body_emblem_set                 */
    unsigned char emblem_dan; /* +0x38: card_body_emblem_dan_set             */
} net_select_rec;

/* ---- vf4lib/libeffect: the snow and sand ground -------------------- */

/* The ground frame record of the snow (snow_ground_frame) and the sand
 * (sand_ground_frame): the grid builders clear both members through one
 * pool word, so it is one object, not a short[2]. */
typedef struct ground_frame_rec {
    char field_0x0;           /* only ever cleared                           */
    short frame;              /* +0x02: the ground update's frame counter    */
} ground_frame_rec;

/* ---- vf4lib/libeffect/src/effect.c -------------------------------- */

/* One running effect (24 bytes, effect_slots[50]): effect_slots_add copies
 * the rows of an effect list in, up to its -1; the task calls each hook of
 * a slot that is on. */
typedef struct effect_slot {
    int on;
    char *name;               /* +0x04 */
    void (*init)(void);       /* +0x08: effect_slots_init_all                */
    void (*exec)(void);       /* +0x0c: effect_task_exec                     */
    void (*draw)(void);       /* +0x10: effect_slots_draw_all                */
    void (*free)(void);       /* +0x14: effect_slots_free_all                */
} effect_slot;

/* ---- vf4lib/libcard/src/card_item.c (file name ours) ---------------- */

/* One player's item lottery (20 bytes, item_lottery[2]), cleared by
 * item_lottery_clear. */
typedef struct item_lottery_rec {
    int state;                /* 0, 1 (item_lottery_kind1_set), 2 (_kind2_set) */
    int roll;                 /* +0x04: item_lottery_stage_roll's draw        */
    int field_0x8;            /* +0x08: item_lottery_pending tests it with roll */
    int result;               /* +0x0c: item_lottery_judge's draw             */
    int field_0x10;
} item_lottery_rec;

/* ---- the mission text renderer (msnm_text.c, file name ours) -------- */

/* One named text color of msn_text_colors[10] ("DEF", "RED", ...
 * "CUSTOM4"): the pair msn_text_color_apply passes to sprite_colors_set. */
typedef struct msn_text_color {
    char *name;
    unsigned int color;       /* +0x04 */
    unsigned int color2;      /* +0x08 */
} msn_text_color;

/* ---- vf4lib/libsprite/src/aetset.c ------------------------------ */

/* One block of the AET stack (16 bytes): aet_stack_push hands it out,
 * aet_stack_pop gives it back and aet_stack_entry_clear clears it. */
typedef struct aet_stack_entry {
    char used;
    int set;                  /* +0x04: the push's first argument            */
    int addr;                 /* +0x08: the block, from the stack's top      */
    int size;                 /* +0x0c                                        */
} aet_stack_entry;

/* The AET stack memory (at aet_stack): blocks taken from aet_buf in push
 * order, at most 30, and at most 0xCCCCC bytes. */
typedef struct aet_stack_rec {
    int n;                    /* blocks in use                               */
    int top;                  /* +0x04: the next free byte (aet_buf at reset) */
    aet_stack_entry e[30];    /* +0x08                                        */
} aet_stack_rec;

/* ---- vf4lib/libaction/src/coli_attack.c --------------------------- */

/* One row of hit parameters (48 bytes): est_data[14], filled from
 * est_data_default by est_data_init and taken by the attack's +0x289 byte;
 * the custom record at rob +0x380 has the same layout, and
 * hit_params_custom_load copies its set members (s other than -0x8000, f
 * other than 0) over row 1. */
typedef struct est_row {
    short s[5];
    float f[9];               /* +0x0c */
} est_row;

/* ---- vf4lib/libsprite/src/aetset.c: the AET file list ------------- */

typedef struct aet_file_row {
    char *label;              /* "NICK_SK_BASE" ...                          */
    char *name;               /* +0x04: "nick_sk_base_aet.bin" ...           */
} aet_file_row;

/* The AET file list (at aet_file_table): its row count and its rows, which
 * aet_set_load takes by set number. */
typedef struct aet_file_list {
    int count;                /* 30                                          */
    aet_file_row *rows;       /* +0x04 */
} aet_file_list;

/* the word pair aet_globals_init writes: { 4, &aet_file_table }; aet_file_label_get reads the list through it */
typedef struct aet_file_table_hdr_rec {
    int count;                /* +0x0: 4 (what it counts is not established) */
    aet_file_list *list;      /* +0x4: the table of AET files */
} aet_file_table_hdr_rec;

/* ---- vf4lib/libcard/src/card_mode.c: the renewal texts ------------- */

/* One card renewal text (40 bytes, card_renew_text_table[27]): up to eight
 * line pointers, 0 ending them early; card_renew_text_draw shows line 0 of
 * text 0 in the bottom bar and the lines of the others in the panel. */
typedef struct card_renew_text {
    int field_0x0;
    int field_0x4;
    char *lines[8];           /* +0x08 */
} card_renew_text;

/* ---- vf4lib/libeffect/src/effect_etc.c: the path fish ------------- */

/* One step of a fish path script (0x1c bytes). A kind of 0 or more ends
 * the step after that many frames; -2 / -3 at an x / z goal (lim), -4 when
 * the yaw crosses lim; -5 / -6 branch to next by x / z against off, -7
 * always branches. */
typedef struct fish_path_seg {
    short kind;
    short dy;                 /* +0x02: yaw added each frame                 */
    short dx;                 /* +0x04: pitch added each frame               */
    short field_0x6;
    float spd;                /* +0x08: speed added each frame               */
    float rate;               /* +0x0c: frame rate of the fish's animation    */
    float off;                /* +0x10: the goal or branch coordinate        */
    short lim;                /* +0x14: the -4 yaw                           */
    short field_0x16;
    struct fish_path_seg *next; /* +0x18: the branch target                 */
} fish_path_seg;

/* One path-following fish (0x50 bytes): fish_paths' 150 and the single
 * fish_solo1..3; fish_path_member_init / fish_solo_setup set it up from a
 * definition, fish_path_follow runs its script. */
typedef struct fish_path_rec {
    unsigned char flags;      /* 8: waiting out wait before the first step   */
    char field_0x1;
    short frames;             /* +0x02: the animation's frame count          */
    short frame;              /* +0x04                                        */
    short ry;                 /* +0x06: yaw                                  */
    short ry_prev;            /* +0x08                                        */
    short rx;                 /* +0x0a: pitch                                */
    short turn;               /* +0x0c: pitch still to turn back             */
    short max_turn;           /* +0x0e: per frame                            */
    short rz;                 /* +0x10: roll                                 */
    short count;              /* +0x12: frames into the step                 */
    short wait;               /* +0x14: start delay                          */
    short field_0x16;
    float speed;              /* +0x18 */
    float frame_f;            /* +0x1c: the frame before rounding            */
    float rate;               /* +0x20: the fish's own frame rate            */
    float lim;                /* +0x24: the -2 / -3 goal                     */
    vec3 pos;                 /* +0x28 */
    vec3 off;                 /* +0x34: the random offset from the definition */
    float lo;                 /* +0x40: the height band                      */
    float hi;                 /* +0x44 */
    int *models;              /* +0x48: per frame (fish_path_draw)           */
    fish_path_seg *seg;       /* +0x4c: the step in hand                     */
} fish_path_rec;

/* ---- vf4lib/librobdisp: the wind --------------------------------- */

/* A wind record (0x74 bytes): bone_chain_wind for the bone chains,
 * stage_wind for the stage (wind_update steps it). The vector is the
 * base z plus eight sine waves, turned by the two angles; the debug menu
 * names bias, cycle and ratio ("wind_cycle0", "wind_ratio0" ...). */
typedef struct wind_rec {
    vec3 vec;                 /* the result                                  */
    float bias;               /* +0x0c */
    float cycle[8];           /* +0x10: each wave's speed                    */
    float ratio[8];           /* +0x30: each wave's amplitude                */
    short xang;               /* +0x50 */
    short yang;               /* +0x52 */
    float phase[8];           /* +0x54 */
} wind_rec;

/* One stage's wind (0x20 bytes, stage_wind_table[24]): what
 * stage_wind_set copies into stage_wind. Its clearing loop starts at
 * wave 2, so the third pair it copies is zeroed again and only waves 0
 * and 1 blow. */
typedef struct wind_preset {
    float bias;
    float cycle0;             /* +0x04 */
    float ratio0;             /* +0x08 */
    float cycle1;             /* +0x0c */
    float ratio1;             /* +0x10 */
    float cycle2;             /* +0x14 */
    float ratio2;             /* +0x18 */
    short xang;               /* +0x1c */
    short yang;               /* +0x1e */
} wind_preset;

/* ---- vf4lib/libeffect: the fighter puffs, audience, fish sets ------ */

/* A "STEP " puff (0x20 bytes, step_fx[64]): step_fx_spawn takes a free
 * one at a fighter's foot, step_fx_update runs its frames and
 * step_fx_draw shows it facing the camera. */
typedef struct step_fx_rec {
    uchar flags;              /* 1 running, 2 shown                          */
    char field_0x1;
    short frame;              /* +0x02 */
    short frames;             /* +0x04: step_fx_init                         */
    short field_0x6;
    int model;                /* +0x08: the first model id                   */
    float time;               /* +0x0c */
    float scale;              /* +0x10: 0.5 or 1.0 by the step's length      */
    vec3 pos;                 /* +0x14 */
} step_fx_rec;

/* An audience animation set (12 bytes, audience_anim_sets[21]). */
typedef struct audience_anim_set {
    char step;                /* > 0 plays forward, else backward            */
    char no_repeat_set;       /* +0x01: the set audience_anim_pick must not pick next (-1 none) */
    short frames;             /* +0x02 */
    int first_obj;            /* +0x04: the first object id                  */
    int *objs;                /* +0x08: the frames' models (audience_init)   */
} audience_anim_set;

/* A fish model set (0x1c bytes, fish_model_sets[5]). */
typedef struct fish_model_set {
    int first_obj;            /* the first object id of the frame run        */
    int *objs;                /* +0x04: each frame's model                   */
    int clumps3;              /* +0x08: frames of 3 merged models            */
    int clumps6;              /* +0x0c: frames of 6 merged models            */
    short frames;             /* +0x10 */
    short field_0x12;
    uint x_range;             /* +0x14: the clumps' offset ranges            */
    int y_range;              /* +0x18 */
} fish_model_set;

/* A nickname point slot (16 bytes, nickname_points[21][2]: name x 3 +
 * counter, then side); the counters take a pointer to it as short *. */
typedef struct nickname_point {
    ushort count;
    short state;              /* +0x02 */
    int work[3];              /* +0x04 */
} nickname_point;

/* A fighter effect record (0x3c bytes): rob_fx_pool[16] (the impact
 * effects, rob_fx_spawn) and down_fx[2] (one "DOWN " effect per fighter).
 * fx_anim_reset / fx_anim_step run the frame: from the frame table when
 * there is one (-1 ends it), else time itself up to the frame count. */
typedef struct fx_rec {
    uchar flags;              /* 1 running, 2 shown, 4 armed (down_fx),
                                 8 waiting for its fighter, 0x10 aimed along
                                 dir, 0x20 keeps the matrix's turn           */
    char wait;                /* +0x01: frames waited                        */
    char slot;                /* +0x02: the fighter's slot (rob +0x328)      */
    char field_0x3;
    short frames;             /* +0x04 */
    short frame;              /* +0x06 */
    short turn;               /* +0x08 */
    short field_0xa;
    int model;                /* +0x0c: the first model id, -1 none          */
    float time;               /* +0x10 */
    float speed;              /* +0x14 */
    float scale;              /* +0x18 */
    short *frame_table;       /* +0x1c */
    int rob;                  /* +0x20: the fighter, or 0                    */
    vec3 pos;                 /* +0x24 */
    vec3 dir;                 /* +0x30 */
} fx_rec;

/* A breakable stage piece (0x24 bytes): ROME's and YAM STAGE's fence rows
 * (rome_fence_row0 / 1, yam_fence_row0 / 1) and DJO STAGE's
 * djo_fence_segs (24 each), and the BREAK floor's break_floor_tiles (400);
 * each list is ended by model -1. */
typedef struct break_piece {
    vec3 pos;
    short turn;               /* +0x0c: Y turn                               */
    short field_0xe;
    int model;                /* +0x10: -1 ends the list                     */
    int model2;               /* +0x14: a fence's post after a break, a
                                 tile's cracked object                       */
    int *anims;               /* +0x18: the break animations, 0-ended        */
    short idx;                /* +0x1c: the break object (DJO: motion path,
                                 < 0 none)                                   */
    short turn2;              /* +0x1e: the post's turn                      */
    uchar flags;              /* +0x20: fences 8 hit this frame, 0x10
                                 broken; tiles 2 cracked, 4 broken           */
    char field_0x21[3];
} break_piece;

/* A DJO fence segment's fall (0x1c bytes, djo_fence_fall[24]):
 * djo_fence_fall_start starts it, djo_fence_update plays the segment's
 * motion path, djo_fence_segments_draw draws the segment there. */
typedef struct djo_fence_fall_rec {
    vec3 pos;
    short rot[3];             /* +0x0c */
    short field_0x12;
    float time;               /* +0x14 */
    uchar flags;              /* +0x18: 1 falling, 2 finished                */
    char field_0x19[3];
} djo_fence_fall_rec;

/* A rope link (0x70 bytes, rope_links[128]): the ring rope of
 * rob_rope.c. rope_link_step moves a free link (state 1) by its velocity,
 * pulls it toward its neighbors' old positions and keeps it above the
 * floor; a link a fighter holds is state 2. */
typedef struct rope_link {
    int state;                /* 1 free, 2 held or an end                    */
    float len_prev;           /* +0x04: the length to the previous link      */
    float len_next;           /* +0x08 */
    vec3 pos;                 /* +0x0c */
    vec3 old;                 /* +0x18 */
    vec3 vel;                 /* +0x24 */
    float m[16];              /* +0x30: the link's matrix, for rope_draw     */
} rope_link;

/* A rain streak (100 bytes, rain_streaks[36]; 21 near, 15 far). */
typedef struct rain_streak {
    vec3 pos;
    vec3 vel;                 /* +0x0c */
    vec3 base;                /* +0x18: the respawn point                    */
    vec3 vel0;                /* +0x24: the velocity a respawn restores      */
    vec3 shape[3];            /* +0x30: the streak's corner offsets          */
    float field_0x54;
    float dist;               /* +0x58: the horizontal distance of base      */
    float weight;             /* +0x5c: the gravity weight (x -0.00351 a frame) */
    int idx;                  /* +0x60 */
} rain_streak;

/* A "_DUST_" stage object (0x24c bytes, dust_records[20]):
 * dust_models_find takes the model and its scroll speeds, the uv pairs
 * are saved once, and dust_uv_apply writes them back each frame offset by
 * the scroll (u, v), which dust_uv_scroll_step advances and wraps at 2. */
typedef struct dust_rec {
    char flag;                /* 1: the model's vertices are 8 words, else 6 */
    char field_0x1[3];
    int n;                    /* +0x04: the vertex count (at most 69)        */
    int model;                /* +0x08 */
    float u;                  /* +0x0c */
    float v;                  /* +0x10 */
    float du;                 /* +0x14 */
    float dv;                 /* +0x18 */
    struct { float s; float t; } uv[70]; /* +0x1c: the saved texture coordinates */
} dust_rec;

/* Object memory as get_object_mem hands it out (and as the load requests
 * pass their buffers): the block, its size and one more word. */
typedef struct obj_mem {
    void *p;
    int a;
    int b;
} obj_mem;

/* A background load request (0x114 bytes, poly_queue[32]):
 * load_poly_req / objbank_free_req fill one, load_poly_bg_service steps
 * it through its states (1 load, 2 relocate and copy, 4 textures, 3 tune,
 * 5 texture load, 7 free, 8 / 9 sprite records, 0 done). */
typedef struct poly_load_entry {
    uint mask;
    int state;
    int f08;
    int tex_base;
    int bank;
    int set;
    struct obj_mem mem;
    struct static_heap_rec obj;
    struct static_heap_rec mdl;
    struct static_heap_rec buf;
    char obj_name[64];
    char mdl_name[64];
    char tex_name[68];
    struct txn_entry *txn_list;
    int node;
} poly_load_entry;

/* A NYC helicopter (0x4c bytes, nyc_helis[4]): nyc_light_init starts it
 * on its motion path, nyc_heli_update plays the path (looping from trst
 * at tmax) and spins the rotors, nyc_light_draw aims its light. */
typedef struct nyc_heli {
    float t0;                 /* the start time                              */
    float t;                  /* +0x04 */
    float tmax;               /* +0x08: the path's length                    */
    float trst;               /* +0x0c: where it loops back to               */
    float rotor_t;            /* +0x10 */
    float rotor_tmax;         /* +0x14 */
    vec3 pos;                 /* +0x18 */
    vec3 dir;                 /* +0x24: the light's direction                */
    vec3 rot;                 /* +0x30 */
    short rotor0;             /* +0x3c */
    short rotor1;             /* +0x3e */
    short field_0x40;
    short ry;                 /* +0x42: the light's aim                      */
    short rz;                 /* +0x44 */
    short field_0x46;
    char id;                  /* +0x48: the motion path (0 or 1)             */
    char field_0x49[3];
} nyc_heli;

/* A side's vp (CPU player) work record (0x88 bytes, vp_work_1p and
 * vp_work_2p): vp_work_init links it to its rob and buffers, vp_round_init
 * resets it from the two characters' tables, vp_think runs it. */
typedef struct vp_work {
    char n;                   /* the opponent table's entry count            */
    char opp_level;           /* +0x01: the opponent's attack level
                                 (vp_opp_attack_level), for vp_guard_height_set */
    char stance;              /* +0x02: character 0x0f's stance
                                 (vp_stance_from_move)                       */
    char ukemi;               /* +0x03: the tech roll type (vp_ukemi_pick):
                                 0 none, 2 up, 3 down                        */
    char rise;                /* +0x04: the rise type 1-4 (vp_mode_rise)     */
    char yoke_side;           /* +0x05: the sidestep's side (vp_mode_yoke)   */
    char field_0x6;
    char field_0x7;
    char ynt;                 /* +0x08: the dodge type (vp_yoke_start): 1
                                 sidestep, 2 escapes, 3 both                 */
    char gnt;                 /* +0x09: the guard type 0-2 (vp_guard_start)  */
    char mash;                /* +0x0a: bumped by the level's chance while
                                 staggered; its low bit picks vp_mash_inputs */
    char field_0xb;
    char attacks;             /* +0x0c: the action's attacks left; 1 the last */
    char stroke_step;         /* +0x0d: vp_stroke_result_watch's step; 0xff
                                 from vp_round_init                          */
    char run_step;            /* +0x0e: vp_stroke_run's step; 0xff           */
    char esc_step;            /* +0x0f: vp_escape_seq_run's step; 0xff       */
    char field_0x10;
    char field_0x11;
    short mode_frames;        /* +0x12: frames in the mode (vp_think)        */
    short idle_frames;        /* +0x14: frames the rob stood idle (vp_think,
                                 vp_stroke_stalled)                          */
    short react;              /* +0x16: the reaction delay before an action
                                 (vp_mode_stroke)                            */
    short field_0x18;
    short guard_react;        /* +0x1a: the reaction delay before a guard
                                 (vp_guard_height_set's callers)             */
    short seq_frames;         /* +0x1c: the input sequence row's frames left */
    short disadv;             /* +0x1e: frames behind, negative ahead
                                 (vp_frame_disadv_calc)                      */
    char field_0x20[0xc];
    uint flags;               /* +0x2c: 1 on (vp_both_enabled), 2            */
    int stroke;               /* +0x30: the own attack's result, bits 0-9
                                 STROKE_HIT .. STROKE_NOHIT                  */
    int mode;                 /* +0x34: indexes vp_mode_funcs               */
    int exc_mode;             /* +0x38: indexes vp_exc_funcs                */
    int move;                 /* +0x3c: the rob's move when it changed
                                 (vp_move_change_note)                       */
    int seq;                  /* +0x40: the running input sequence          */
    int esc_seqs[3];          /* +0x44: the escape sequences queued
                                 (vp_escape_seq_pick)                        */
    int action;               /* +0x50: a common action (vp_common_action_get) */
    int combo;                /* +0x54: the combo step list (vp_mode_combo) */
    struct vp_level_params_rec *level_params;         /* +0x58: vp_level_params_get                  */
    int chara;                /* +0x5c: this side's character table          */
    int opp_chara;            /* +0x60: the opponent's                       */
    int renkei_step;          /* +0x64: the user chain's step
                                 (vp_renkei_next_check)                      */
    int renkei;               /* +0x68: vp_user_renkei_get (debug user)      */
    short *renkei_work;       /* +0x6c: 0x38 bytes per side                  */
    struct vp_work *other;    /* +0x70: the other side's record              */
    char *traits;             /* +0x74: level, eight trait bytes, the card
                                 count (+0xa); 12 bytes per side             */
    struct rob_rec *rob;           /* +0x78: rob_get                              */
    short *history;           /* +0x7c: eight 8-byte rows                    */
    struct vp_rob_snap_rec *snap;          /* +0x80: the rob's 16-byte snapshot
                                 (vp_rob_state_snapshot)                     */
    struct vp_hame_rec *log;               /* +0x84: 64 short pairs                       */
} vp_work;

typedef int (*vp_mode_fn)(vp_work *);

/* A rob's collision record (0x460 bytes, rob_coli_records[2]; the rob
 * points at it from +0x3c): rob_coli_clear clears it, rob_coli_task_start
 * fills both sphere banks with the rob's position, robs_push_apart pushes
 * the two robs apart. */
typedef struct rob_coli_rec {
    struct rob_rec *rob;
    vec3 *sph_a;              /* +0x04: points at bank_a                     */
    vec3 *sph_b;              /* +0x08: points at bank_b                     */
    uint flags;               /* +0x0c: 1 tested, 8 pushed apart this frame  */
    int field_0x10;
    int field_0x14;
    int touch_mask;           /* +0x18: a 26-bit sphere mask: coli_sphere_sets_test takes a touching pair whose sphere of this set is in it out of the distance and marks the other set's sphere in that set's touched_by_masked (taken as 0 when bit 13 of the rob's flags24 is set) */
    int field_0x1c[2];
    int touched_by_masked;    /* +0x24: bits of this set's spheres that touched a sphere in the other set's touch_mask (ored in) */
    int field_0x28[3];
    int touch_bits;           /* +0x34: bits of this set's spheres that touched any sphere of the other set (replaced each test) */
    int touch_pairs;          /* +0x38: the number of touching pairs (the same in both sets) */
    int field_0x3c[7];        /* +0x3c-+0x57 (+0x3c, +0x48 read singly)      */
    int field_0x58;
    short sph_flag[26];       /* +0x5c */
    char field_0x90[4];
    char field_0x94;
    char field_0x95[3];
    int field_0x98;
    char field_0x9c[4];
    vec3 bank_a[26];          /* +0xa0: sphere positions                     */
    vec3 bank_b[26];          /* +0x1d8 */
    vec3 ofs[9];              /* +0x310-+0x37b: offsets (+0x34c the push,
                                 +0x364 the one rob_coli_offset applies)     */
    char field_0x37c[4];
    float field_0x380[2];
    float touch_dist;         /* +0x388: the largest closest-approach distance between centers over the touching pairs no mask took (coli_sphere_sets_test; the same in both sets) */
    float field_0x38c;
    float rad_a[26];          /* +0x390: sphere radii                        */
    float rad_b[26];          /* +0x3f8 */
} rob_coli_rec;

/* rob_rec: the fighter record (two at 0x0C37BA30 + side * 0x86C, rob_get(side)).
   Members from every matched access (offset, width, sign) and the live
   checks in docs/asset-formats.md; unexplained runs stay field_0x... */

/* the byte view of an action / request word (0x4 bytes) */
typedef struct rob_action_part rob_action_part, *Prob_action_part;

struct rob_action_part {
    short move;                              /* +0x0: move (motion) number; low 15 bits the motion id, 0x8000 a flag (key evidence: rob_move_step writes n + 0x5000000 | 0x8000; docs/asset-formats.md 'rob +0x64 = the move word') */
    uchar lever;                             /* +0x2: lever / direction byte (bits 0 / 1 the jump direction) (key evidence: action_jump_start (+0x66)) */
    char type;                               /* +0x3: action type; indexes rob_action_handlers (key evidence: rob_action_request_take ('the byte +0x67 is the action type')) */
};

/* an action or request word: read and written whole (uint) and by its parts; both readings are used by matched code (rob_action_request_take whole, rob_action_handlers dispatch on the type byte) (0x4 bytes) */
typedef union rob_action_rec rob_action_rec, *Prob_action_rec;

union rob_action_rec {
    uint word;                               /* +0x0: the whole word: type << 24 | lever << 16 | move (key evidence: rob_action_request_take, rob_move_step) */
    rob_action_part part;                    /* +0x0: the same word by parts (key evidence: rob_action_handlers dispatch on +0x63; control_ctx_* read +0x66) */
};

/* a hit counter beside its damage sum (0x4 bytes) */
typedef struct rob_combo_rec rob_combo_rec, *Prob_combo_rec;

struct rob_combo_rec {
    short hits;                              /* +0x0: hits counted (key evidence: nickname_in_combo ('the hit counters beside those damage sums')) */
    short damage;                            /* +0x2: damage summed (key evidence: rob_damage_apply (adds to +0x234 / +0x238 / +0x23C / +0x240 by kind bits 1-4)) */
};

/* one reaction row { stun, recovery end, reaction move } of the move's frame table (0x6 bytes) */
typedef struct rob_react_row_rec rob_react_row_rec, *Prob_react_row_rec;

struct rob_react_row_rec {
    short stun;                              /* +0x0: stun frames (key evidence: move_cmd_05 (patches +0x2C6..+0x2CA for result bit 0x4000)) */
    short recovery;                          /* +0x2: recovery end frame (key evidence: move_cmd_05) */
    short wait;                              /* +0x4: wait given to hit_wait_timer_set by hit_reaction (key evidence: move_cmd_05, hit_reaction) */
};

/* one limb reach target (index 0-3, swapped in pairs when mirrored) (0x24 bytes) */
typedef struct rob_limb_rec rob_limb_rec, *Prob_limb_rec;

struct rob_limb_rec {
    vec3 target;                             /* +0x0: target point (x / z negated with the bone swap when mirrored) (key evidence: move_tcmd_39_limb_target) */
    vec3 target_local;                       /* +0xc: target in the fighter's local frame (key evidence: rob_attack_reach (mtx_xform_point then vec_to_local into +0x0C)) */
    float weight;                            /* +0x18: blend weight (0 at the command) (key evidence: move_tcmd_39_limb_target (l->a = 0)) */
    float speed;                             /* +0x1c: weight step (1 / frames) (key evidence: move_tcmd_39_limb_target (l->s)) */
    char bone;                               /* +0x20: bone index + 1; 0 none (key evidence: move_tcmd_39_limb_target (l->c = c + 1), move_start clears it) */
    char field_0x21[3];                      /* +0x21 */
};

/* the attack reach / look-at state at rob +0x3F0 (rob_attack_reach's reach_state, rob_eval_matrix / rob_eval_output's q) (0xf4 bytes) */
typedef struct rob_reach_rec rob_reach_rec, *Prob_reach_rec;

struct rob_reach_rec {
    char field_0x0;                          /* +0x0 */
    uchar flags;                             /* +0x1: reach flags (bit 1 from move flag 0x4000, bit 2 reach on; 8 at round start) (key evidence: rob_attack_reach (r->flags), move_start, robs_round_init) */
    char field_0x2[14];                      /* +0x2 */
    float target[5];                         /* +0x10: reach pose target (one of the 0x0C18BE80 poses by the attacking limb) (key evidence: rob_attack_reach (r->tgt)) */
    float current[5];                        /* +0x24: reach pose now; eases toward target (key evidence: rob_attack_reach (r->cur), rob_eval_matrix (q + 0x24)) */
    vec3 look_point;                         /* +0x38: look-at point this frame (key evidence: rob_attack_reach (r->a), rob_eval_output (q + 0x38)) */
    vec3 look_target;                        /* +0x44: look-at point held (the opponent's head in the placed frame) (key evidence: rob_attack_reach (*pb = *pa); docs/asset-formats.md 'the opponent's head (rob +0x434)') */
    float field_0x50;                        /* +0x50 */
    float rate;                              /* +0x54: reach rate (key evidence: rob_attack_reach (r->rate), rob_round_reset) */
    float dist;                              /* +0x58: reach distance (0.75 of the measured one) (key evidence: rob_attack_reach (r->dist)) */
    float out;                               /* +0x5c: dist x rate, the reach applied (key evidence: rob_attack_reach (r->out), rob_eval_matrix (q + 0x5C)) */
    rob_limb_rec limbs[4];                   /* +0x60: limb reach targets (key evidence: move_tcmd_39_limb_target, rob_limbs_reset, move_start) */
    float look_weight;                       /* +0xf0: look-at blend weight (key evidence: rob_eval_output (q + 0xF0); docs/asset-formats.md 'blended in by rob +0x4E0') */
};

/* one hand's shape state (0 one hand, 1 the other) (0x14 bytes) */
typedef struct rob_hand_rec rob_hand_rec, *Prob_hand_rec;

struct rob_hand_rec {
    float field_0x0;                         /* +0x0: base level: rob_hand_step adds 0.003125 to it once (clamped 0..0.2) for target / current when flags bit 0, and never writes it back (key evidence: rob_hand_step (e->a)) */
    float target;                            /* +0x4: shape value target (key evidence: rob_hand_step (e->b), move_tcmd_13_hand_shape (a->f)) */
    float current;                           /* +0x8: shape value now; eases toward target over frames (key evidence: rob_hand_step (e->c)) */
    float smooth_target;                     /* +0xc: 10 x the motion's hand value, blended in by rob +0x3D4 when +0x3A6 bit 3 (key evidence: rob_hand_step (e->d); docs/asset-formats.md 'hand records (rob +0x504, +0x518)') */
    short frames;                            /* +0x10: frames left to reach target (key evidence: rob_hand_step (e->n), move_tcmd_13_hand_shape (d)) */
    char shape;                              /* +0x12: 31 minus the 0-31 level; -1 none (key evidence: rob_hand_step (e->v), rob_hands_reset (0xff)) */
    uchar flags;                             /* +0x13: bit 0: target / current are taken from field_0x0 (key evidence: rob_hand_step (e->f & 1), move_cmd_38 (|= 1)) */
};

/* a per-frame snapshot of the rob (ring of 4 at rob +0x588, rob_old_push); '_prev' fields are copied from the previous snapshot (0xa8 bytes) */
typedef struct rob_old_rec rob_old_rec, *Prob_old_rec;

struct rob_old_rec {
    uchar field_0x0;                         /* +0x0: rob +0x30 (key evidence: rob_old_push) */
    char attack_rate;                        /* +0x1: rob +0x176 (key evidence: rob_old_push) */
    char attack_level;                       /* +0x2: rob +0x287 (key evidence: rob_old_push) */
    char field_0x3;                          /* +0x3 */
    ushort move_no;                          /* +0x4: rob +0x08 (key evidence: rob_old_push) */
    ushort side_flags;                       /* +0x6: rob +0x318 (key evidence: rob_old_push, rep_frame_apply) */
    short frame;                             /* +0x8: rob +0x0A (key evidence: rob_old_push) */
    short active_start;                      /* +0xa: rob +0x2E6 (key evidence: rob_old_push) */
    short field_0xc;                         /* +0xc */
    short heading;                           /* +0xe: rob +0x0C (key evidence: rob_old_push) */
    short life;                              /* +0x10: rob +0x10 (key evidence: rob_old_push) */
    short field_0x12;                        /* +0x12: rob +0x2F8 (key evidence: rob_old_push) */
    short facing_offset;                     /* +0x14: rob +0x2F6 (key evidence: rob_old_push) */
    short field_0x16;                        /* +0x16: rob +0x22A (key evidence: rob_old_push) */
    short field_0x18;                        /* +0x18: rob +0x22C (key evidence: rob_old_push) */
    short stun;                              /* +0x1a: rob +0x12 (key evidence: rob_old_push) */
    uint flags;                              /* +0x1c: rob +0x18 (key evidence: rob_old_push) */
    uint flags_prev;                         /* +0x20: previous snapshot's flags (key evidence: rob_old_push) */
    uint state;                              /* +0x24: rob +0x20 (key evidence: rob_old_push) */
    uint flags24;                            /* +0x28: rob +0x24 (key evidence: rob_old_push) */
    uint flags28;                            /* +0x2c: rob +0x28 (key evidence: rob_old_push) */
    uint flags2c;                            /* +0x30: rob +0x2C (key evidence: rob_old_push) */
    uint move_flags;                         /* +0x34: rob +0x324 (key evidence: rob_old_push) */
    uint relation_flags;                     /* +0x38: rob +0x1CC (key evidence: rob_old_push) */
    uint state_prev;                         /* +0x3c: previous snapshot's state (key evidence: rob_old_push) */
    uint flags24_prev;                       /* +0x40: previous snapshot's flags24 (key evidence: rob_old_push) */
    uint flags28_prev;                       /* +0x44: previous snapshot's flags28 (key evidence: rob_old_push) */
    uint flags2c_prev;                       /* +0x48: previous snapshot's flags2c (key evidence: rob_old_push) */
    uint relation_flags_prev;                /* +0x4c: previous snapshot's relation_flags (key evidence: rob_old_push) */
    uint field_0x50;                         /* +0x50: rob +0x328 (key evidence: rob_old_push) */
    int attack_flags2;                       /* +0x54: rob +0x32C (key evidence: rob_old_push) */
    rob_action_rec action_prev;              /* +0x58: previous snapshot's action (key evidence: rob_old_push; +0x5B type byte read through old_cur) */
    rob_action_rec action;                   /* +0x5c: rob +0x60 (key evidence: rob_old_push; +0x5F type byte read through old_cur) */
    vec3 place_pos;                          /* +0x60: rob +0x84 (key evidence: rob_old_push, throw_position_follow (+0x584 +0x60)) */
    vec3 pos;                                /* +0x6c: rob +0x90 (key evidence: rob_old_push) */
    vec3 ground_pos;                         /* +0x78: rob +0x9C (key evidence: rob_old_push) */
    vec3 field_0x84;                         /* +0x84 */
    vec3 velocity;                           /* +0x90: rob +0xB4 (key evidence: rob_old_push) */
    vec3 pos_prev;                           /* +0x9c: previous snapshot's pos (key evidence: rob_old_push) */
};

/* the fighter record (0x86c bytes) */
typedef struct rob_rec rob_rec, *Prob_rec;

struct rob_rec {
    uchar side;                              /* +0x0: fighter side 0 / 1 (key evidence: robs_clear stores the side number (+0); rob_get) */
    uchar chara;                             /* +0x1: character id (key evidence: rob_chara_bind; chara_data_get index) */
    uchar costume;                           /* +0x2: costume (key evidence: rob_chara_bind, demo_rob_update) */
    uchar field_0x3;                         /* +0x3: costume-derived index (chara play counts) (key evidence: robs_start, game_stats) */
    char field_0x4[1];
    char action_step;                        /* +0x5: step of the current action handler (key evidence: rob_action_* start / update (s8 writes 51)) */
    uchar move_phase;                        /* +0x6: move phase: 0 start-up, 1 active, 2 recovery (key evidence: rob_move_step ('the move phase byte +6')) */
    char field_0x7[1];
    ushort move_no;                          /* +0x8: current move (motion) number (key evidence: rob_move_set, rob_old_push) */
    short frame;                             /* +0xa: current move frame (integer part of frame_pos) (key evidence: rob_frame_advance) */
    short heading;                           /* +0xc: facing angle (16-bit angle units) (key evidence: rob_start (s16 +2 into +0x0C), rob_move_facing) */
    short facing;                            /* +0xe: heading plus or minus facing_offset when state bit 8 (key evidence: rob_state_flags_update ('the angle +0x0E')) */
    short life;                              /* +0x10: life (key evidence: rob_round_reset (life +0x10 / +0xF8 = match_energy)) */
    short stun;                              /* +0x12: stun frames left (key evidence: rob_frames_left ('the stun +0x12'), move_tcmd_12) */
    ushort skel_heading;                     /* +0x14: heading taken by the skeleton update (key evidence: rob_skeleton_update) */
    char field_0x16[2];
    uint flags;                              /* +0x18: fighter flags (bit 0 active) (key evidence: make_old_task_frame, robs_state_clear) */
    uint flags_copy;                         /* +0x1c: copy of flags taken when not paused (key evidence: rob_skeleton_update, rob_round_reset ('copies them to +0x1C')) */
    uint state;                              /* +0x20: state flag word (key evidence: rob_move_step, docs/asset-formats.md flag words +0x20..+0x2C) */
    uint flags24;                            /* +0x24: flag word (set / clear pairs of the move commands) (key evidence: move_cmd_33_flags24; docs/asset-formats.md) */
    uint flags28;                            /* +0x28: flag word (key evidence: docs/asset-formats.md flag words +0x20..+0x2C) */
    uint flags2c;                            /* +0x2c: flag word (key evidence: docs/asset-formats.md flag words +0x20..+0x2C) */
    uchar field_0x30;                        /* +0x30: attack bits (bit 1 during an active attack) (key evidence: rob_attack_hit_check, vp_opp_attack_yokeable) */
    uchar guard_state;                       /* +0x31: guard state: 0 none, 1 standing, 3 crouching, 5 move states 0x10 / 0x20 (key evidence: rob_guard_state_update) */
    uchar air_flags;                         /* +0x32: bit 0: keep x / z while airborne in a turn (key evidence: rob_turn_step, move_cmd_02) */
    uchar turn_flags;                        /* +0x33: turn kind bits (key evidence: rob_turn_update, move_tcmd_10_state33) */
    struct rob_rec *opp;                     /* +0x34: the opponent (key evidence: rob_round_reset (rob_get of the other side)) */
    undefined *chara_data;                   /* +0x38: character record (key evidence: rob_chara_bind (+0x38 = chara_data_get(character))) */
    struct rob_coli_rec *coli;               /* +0x3c: collision record (key evidence: rob_info_window_refresh) */
    struct bone_work_rec *bones;             /* +0x40: bone records (0x6C each) (key evidence: rob_work_buffers_assign (0x13D4 bytes at 0x0C431C40)) */
    struct bone_work_rec *face_bones;        /* +0x44: face bone records (33 bones) (key evidence: rob_work_buffers_assign, face_rig_eval) */
    struct control_rec *ctrl;                         /* +0x48: control object (its +0x84 points back) (key evidence: rob_info_window_refresh) */
    float *bone_mtx;                         /* +0x4c: 31 bone matrices (key evidence: rob_work_buffers_assign; docs/asset-formats.md (pose_passes against rob +0x4C in six save states)) */
    float *bone_mtx_prev;                    /* +0x50: the other bone matrix buffer, swapped with bone_mtx each frame (key evidence: robs_frame_step ('swaps +0x4C / +0x50')) */
    float *mtx;                              /* +0x54: slot matrices (163) (key evidence: rob_work_buffers_assign (0x28C0), rob_slot_pos_get) */
    float *face_mtx;                         /* +0x58: 31 face matrices (key evidence: rob_work_buffers_assign, face_skin_update) */
    char *stats;                             /* +0x5c: versus statistics record (vs_stats row) (key evidence: robs_boot_init) */
    rob_action_rec action;                   /* +0x60: current action word (key evidence: rob_action_request_take; rob_action_handlers by part.type) */
    rob_action_rec request;                  /* +0x64: requested action / move word (key evidence: rob_action_request_take, control_ctx_down; docs/asset-formats.md 'rob +0x64 = the move word') */
    rob_action_rec request_pending;          /* +0x68: pending request, moved into request when set (key evidence: rob_action_request_take) */
    rob_action_rec request_last;             /* +0x6c: last request taken (key evidence: rob_action_request_take, rob_action_requests_take) */
    float frame_pos;                         /* +0x70: move frame position (key evidence: rob_frame_advance, move_sync_rate) */
    float frame_speed;                       /* +0x74: frames per tick (key evidence: move_tcmd_39_limb_target (divides by it), rob_round_reset) */
    char field_0x78[4];
    float floor_y;                           /* +0x7c: floor height under the rob (key evidence: rob_floor_check, rob_frames_to_ground) */
    float floor_y_kept;                      /* +0x80: floor height kept unless floor flag 0x400 or flags bit 0x100 (key evidence: rob_floor_check) */
    vec3 place_pos;                          /* +0x84: placement position (key evidence: rob_start (three position copies +0x84 / +0x90 / +0x9C), rob_velocity_apply) */
    vec3 pos;                                /* +0x90: root position (root matrix translation) (key evidence: rob_skeleton_update) */
    vec3 ground_pos;                         /* +0x9c: root x / z at placement height (key evidence: rob_skeleton_update, down_fx_update) */
    vec3 skel_place_pos;                     /* +0xa8: place_pos taken by the skeleton update when not paused (key evidence: rob_skeleton_update, rep_save_exec) */
    vec3 velocity;                           /* +0xb4: velocity (key evidence: rob_old_push ('velocity +0xB4'), move_tcmd_08_launch) */
    vec3 throw_offset;                       /* +0xc0: offset from the throw partner (key evidence: throw_position_follow, rob_velocity_apply) */
    float screen_xy[2];                      /* +0xcc: screen position in 640 x 480 pixels (key evidence: rob_info_update (proj_point_to_pixel_xy), input_log_dir_get) */
    vec3 view_pos;                           /* +0xd4: pos in the camera's view frame (key evidence: rob_info_update (mtx_xform_point)) */
    char field_0xe0[24];
    short life_max;                          /* +0xf8: life at round start (key evidence: rob_round_reset (+0xF8 = match_energy)) */
    short field_0xfa;                        /* +0xfa: frame counter; cleared on hit (key evidence: robs_frame_step, hit_result_guard_knockback) */
    short turn_target;                       /* +0xfc: facing target angle (key evidence: rob_turn_update ('facing target +0xFC')) */
    char field_0xfe[2];
    int field_0x100;                         /* +0x100: frame counter (key evidence: robs_frame_step ('counts +0x100 up')) */
    short field_0x104;                       /* +0x104: timer (lever guard; 60) (key evidence: robs_frame_step, lever_guard_check, action_lever_update) */
    short field_0x106;                       /* +0x106: timer (120 on a wall hit) (key evidence: robs_frame_step, rob_wall_hit_react) */
    short field_0x108;                       /* +0x108: -1 each frame; set from the opponent's move (key evidence: robs_frame_clear, move_tcmd_32_opponent_108_add) */
    char field_0x10a[2];
    uint field_0x10c;                        /* +0x10c: hit result bits (throws mark it) (key evidence: attack_connect) */
    uint field_0x110;                        /* +0x110 (key evidence: move_tcmd_32_opponent_108_add) */
    char field_0x114;                        /* +0x114 (key evidence: nickname_attack_open) */
    char field_0x115;                        /* +0x115: attack result byte (throws mark it) (key evidence: attack_connect) */
    char field_0x116;                        /* +0x116 */
    char field_0x117[1];
    uchar field_0x118;                       /* +0x118: tech roll window bits (key evidence: rob_info_update, control_ctx_ukemi, move_cmd_21) */
    char field_0x119;                        /* +0x119: timer (40) (key evidence: robs_frame_step, control_ctx_ukemi) */
    uchar field_0x11a;                       /* +0x11a (key evidence: action_18_update (= 1)) */
    char field_0x11b;                        /* +0x11b (key evidence: rob_flag_11b_set, rob_flag_11b_take) */
    int guard_input_hold;                    /* +0x11c: frames the guard input is held (key evidence: input_sample (through ctrl +0x84)) */
    int guard_input_time;                    /* +0x120: time of the guard input (key evidence: input_sample) */
    short field_0x124;                       /* +0x124: timer (key evidence: robs_frame_step) */
    char field_0x126[2];
    float field_0x128;                       /* +0x128: the character record's float +8 (key evidence: rob_chara_bind) */
    float wall_dist[4];                      /* +0x12c: wall distance per probe direction (key evidence: rob_floor_check) */
    float bound_dist[4];                     /* +0x13c: ring boundary distance per probe direction (key evidence: rob_floor_check) */
    short wall_angle[4];                     /* +0x14c: wall angle per probe direction (key evidence: rob_floor_check) */
    short bound_angle[4];                    /* +0x154: boundary angle per probe direction (key evidence: rob_floor_check) */
    uchar wall_bits;                         /* +0x15c: probe directions that met a wall (key evidence: rob_floor_check) */
    char field_0x15d[1];
    short body_pitch;                        /* +0x15e: body pitch (asin) (key evidence: rob_skeleton_update) */
    short body_yaw;                          /* +0x160: body yaw (atan2) (key evidence: rob_skeleton_update) */
    char field_0x162[4];
    short field_0x166;                       /* +0x166: damage tick sum (key evidence: move_tcmd_15_damage_tick, move_tcmd_12, thrown_update_common) */
    short throw_damage;                      /* +0x168: throw damage carried by the thrower (key evidence: move_cmd_15_throw_damage, thrown_update_common, throw_pair_connect) */
    short frame_disadvantage;                /* +0x16a: own frames left minus the opponent's, so above 0 is a disadvantage (key evidence: robs_frame_advantage_update; docs/asset-formats.md; seen live 2026-11-10) */
    int press_count;                         /* +0x16c: button presses counted (key evidence: control_ctx_down, control_ctx_mash) */
    uint event_flags;                        /* +0x170: event word matched against the move's event list (key evidence: move_event_branch_run, attack_hit_resolve) */
    short field_0x174;                       /* +0x174 (key evidence: rob_round_reset, move_start) */
    char attack_rate;                        /* +0x176: attack rate 0..40 (drink count) (key evidence: rob_state_flags_update, shun_drink_count_draw) */
    char attack_rate_delta;                  /* +0x177: pending attack-rate change (key evidence: rob_rate_adjust, move_tcmd_19_adjust_177) */
    uchar field_0x178;                       /* +0x178 (key evidence: move_cmd_48, rob_move_step) */
    char field_0x179[3];
    float body_mtx[16];                      /* +0x17c: body matrix (key evidence: rob_skeleton_update) */
    char followup_count;                     /* +0x1bc: follow-up count, against field_0x290 (key evidence: action_stun_followup_update, rob_action_request_take) */
    uchar followup_flags;                    /* +0x1bd: follow-up flags (key evidence: move_cmd_25) */
    char field_0x1be[2];
    uint card_flags;                         /* +0x1c0: card flags (key evidence: robs_state_clear) */
    ushort **move_tables;                    /* +0x1c4: move table list (key evidence: move_tcmd_34, action_reaction_record_reset) */
    ushort *move_table;                      /* +0x1c8: current move table (entry 0 the stance move) (key evidence: rob_pos_preset_apply, rob_throw_pick) */
    uint relation_flags;                     /* +0x1cc: relation flags to the opponent (key evidence: rob_info_clear, control_dir_request) */
    uint reach_flags;                        /* +0x1d0: reach flags (key evidence: rob_info_clear, rob_reach_flags_update) */
    uint reach_flags2;                       /* +0x1d4: second reach flag word (throws test it) (key evidence: rob_info_clear, rob_throw_pick) */
    short opp_dir;                           /* +0x1d8: direction to the opponent (key evidence: rob_info_clear, rob_turn_update) */
    char field_0x1da[2];
    float opp_dist;                          /* +0x1dc: distance to the opponent (key evidence: rob_info_clear, rob_round_reset) */
    char jump_dir;                           /* +0x1e0: jump direction 1 / -1 / 0 (key evidence: action_jump_start) */
    char field_0x1e1[1];
    short frames_to_land;                    /* +0x1e2: airborne frames to landing (key evidence: rob_frame_advance, rob_frames_to_ground) */
    char preset_throw;                       /* +0x1e4: preset throw kind (key evidence: rob_throw_pick) */
    char field_0x1e5[3];
    uint preset_throw_move;                  /* +0x1e8: preset throw move word (key evidence: rob_throw_pick) */
    ushort field_0x1ec;                      /* +0x1ec */
    char field_0x1ee[2];
    vec3 knockback;                          /* +0x1f0: knockback velocity (key evidence: hit_knockback_set) */
    vec3 slide_velocity;                     /* +0x1fc: guard slide velocity (knockback doubled) (key evidence: action_guard_stun_update) */
    vec3 push_velocity;                      /* +0x208: the move's push (key evidence: rob_velocity_apply ('the move's push (+0x208 / +0x210 before +0x2EE)')) */
    vec3 ring_out_point;                     /* +0x214: ring-out edge point (key evidence: rob_floor_check, rob_ring_out_start) */
    char field_0x220;                        /* +0x220 (key evidence: control_ctx_mash, action_15_start) */
    char slot;                               /* +0x221: current slot (matrix slot through 0x0C166604) (key evidence: rob_slot_pos_get) */
    char attack_history_idx;                 /* +0x222: index into attack_history (key evidence: attack_history_clear, attack_connect) */
    char field_0x223;                        /* +0x223: down counter (key evidence: action_down_update) */
    char field_0x224;                        /* +0x224 */
    char field_0x225[1];
    short field_0x226;                       /* +0x226: cleared each frame (key evidence: robs_frame_clear) */
    short pending_damage;                    /* +0x228: damage pending (key evidence: rob_damage_apply, move_tcmd_15_damage_tick) */
    short field_0x22a;                       /* +0x22a (key evidence: rob_old_push) */
    short field_0x22c;                       /* +0x22c (key evidence: rob_old_push) */
    short last_damage;                       /* +0x22e: last damage (key evidence: robs_hits_resolve ('last damage (+0x22E / +0x230)')) */
    short last_damage_2;                     /* +0x230: last damage (second word) (key evidence: robs_hits_resolve) */
    rob_combo_rec combo[5];                  /* +0x232: combo hit counters and damage sums (index 0-3 damage by kind bits 1-4) (key evidence: rob_combo_max_update, rob_damage_apply) */
    short combo_hits_best;                   /* +0x246: largest combo hits (key evidence: rob_combo_max_update) */
    short combo_damage_best;                 /* +0x248: largest combo damage (key evidence: rob_combo_max_update) */
    short max_wall_combo;                    /* +0x24a: max wall combo (key evidence: chara_data_page_draw label chara_max_wall_combo) */
    short max_wall_damage;                   /* +0x24c: max wall damage (key evidence: chara_data_page_draw label chara_max_wall_damage) */
    short max_combo;                         /* +0x24e: max combo (key evidence: chara_data_page_draw label chara_max_combo) */
    short max_damage;                        /* +0x250: max damage (key evidence: chara_data_page_draw label chara_max_damage) */
    short attack_history[16];                /* +0x252: move id per hit (key evidence: attack_connect, attack_history_clear) */
    char field_0x272[2];
    int field_0x274;                         /* +0x274: word set by a catch (key evidence: hit_catch_mask_check (defender +0x274)) */
    int field_0x278;                         /* +0x278: word set by a catch (attacker +0x338) (key evidence: hit_catch_mask_check) */
    uint throw_request;                      /* +0x27c: requested throws (key evidence: rob_throw_pick) */
    int facing_override;                     /* +0x280: pending facing override; -1 none (key evidence: rob_move_facing, rob_facing_override_clear) */
    uchar field_0x284;                       /* +0x284: move record byte +4 (key evidence: move_start; docs/asset-formats.md 'u8 (rob +0x284)') */
    uchar move_stance;                       /* +0x285: stance byte of the move record (+5; bit 6 a side flag) (key evidence: move_start; docs/asset-formats.md 'stance byte rob +0x285') */
    char attack_limb;                        /* +0x286: attacking limb (key evidence: robs_coli_prepare, rob_attack_reach) */
    char attack_level;                       /* +0x287: attack level (key evidence: rob_attack_reach, hit_catch_mask_check, attack_connect) */
    uchar field_0x288;                       /* +0x288 (key evidence: move_cmd_03_attack) */
    char hit_param_row;                      /* +0x289: est_data row of the hit (key evidence: attack_connect, move_cmd_03_attack) */
    uchar hit_sound;                         /* +0x28a: hit sound (key evidence: attack_result_sounds_play) */
    uchar hit_sound_2;                       /* +0x28b: hit sound with 0x20 (key evidence: attack_result_sounds_play) */
    uchar hit_sound_3;                       /* +0x28c: hit sound with 0x4000 (key evidence: attack_result_sounds_play) */
    char event_count;                        /* +0x28d: entries in event_list (key evidence: move_event_entry_find) */
    char shake_count;                        /* +0x28e: entries in shake_list (key evidence: move_hit_camera_shake) */
    char field_0x28f;                        /* +0x28f (key evidence: move_cmd_21, control_ctx_ukemi, action_15_update) */
    char field_0x290;                        /* +0x290: follow-up limit (against followup_count) (key evidence: move_cmd_25, action_stun_followup_update) */
    uchar field_0x291;                       /* +0x291 (key evidence: move_cmd_30) */
    char field_0x292;                        /* +0x292: escape counts (key evidence: move_cmd_16, script_op_63_escape) */
    char field_0x293[1];
    short field_0x294;                       /* +0x294 (key evidence: move_cmd_15_throw_damage, move_cmd_16) */
    short field_0x296;                       /* +0x296 (key evidence: move_cmd_15_throw_damage, move_cmd_16) */
    short field_0x298;                       /* +0x298 (key evidence: move_cmd_15_throw_damage) */
    short field_0x29a;                       /* +0x29a (key evidence: move_cmd_30) */
    short followup_move;                     /* +0x29c: follow-up move (key evidence: move_cmd_25, action_stun_followup_update) */
    short followup_frame;                    /* +0x29e: follow-up start frame (key evidence: move_cmd_25) */
    short attack_damage;                     /* +0x2a0: damage scaled by the attack rate (key evidence: move_cmd_03_attack, rob_pending_attack_damage) */
    short attack_damage_base;                /* +0x2a2: damage of the attack command (key evidence: move_cmd_03_attack) */
    ushort attack_cmd_flags;                 /* +0x2a4: attack command flags (bits 0 / 1 change the default hit stun) (key evidence: move_cmd_03_attack) */
    short attack_angle;                      /* +0x2a6: hit angle relative to heading (key evidence: move_cmd_03_attack, attack_connect) */
    short react_stun[5];                     /* +0x2a8: stun per reaction row (guard, hit, ...) (key evidence: move_cmd_04_frame_table) */
    short react_recovery[5];                 /* +0x2b2: recovery end per reaction row (key evidence: move_cmd_04_frame_table) */
    short react_wait[5];                     /* +0x2bc: wait per reaction row, given to hit_wait_timer_set by hit_reaction (key evidence: move_cmd_04_frame_table, hit_reaction) */
    rob_react_row_rec result_rows[4];        /* +0x2c6: rows for hit-result bits 0x4000, 0x8000, 0x10000, 0x800 (key evidence: move_cmd_05) */
    ushort field_0x2de;                      /* +0x2de (key evidence: move_cmd_00) */
    ushort field_0x2e0;                      /* +0x2e0 (key evidence: move_cmd_00) */
    ushort field_0x2e2;                      /* +0x2e2 (key evidence: move_cmd_00, rob_move_step) */
    short move_length;                       /* +0x2e4: motion frame count (key evidence: move_start; docs/asset-formats.md (mot_frames)) */
    short active_start;                      /* +0x2e6: first active frame (key evidence: rob_move_step) */
    short active_end;                        /* +0x2e8: active end frame (key evidence: rob_move_step) */
    short recovery_end;                      /* +0x2ea: recovery end frame (key evidence: rob_move_step, rob_frames_left) */
    ushort field_0x2ec;                      /* +0x2ec (key evidence: move_cmd_31) */
    short field_0x2ee;                       /* +0x2ee: frame the move's push stops (key evidence: rob_velocity_apply, move_cmd_02) */
    short field_0x2f0;                       /* +0x2f0 (key evidence: move_cmd_02) */
    short field_0x2f2;                       /* +0x2f2: frame threshold (state 0x200400) (key evidence: rob_attack_reach, move_cmd_16) */
    short field_0x2f4;                       /* +0x2f4 (key evidence: jump_move_kind) */
    short facing_offset;                     /* +0x2f6: facing offset from heading (key evidence: rob_state_flags_update, rob_move_facing) */
    short field_0x2f8;                       /* +0x2f8 (key evidence: move_cmd_32) */
    short air_frames;                        /* +0x2fa: air frames (key evidence: action_jump_update ('the air frames (+0x2FA ...')) */
    char field_0x2fc[4];
    short field_0x300[4];                    /* +0x300 (key evidence: move_cmd_21) */
    short field_0x308;                       /* +0x308 (key evidence: move_cmd_21) */
    short field_0x30a;                       /* +0x30a: frame window of field_0x33c (key evidence: move_cmd_26, rob_guard_state_update) */
    char field_0x30c[2];
    ushort field_0x30e;                      /* +0x30e */
    ushort field_0x310;                      /* +0x310 */
    short field_0x312;                       /* +0x312 */
    short field_0x314;                       /* +0x314 */
    short field_0x316;                       /* +0x316: timer (key evidence: robs_frame_step, move_tcmd_44) */
    ushort side_flags;                       /* +0x318: move side flags (bit 3 side, bit 4 pose mirror, bit 5 timed done, bit 7 list 37C) (key evidence: docs/asset-formats.md (rob_pose_mirror, toggle_side), move_cmd_28_list_37c) */
    char field_0x31a[2];
    int loop_count;                          /* +0x31c: move loop count (key evidence: move_tcmd_00_random_follow) */
    int followup_word;                       /* +0x320: follow-up move word (key evidence: move_tcmd_00_random_follow) */
    uint move_flags;                         /* +0x324: the move record's flags (key evidence: move_start; docs/asset-formats.md ('u32 flags, kept at rob +0x324')) */
    uint field_0x328;                        /* +0x328: attack word (low byte the guard motion table index; bits 0x100 / 0x200 / 0x200000 / 0x400000) (key evidence: move_cmd_03_attack, vp_opp_attack_yokeable) */
    int attack_flags2;                       /* +0x32c: copied to hit_attack_flags2 (key evidence: move_cmd_03_attack, attack_connect) */
    uint attack_mask;                        /* +0x330: attack mask the defender's catch mask must meet (key evidence: move_cmd_22, hit_catch_mask_check) */
    uint catch_mask;                         /* +0x334: catch mask (key evidence: hit_catch_mask_check) */
    uint field_0x338;                        /* +0x338 (key evidence: move_cmd_22, script_op_49_set_rob338) */
    uint field_0x33c;                        /* +0x33c: guard word (key evidence: move_cmd_26, rob_guard_state_update) */
    uint field_0x340;                        /* +0x340 (key evidence: move_tcmd_44) */
    uint field_0x344;                        /* +0x344: bit set by command (key evidence: move_cmd_50) */
    float field_0x348;                       /* +0x348 (key evidence: move_cmd_30) */
    float jump_height;                       /* +0x34c: jump target height (key evidence: action_jump_update) */
    float knockback_scale;                   /* +0x350: knockback scale (key evidence: hit_knockback_set) */
    float gravity_scale;                     /* +0x354: gravity scale (key evidence: rob_velocity_apply) */
    float jump_target_x;                     /* +0x358: jump target x (key evidence: jump_target_offset) */
    float jump_target_z;                     /* +0x35c: jump target z (key evidence: jump_target_offset) */
    char field_0x360[4];
    float field_0x364;                       /* +0x364 (key evidence: move_cmd_04_frame_table) */
    float field_0x368;                       /* +0x368 (key evidence: move_cmd_04_frame_table) */
    float field_0x36c;                       /* +0x36c (key evidence: move_cmd_43) */
    float field_0x370;                       /* +0x370 (key evidence: move_tcmd_44) */
    undefined *event_list;                   /* +0x374: the move's event list (command 0x13) (key evidence: move_event_entry_find) */
    undefined *shake_list;                   /* +0x378: camera shake list (command 27) (key evidence: move_hit_camera_shake) */
    undefined *field_0x37c;                  /* +0x37c: argument list of command 28 (key evidence: move_cmd_28_list_37c) */
    struct est_row *hit_params;                   /* +0x380: custom hit parameter row (est_row layout) (key evidence: est_data_window_draw; docs/progress.md) */
    undefined *attack_events;                /* +0x384: attack events (key evidence: attack_result_sounds_play) */
    undefined *timed_list;                   /* +0x388: the move's timed list (key evidence: motion_test_step) */
    undefined *field_0x38c;                  /* +0x38c */
    undefined *field_0x390;                  /* +0x390 */
    undefined *branch_list;                  /* +0x394: the move's branch list (key evidence: control_command_step) */
    undefined *field_0x398;                  /* +0x398 (key evidence: control_ctx_normal, move_start) */
    undefined *motion;                       /* +0x39c: motion data in the motion bank (key evidence: rob_mot_bind) */
    char field_0x3a0[4];
    uchar frame_parity;                      /* +0x3a4: toggled each frame (key evidence: robs_frame_step) */
    char field_0x3a5[1];
    ushort motion_flags;                     /* +0x3a6: motion flags (bit 3 hand blend, bit 4 _M twin, 0x1000) (key evidence: move_start, rob_hand_step; docs/asset-formats.md) */
    uchar field_0x3a8;                       /* +0x3a8 (key evidence: move_start) */
    char field_0x3a9[1];
    ushort field_0x3aa;                      /* +0x3aa (key evidence: move_cmd_47) */
    short face_motion;                       /* +0x3ac: face motion (key evidence: face_mot_bind; docs/asset-formats.md) */
    short yaw_delta;                         /* +0x3ae: yaw change of this frame (key evidence: rob_skeleton_update, rob_yaw_delta_mtx_set) */
    short field_0x3b0;                       /* +0x3b0 (key evidence: rob_skeleton_update) */
    char field_0x3b2[2];
    short field_0x3b4;                       /* +0x3b4: timer (key evidence: robs_frame_step, rob_move_frame) */
    char field_0x3b6[26];
    float blend_weight;                      /* +0x3d0: motion-change blend weight (key evidence: rob_skeleton_update, bone_pos_blend_prev) */
    float rsmooth_weight;                    /* +0x3d4: end-of-move blend weight (key evidence: rob_hand_step; docs/asset-formats.md) */
    float blend_length;                      /* +0x3d8: blend length (weight step 1 / length) (key evidence: bone_pos_blend_prev, mot_blend_start) */
    float root_offset_y;                     /* +0x3dc: root offset y (key evidence: root_mtx_blend_capture (translate(+0x3E0, +0x3DC, +0x3E8))) */
    vec3 root_offset;                        /* +0x3e0: root offset (x, unused, z) (key evidence: rob_root_offset_apply) */
    float *place_mtx;                        /* +0x3ec: placement matrix (key evidence: rob_work_buffers_assign, cam_mode_softimage) */
    rob_reach_rec reach;                     /* +0x3f0: attack reach and look-at state (key evidence: rob_attack_reach, rob_eval_output) */
    struct rig_entry *rig;                     /* +0x4e4: rig (body_rigs) (key evidence: robs_clear) */
    undefined *channel_pool;                 /* +0x4e8: channel pool (key evidence: rob_work_buffers_assign) */
    undefined *link_pool;                    /* +0x4ec: link pool (key evidence: rob_work_buffers_assign) */
    undefined *face_channel_pool;            /* +0x4f0: face channel pool (key evidence: rob_work_buffers_assign) */
    undefined *face_link_pool;               /* +0x4f4: face link pool (key evidence: rob_work_buffers_assign) */
    rob_hand_rec hands[2];                   /* +0x4f8: hand shape records (key evidence: rob_hands_reset, rob_hand_step) */
    int rigid_face;                          /* +0x520: rigid face model id; -1 none (key evidence: face_rigid_pick, rob_face_reset) */
    short face_timer;                        /* +0x524: face flag timer (key evidence: rob_face_reset) */
    char field_0x526[2];
    float fl_rate;                           /* +0x528: fl_rate (key evidence: debug menu items rob0_fl_rate / rob1_fl_rate (0x0C37BF58 / 0x0C37C7C4)) */
    uchar face_flags;                        /* +0x52c: face flags (bit 1 voice cue, bit 3 held) (key evidence: face_rigid_pick, rob_face_reset) */
    uchar eye_look;                          /* +0x52d: eyes look at a point (timed command 41) (key evidence: move_tcmd_41_eye_look; docs/asset-formats.md) */
    short eye_angles[6];                     /* +0x52e: eye angles (key evidence: face_rig_walk ('the eye angles at rob +0x52E..')) */
    short field_0x53a;                       /* +0x53a */
    char field_0x53c[4];
    uint body_model;                         /* +0x540: body model word (key evidence: rob_draw_full, rob_cloth_init) */
    uchar field_0x544;                       /* +0x544: debug numbers switch (key evidence: rob_debug_numbers_draw, control_ctx_mash) */
    char grip_mode;                          /* +0x545: rope grip mode (key evidence: move_tcmd_18, rope_grips_update) */
    char grip_bone;                          /* +0x546: rope grip bone (key evidence: move_tcmd_18, rope_grips_update) */
    char field_0x547[1];
    short grip_link;                         /* +0x548: rope grip link (key evidence: move_tcmd_18, rope_grips_update) */
    char field_0x54a[2];
    undefined *reaction;                     /* +0x54c: reaction record (key evidence: action_reaction_record_reset, attack_hit_resolve) */
    char stance;                             /* +0x550: stance (picks the control set) (key evidence: control_context_update, vp_stance_get) */
    char field_0x551;                        /* +0x551 (key evidence: rob_round_reset) */
    char field_0x552[2];
    float field_0x554;                       /* +0x554 (key evidence: rob_round_reset) */
    float field_0x558;                       /* +0x558 (key evidence: rob_round_reset) */
    uchar approach_mode;                     /* +0x55c: approach mode (key evidence: move_tcmd_22_approach, rob_velocity_apply) */
    char field_0x55d[1];
    short approach_frame;                    /* +0x55e: approach end frame (key evidence: rob_velocity_apply) */
    vec3 approach_target;                    /* +0x560: approach target (key evidence: rob_velocity_apply) */
    undefined *items;                        /* +0x56c: card item slots (key evidence: rob_item_colors_apply) */
    uchar field_0x570;                       /* +0x570: parked demo byte (key evidence: demo_rob_update) */
    uchar field_0x571;                       /* +0x571: parked demo byte (key evidence: demo_rob_update) */
    uchar model_slot;                        /* +0x572: model slot (key evidence: demo_start, demo_rob_update) */
    uchar field_0x573;                       /* +0x573: parked demo byte (key evidence: demo_rob_update) */
    ushort field_0x574;                      /* +0x574 (key evidence: demo_rob_update) */
    uchar win_pose;                          /* +0x576: last win pose (key evidence: win_pose_pick) */
    uchar cam_bits;                          /* +0x577: camera bits 0-2 (key evidence: cam_mode_rob, move_tcmd_16) */
    uchar field_0x578;                       /* +0x578 (key evidence: rob_motion_set) */
    char slow_frames;                        /* +0x579: slowed frames (key evidence: rob_frame_advance, hit_cmd49_absorb_check) */
    char field_0x57a[2];
    float slow_rate;                         /* +0x57c: slowed frame rate (key evidence: rob_frame_advance) */
    int old_idx;                             /* +0x580: snapshot index (key evidence: rob_old_push, make_old_task_start) */
    rob_old_rec *old_cur;                    /* +0x584: the newest snapshot (key evidence: rob_old_push) */
    rob_old_rec old[4];                      /* +0x588: snapshot ring (key evidence: rob_old_push) */
    short facing_deg;                        /* +0x828: facing in degrees (key evidence: robs_frame_step) */
    short heading_deg;                       /* +0x82a: heading in degrees (key evidence: robs_frame_step) */
    short facing_offset_deg;                 /* +0x82c: facing_offset in degrees (key evidence: robs_frame_step) */
    short body_yaw_deg;                      /* +0x82e: body_yaw in degrees (key evidence: robs_frame_step) */
    short attack_angle_deg;                  /* +0x830: attack_angle in degrees (key evidence: robs_frame_step) */
    char field_0x832[6];
    short frame_adv_guard;                   /* +0x838: frame advantage on guard (key evidence: frame_advantage (debug menu)) */
    short frame_adv_hit;                     /* +0x83a: frame advantage on hit (key evidence: frame_advantage (debug menu)) */
    short frame_adv_hit_2;                   /* +0x83c: frame advantage on hit (second case) (key evidence: frame_advantage (debug menu)) */
    char field_0x83e[4];
    short damage_last;                       /* +0x842: last damage applied (key evidence: rob_damage_apply) */
    short damage_sum;                        /* +0x844: damage applied, summed (key evidence: rob_damage_apply) */
    char field_0x846[38];
};

/* A snow flake (0x28 bytes, snow_flakes[150]): snow_flakes_sheets_init
 * scatters it, snow_flakes_update drops it (respawning above the camera),
 * snow_flakes_draw writes its triangle into the flake model. The DJO snow
 * draws the first six as big flakes, spinning them by spin. */
typedef struct snow_flake {
    vec3 pos;
    vec3 home;                /* +0x0c: the starting x, top and z            */
    float weight;             /* +0x18: 1 to 2                               */
    float *tri;               /* +0x1c: its triangle in the flake model
                                 (snow_flakes_bind_model)                    */
    uchar sway;               /* +0x20: its snow_sway circle (0-15)          */
    char field_0x21[3];
    int spin;                 /* +0x24: a random start angle                 */
} snow_flake;

/* A sway circle (16 bytes, snow_sway[16]): snow_sway_update turns ang by
 * spd and sets x / y on a circle of radius r; the flakes add x. */
typedef struct snow_sway_circle {
    short ang;
    short spd;
    float x;                  /* +0x04 */
    float y;
    float r;                  /* +0x0c */
} snow_sway_circle;

/* A falling snow sheet (0x28 bytes, snow_sheets[50]): placed round the
 * camera at angle place and distance r, with three heights that fall by
 * spd and wrap. The spot tables give r, spd and place (written as a
 * word; snow_sheets_update reads place as a short). */
typedef struct snow_sheet_spot {
    float r;                  /* the sheet's radius offset; -1 ends the list */
    float speed;
    int angle;                /* 0x8000 and so on, copied into the sheet's angle word */
} snow_sheet_spot;

typedef struct snow_sheet {
    float x;
    float z;                  /* +0x04 */
    float y[3];               /* +0x08 */
    float r;                  /* +0x14 */
    float spd;                /* +0x18 */
    short place;              /* +0x1c */
    short field_0x1e;
    int spin;                 /* +0x20: a random start angle, turned against
                                 the camera's yaw                            */
    char on;                  /* +0x24 */
    char field_0x25[3];
} snow_sheet;

/* An advertise-mode effect animation (8 bytes, adv_fx_anims[6]):
 * adv_fx_init starts t at a random frame, adv_fx_update steps t and keeps
 * its whole part in frame (0 to 79), adv_fx_draw draws model 0x170000 +
 * frame at each position. */
typedef struct adv_fx_anim {
    short frame;
    short field_0x2;
    float t;                  /* +0x04 */
} adv_fx_anim;

/* A NYC searchlight (0x70 bytes; nyc_searchlights[2] follow the fighters,
 * nyc_searchlights_fixed[2], nyc_heli_light0 / 1): nyc_light_init sets
 * the color, ranges and aim, nyc_searchlights_update follows a fighter
 * (or the stage center) and turns the lamp, nyc_searchlight_draw draws
 * it. */
typedef struct nyc_searchlight {
    struct vec3 aim;
    struct vec3 pos;
    float aim_radius_outer;
    float aim_radius_inner;
    struct light_src_rec light;
    float aim_dist;
    short pitch;
    short yaw;
    short roll;
    char model;
    char flags;
} nyc_searchlight;

/* One TA buffer's four area offsets (gfx_state_rec.area_offsets): region array, object list, ISP/TSP parameters, TSP. */
typedef struct gfx_ta_areas {
    undefined4 region;
    undefined4 objlist;
    undefined4 isp;
    undefined4 tsp;
} gfx_ta_areas;

/* A gfx_state_rec hook: the function and the argument it is called with. */
typedef struct gfx_hook {
    void (*fn)(undefined4);
    undefined4 arg;
} gfx_hook;

/* One TA buffer's record (gfx_state_rec.ta_bufs, 0x38 bytes): its bases and limits, split word and list ends. */
typedef struct gfx_ta_buf {
    undefined4 base;
    undefined4 region_base;
    undefined4 field_0x8;
    undefined4 ol_base;
    undefined4 ol_limit;
    undefined4 opb_start;
    undefined4 isp_base;
    undefined4 isp_limit;
    undefined4 split_word;
    undefined4 end_a;
    undefined4 end_b;
    undefined4 last_a;
    undefined4 last_b;
    undefined4 ends_pending;
} gfx_ta_buf;

/* The graphics library's state block (0x514 bytes, gfx_state). The
 * member types are the ones the sign and pointer passes gave the
 * generated layout; members not named here keep their offsets. */
typedef struct gfx_state_rec {
    undefined4 info_ptr;      /* +0x0: lib_state_set_up_from_cfg points it at the info record at +0x514 (words 4, 1, 4, 0 and the credits text) */
    undefined4 chip_rev;      /* +0x4: pvr_regs_defaults_and_vram_clear sets 5 or 6 by the chip revision (hangs unless 6); spg_timing_ntsc/pal/vga and the register defaults branch on 6 */
    undefined4 flags; /* +0x8: bit 2: put surfaces in the half with more room */
    undefined1 unused_c[8]; /* +0xc..+0x13: no access in the game reaches these bytes (build/structs/nn2/gfx_reach.py, 2026-11-04) */
    undefined4 field_0x14; /* +0x14: lib_state_set_up_from_cfg stores 0x10000; nothing reads it */
    undefined4 vram_size; /* +0x18: the texture memory size (8 or 16 MB) */
    undefined4 field_0x1c; /* +0x1c: lib_state_set_up_from_cfg stores 0x800000 (the 8 MB default, beside vram_size); nothing reads it */
    undefined4 video_out;     /* +0x20: the video output 0-2 (KAMUI VideoMode) lib_start_with_video_out keeps; display_timing_and_fb_program picks spg_timing_ntsc / vga / pal by it */
    undefined4 display_mode;  /* +0x24: display mode bits (interlace, 640 wide 0x10, 480 high 0x20) kept by display_timing_and_fb_program, read by fb_size_from_display_mode, the spg_timing_* fillers and render_start_ready_frame */
    undefined4 fb_format;     /* +0x28: frame buffer pixel format kept by display_timing_and_fb_program / fb_vram_alloc; bytes a pixel by it in fb_size_from_display_mode */
    undefined4 dither;        /* +0x2c: dither flag kept by display_timing_and_fb_program and fb_rebuild_dither_aa */
    undefined4 anti_alias;    /* +0x30: anti-alias flag kept by display_timing_and_fb_program and fb_rebuild_dither_aa */
    undefined4 fb_sof_hold;   /* +0x34: fb_vram_alloc / render_buffers_lay_out mode word; when 1 render_start_ready_frame and display_flip_rendered leave the frame buffer start registers alone */
    undefined4 top_skip_mode; /* +0x38: fb_top_lines_skip_set keeps its mode (1 hides the top lines) */
    int top_skip_lines;       /* +0x3c: fb_top_lines_skip_set keeps the hidden line count; vo_start_offset_apply clamps against it */
    undefined4 read_width;    /* +0x40: fb_reg_images_pack keeps the read width (the FB_R_SIZE x field) */
    undefined4 read_height;   /* +0x44: fb_reg_images_pack keeps the read height; vo_start_offset_apply shortens FB_R_SIZE from it */
    undefined4 fb_width;      /* +0x48: fb_layout_compute keeps the frame width; close_lists_and_draw_to_surface clamps the target width to it */
    undefined4 fb_height;     /* +0x4c: fb_layout_compute keeps the frame height; close_lists_and_draw_to_surface clamps the target height to it */
    undefined4 vo_offset_x;   /* +0x50: vo_start_offset_apply keeps the picture x shift added to VO_STARTX */
    int vo_offset_y;          /* +0x54: vo_start_offset_apply keeps the y shift; spg_hblank_line_write / spg_status_display_line add it to the vertical start */
    int vo_start_x;           /* +0x58: spg_timing_load keeps the base VO_STARTX value */
    int vo_start_y1;          /* +0x5c: spg_timing_load keeps the first field's base vertical start; spg_hblank_line_write adds it */
    int vo_start_y2;          /* +0x60: spg_timing_load keeps the second field's base vertical start */
    undefined4 pool_ta_status; /* +0x64: lib_state_set_up_from_cfg stores the RENDER PoolingTAStatus setting */
    undefined4 list_ctrl;     /* +0x68: ta_buffers_lay_out keeps its list control word (bit 0 column order, low byte reused by opb_size_word_build) */
    gfx_ta_areas area_offsets[2];
    undefined4 area_ends[2];
    undefined4 isp_param_size; /* +0x94: lib_state_set_up_from_cfg stores the RENDER ISPParamSize setting */
    undefined4 tsp_param_size; /* +0x98: lib_state_set_up_from_cfg stores the RENDER TSPParamSize setting */
    undefined4 pal_format;    /* +0x9c: pvr_pal_ram_ctrl_put keeps the palette format written to PAL_RAM_CTRL */
    undefined4 alpha_cutoff;  /* +0xa0: fb_w_ctrl_alpha_cutoff keeps the alpha threshold it puts in FB_W_CTRL */
    undefined4 bg_plane[62];
    undefined4 bg_plane_bytes; /* +0x19c: the bg_plane_from_* loaders store the background plane block's byte size */
    undefined4 bg_isp_word;   /* +0x1a0: background plane ISP/TSP instruction word: bg_plane_context_store copies it from the context, the loaders put it first in the block */
    undefined4 bg_tsp_word;   /* +0x1a4: background plane TSP word (bg_plane_context_store, the bg_plane_from_* loaders) */
    undefined4 bg_tex_word;   /* +0x1a8: background plane texture control word (bg_plane_context_store, the bg_plane_from_* loaders) */
    undefined4 bg_tag_bits;   /* +0x1ac: upper bits of the background tag kept by bg_plane_tag_bits_set; ta_input_finish and tex_target_regs_load add the plane offset */
    uint bg_depth;            /* +0x1b0: the background plane's depth word
                                 (bg_plane_defaults_set, the bg_plane_from_*
                                 loaders); render_start_ready_frame writes it
                                 & ~0xf to ISP_BACKGND_D                    */
    undefined4 bg_tag;        /* +0x1b4: the full background tag ta_input_finish builds; render_start_ready_frame writes it to ISP_BACKGND_T (0x8c) */
    undefined4 bg_face_color;  /* +0x1b8: bg_plane_context_store packs the context face color as ARGB */
    undefined4 bg_offset_color;/* +0x1bc: bg_plane_context_store packs the context offset color as ARGB */
    undefined4 region_rebuild;/* +0x1c0: two-bit mask of region arrays to rebuild: region_modvol_lists_merge / region_presort_flip_on_match set 3, render_start_ready_frame clears a buffer's bit after tile_regions_write */
    undefined4 presort_flag;  /* +0x1c4: the per-tile pre-sort bit tile_regions_write puts in every control word; region_presort_flip_on_match flips it */
    undefined4 tile_limit_on; /* +0x1c8: when 1 tile_regions_write marks tiles past +0x1cc / +0x1d0 as not drawn */
    undefined4 tile_limit_x;   /* +0x1cc: last tile column tile_regions_write draws when tile_limit_on */
    undefined4 tile_limit_y;   /* +0x1d0: last tile row tile_regions_write draws when tile_limit_on */
    undefined4 modvol_merge;  /* +0x1d4: region_modvol_lists_merge turns it on; tile_regions_write then points both modifier list words at one list */
    undefined4 modvol_merge_mode;/* +0x1d8: region_modvol_lists_merge keeps the mode: 1 the opaque one, else the translucent one */
    undefined4 strip_len_code;/* +0x1dc: default 3 (lib_state_set_up_from_cfg); context_hw_words_compute puts it in bits 18-19 of the parameter control word */
    int vsync_countdown;      /* +0x1e0: V-blanks left before a flip; vblank_skip_check counts it down and reloads it from +0x1e4 */
    undefined4 vsync_count;   /* +0x1e4: RENDER VSYNCCount - 1 (lib_state_set_up_from_cfg, frame_vblank_quota_put); vblank_skip_check reloads the countdown from it */
    undefined4 fb_pair_word;  /* +0x1e8: fb_pair_install keeps its fourth argument */
    undefined4 field_0x1ec;   /* +0x1ec: cleared by pvr_irq_entry_and_masks_set with pending_events and tex_render_on; never read */
    undefined1 unused_1f0[24]; /* +0x1f0..+0x207: no access in the game reaches these bytes (build/structs/nn2/gfx_reach.py, 2026-11-04) */
    undefined4 render_busy;   /* +0x208: render busy word: render_start_ready_frame / tex_target_regs_load set 3, render_done_irq clears each chip's bit; tex_render_wait and render_drain wait on it */
    undefined4 ta_busy;       /* +0x20c: list input in progress: frame_lists_send_and_rearm / ta_start_queued_frame set 1, ta_input_finish clears it; render_pipeline_choose waits on it */
    undefined1 unused_210[8]; /* +0x210..+0x217: no access in the game reaches these bytes (build/structs/nn2/gfx_reach.py, 2026-11-04) */
    int field_read_offsets[2];
    undefined1 unused_220[4]; /* +0x220..+0x223: no access in the game reaches these bytes (build/structs/nn2/gfx_reach.py, 2026-11-04) */
    undefined4 render_timeout;/* +0x224: RENDER RenderTimeOut / frame_stall_limits_put: V-blanks before render_stall_check resets a render */
    undefined4 vertex_timeout;/* +0x228: RENDER VertexTimeOut / frame_stall_limits_put: V-blanks before vertex_stall_check drops a frame */
    undefined4 vsync_timeout;  /* +0x22c: lib_state_set_up_from_cfg stores the RENDER VSYNCTimeOut setting */
    gfx_hook hooks[9];
    undefined1 unused_278[4]; /* +0x278..+0x27b: no access in the game reaches these bytes (build/structs/nn2/gfx_reach.py, 2026-11-04) */
    undefined4 field_0x27c;   /* +0x27c: close_lists_and_draw_to_surface stores a local back into it that nothing assigns (the game writes the register it held); never read */
    undefined1 unused_280[32]; /* +0x280..+0x29f: no access in the game reaches these bytes (build/structs/nn2/gfx_reach.py, 2026-11-04) */
    undefined4 display_regs_pending;/* +0x2a0: set by fb_pair_install, fb_rebuild_dither_aa, display_blank_and_quiesce; display_flip_rendered then writes VO_CONTROL and FB_R_CTRL and clears it */
    undefined4 field_0x2a4;   /* +0x2a4: cleared by hook_slots_defaults before it installs the hooks; never read */
    undefined4 pending_events;/* +0x2a8: event bits raised by the interrupt handlers (render end 0, V-blank 1, H-blank 2, list end 5, YUV end 6); pvr_irq_service runs hooks[i] for each and clears it */
    undefined4 x_clip_word;   /* +0x2ac: packed FB_X_CLIP image (fb_clip_rect_store, fb_reg_images_pack); written at render start and by fb_regs_reload_blanked */
    undefined4 y_clip_word;   /* +0x2b0: packed FB_Y_CLIP image (fb_clip_rect_store, fb_reg_images_pack) */
    undefined4 field_0x2b4;   /* +0x2b4: packed by fb_reg_images_pack from its parameters 18-20 (bits 0-3, 4-7, 9-12) beside x_clip_word / y_clip_word; never read */
    undefined4 read_size_word;/* +0x2b8: FB_R_SIZE image built by fb_reg_images_pack; fb_regs_reload_blanked and vo_start_offset_apply write it */
    undefined4 line_stride_word;/* +0x2bc: FB_W_LINESTRIDE image (fb_reg_images_pack); written at render start */
    undefined4 read_ctrl_word;/* +0x2c0: FB_R_CTRL image (fb_reg_images_pack); vo_blank_per_chip, display_blank_and_quiesce clear its enable bit when writing it */
    undefined4 write_ctrl_word;/* +0x2c4: FB_W_CTRL image (fb_reg_images_pack, fb_w_ctrl_alpha_cutoff); written at render start, its pack mode replaced by fb_pack_mode_from_texture */
    undefined4 fb_r_sof1;     /* +0x2c8: fb_pair_install sets it to the first frame buffer and writes it to FB_R_SOF1 (0x50) */
    undefined4 fb_r_sof2;      /* +0x2cc: fb_pair_install sets it to the second frame buffer (FB_R_SOF2, 0x54) */
    undefined1 unused_2d0[8]; /* +0x2d0..+0x2d7: no access in the game reaches these bytes (build/structs/nn2/gfx_reach.py, 2026-11-04) */
    undefined4 sync_ctrl_word;/* +0x2d8: SPG_CONTROL image spg_timing_load packs and writes to 0xd0 */
    undefined4 hblank_int_word;/* +0x2dc: SPG_HBLANK_INT image (spg_timing_load); spg_hblank_line_write replaces its line field */
    undefined4 vblank_int_word;/* +0x2e0: SPG_VBLANK_INT image spg_timing_load writes to 0xcc */
    undefined4 hblank_word;    /* +0x2e4: SPG_HBLANK image spg_timing_load writes to 0xd4 */
    undefined4 sync_load_word;/* +0x2e8: SPG_LOAD image spg_timing_load writes to 0xd8; spg_hblank_line_write / spg_status_display_line take the line count from bits 16-25 */
    undefined4 vblank_word;    /* +0x2ec: SPG_VBLANK image spg_timing_load writes to 0xdc */
    undefined4 sync_width_word;/* +0x2f0: SPG_WIDTH image spg_timing_load writes to 0xe0 */
    undefined4 y_coeff_word;  /* +0x2f4: Y_COEFF image (fb_reg_images_pack), written to 0x118 by fb_regs_reload_blanked */
    undefined4 scaler_word;   /* +0x2f8: SCALER_CTL image (fb_reg_images_pack); render_start_ready_frame writes it with the field bit */
    undefined4 vo_ctrl_word;  /* +0x2fc: VO_CONTROL image (spg_timing_load); written with or without the blank bit 8 by vo_blank_per_chip, display_flip_rendered */
    undefined4 vo_startx_word;/* +0x300: VO_STARTX image (spg_timing_load, vo_start_offset_apply) */
    undefined4 vo_starty_word; /* +0x304: VO_STARTY image (spg_timing_load, vo_start_offset_apply) */
    undefined1 unused_308[4]; /* +0x308..+0x30b: no access in the game reaches these bytes (build/structs/nn2/gfx_reach.py, 2026-11-04) */
    undefined4 core_reg98_value; /* +0x30c: pvr_regs_defaults_and_vram_clear: 0, or 0x800408 on revision-6 chips, then written to core register 0x98 there */
    undefined4 display_end_line;/* +0x310: spg_timing_ntsc/pal/vga: the line the picture ends on (vertical start + display_lines) */
    undefined4 display_start_line;/* +0x314: spg_timing_ntsc/pal/vga: the line the picture starts on (vertical blank end) */
    undefined4 display_lines; /* +0x318: 240 or 480, set by display_timing_and_fb_program and the spg_timing_* fillers */
    undefined1 unused_31c[16]; /* +0x31c..+0x32b: no access in the game reaches these bytes (build/structs/nn2/gfx_reach.py, 2026-11-04) */
    undefined4 fb_addrs[2];
    undefined4 field_0x334;   /* +0x334: cleared by fb_pair_install after it stores fb_addrs[0]; never read */
    undefined4 field_0x338;   /* +0x338: cleared by fb_pair_install after it stores fb_addrs[1]; never read */
    undefined4 single_bank;   /* +0x33c: render_pipeline_choose / render_buffers_lay_out (config bit 0x10): 1 sends the lists at frame end (frame_lists_send_and_rearm), 0 queues ring slots */
    undefined1 unused_340[4]; /* +0x340..+0x343: no access in the game reaches these bytes (build/structs/nn2/gfx_reach.py, 2026-11-04) */
    undefined4 slot_states[4];
    undefined4 frame_idx; /* +0x354: the frame being built */
    undefined4 render_slot;   /* +0x358: the frame slot being rendered, set by render_start_ready_frame; fb_pixel_addrs_by_chip picks the frame buffer by its bit 0 */
    undefined4 ta_slot;       /* +0x35c: the frame slot taking vertices, set by ta_start_queued_frame / frame_lists_send_and_rearm; ta_input_finish marks it complete */
    undefined4 send_mask;     /* +0x360: mask of lists to send, copied from used_lists by frame_lists_send_and_rearm and render_setup_finish; lists_send_to_ta hands it to bank_close_and_submit */
    undefined4 field_0x364;   /* +0x364: cleared at each list send, as send_mask takes used_lists (frame_lists_send_and_rearm, render_setup_finish, close_lists_and_draw_to_surface, redraw_lists_to_surface, ta_lists_rearm); never read */
    undefined4 sent_list;      /* +0x368: ta_start_queued_frame keeps the number of the list it queued last */
    undefined4 vblank_ticks; /* +0x36c: counted by the V-blank interrupt */
    undefined4 frame_pending; /* +0x370: set when a frame is queued or sent (frame_lists_send_and_rearm, ta_direct_lay_out), cleared by vblank_irq; frame_lists_send_and_rearm waits on it */
    undefined4 tex_render_on; /* +0x374: a render to texture is running: frame_slots_stall_scan restores the frame write registers after 16 V-blanks, ta_input_finish skips the slot update */
    undefined4 stall_scan_hold;/* +0x378: set by frame_lists_send_and_rearm / redraw_lists_to_surface on the direct path; frame_slots_stall_scan returns at once while set */
    int slot_list_start[5][4];
    undefined4 slot_list_bytes[5][4];
    undefined4 slot_split_words[4];
    undefined4 kept_list;     /* +0x42c: 0x20 + the list kept open across
                                 banks, 0 none; bank_lists_seed writes no
                                 blank block for it                          */
    undefined4 direct_list;   /* +0x430: low four bits of the list mode (render_pipeline_choose): the list sent straight to the TA, skipped by lists_send_to_ta and scene_lists_close */
    undefined4 list_dma_ticket;/* +0x434: listbuf_list_transfer keeps the transfer ticket; listbuf_transfer_done polls it */
    undefined4 upload_ticket; /* +0x438: vram64_write keeps the texture DMA ticket; vram_upload_dma_idle polls it */
    undefined4 upload_wait;   /* +0x43c: when 1 vram64_write waits for each texture DMA (ticket_wait); render_buffers_lay_out sets it from config bit 0x100000 */
    undefined4 mv_hdr_on;     /* +0x440: when 1 lists_send_to_ta first sends the header at +0x444 before modifier lists 1 and 3 */
    uint *mv_hdr;              /* +0x444: header block lists_send_to_ta patches with the list number and sends before lists 1 and 3 */
    undefined4 mv_hdr_bytes;   /* +0x448: byte size of that header block (lists_send_to_ta) */
    undefined4 vertex_buf_base;/* +0x44c: RENDER VertexBuffer BufferBase (lib_state_set_up_from_cfg, vertex_area_base_size_put); listbuf_lists_carve starts the list areas there */
    undefined4 vertex_buf_size;/* +0x450: VertexBuffer BufferSize (lib_state_set_up_from_cfg, vertex_area_base_size_put) */
    undefined4 ta_mem_size;   /* +0x454: ta_memory_defaults_set stores 8 MB and puts the first TA buffer at half of it */
    undefined4 param_area_size;/* +0x458: ta_buffers_lay_out: three quarters of ta_buf_size, the parameter area of each TA buffer */
    undefined4 field_0x45c; /* +0x45c: ta_memory_defaults_set stores 0x600000 and copies it to +0x460; nothing else reads it */
    undefined4 field_0x460; /* +0x460: ta_memory_defaults_set's copy of +0x45c (re-read through a volatile view) */
    undefined4 ta_buf_size;   /* +0x464: size of each TA buffer (chips_pool_irq_set_up 1 MB, the layout routines from the set-up record); ta_buffers_lay_out places the region array at its end */
    undefined4 tile_cols;     /* +0x468: the tile grid's width
                                 (tile_regions_write, tile_split_bitmap_update) */
    undefined4 tile_rows;     /* +0x46c: its height                         */
    undefined4 double_regions;/* +0x470: opb_size_word_build: lists 1, 2 and 4 all used under option 0x20; ta_buffers_lay_out reserves 96 bytes a tile and tile_regions_write adds the never-drawn triangle */
    undefined4 used_lists;    /* +0x474: used-list mask built by used_lists_mask_set; scene_lists_close, list_dummy_headers_write and the send code test it */
    gfx_ta_buf ta_bufs[2];
    undefined4 opb_downward;  /* +0x4e8: bit 16 of the list control word (ta_buffers_lay_out): object lists placed below the parameters and growing down; enables ta_area_split_shift in the overflow handlers */
    undefined4 opb_codes[5];
    undefined4 list_limits[5];
} gfx_state_rec;

/* The info record (lib_info, 0x0C41E524, right after gfx_state, which points
 * at it from info_ptr): lib_state_set_up_from_cfg fills it (words 4, 1, 4, 0
 * and the credits text); the code reaches it from its own address only. */
typedef struct lib_info_rec {
    undefined4 info_words[4];
    undefined4 credits_text; /* +0x10: the driver's credits text (nec_copyright) */
} lib_info_rec;

/* The render statistics (render_stats, 0x0C41E53C): stall, overflow and
 * time counters the interrupt handlers keep and render_stats_read reports;
 * the code reaches them from this address only. */
typedef struct render_stats_rec {
    undefined4 render_stall_ticks;/* +0x0: render_stall_check's V-blank counter (cleared at each render start) */
    undefined4 vertex_stall_ticks;/* +0x4: vertex_stall_check's V-blank counter (cleared when lists are sent) */
    undefined4 render_stalls; /* +0x8: render stalls counted by render_stall_check and frame_slots_stall_scan; render_stats_read reports it */
    undefined4 vertex_stalls; /* +0xc: vertex stalls counted by vertex_stall_check; render_stats_read reports it */
    undefined4 ol_shrink_tries;/* +0x10: objlist_overflow_irq counts unresolved object list overflows; ta_area_split_shift(1) clears it or reports error 3 past 10 */
    undefined4 param_grow_tries;/* +0x14: param_overflow_irq counts them; ta_area_split_shift(0) clears it on success */
    undefined4 tex_render_ticks;/* +0x18: frame_slots_stall_scan counts V-blanks of a render to texture, restoring the frame write registers past 16 */
    undefined4 ol_overflows;  /* +0x1c: object list overflows counted by objlist_overflow_irq; render_stats_read reports it */
    undefined4 param_overflows;/* +0x20: parameter overflows counted by param_overflow_irq; render_stats_read reports it */
    undefined4 param_used[2];
    undefined4 list_high_water[5];
    undefined4 render_start_time;/* +0x40: TMU0 count render_start_ready_frame notes at render start */
    undefined4 render_time;   /* +0x44: render time render_done_irq records from TMU0; render_stats_read reports it */
    undefined4 ta_start_time; /* +0x48: TMU0 count frame_lists_send_and_rearm / ta_start_queued_frame note when vertices start */
    undefined4 ta_time;       /* +0x4c: ta_input_finish: TA start time minus TMU0 at list end; render_stats_read reports it */
} render_stats_rec;

/* ---- vf4lib/libcore/src/malloc.c ----------------------------------- */
/* A first-fit allocator over 32-byte units; each allocation is preceded by
 * a one-unit header and followed by a "++++" guard. */

struct heap;

/* heap_block.state values: the ASCII words are visible in a memory dump. */
typedef enum e_block_state {
    BLOCK_USED = 0x2D2D2D2D,  /* "----", set by malloc_internal()              */
    BLOCK_FREE = 0x20202020,  /* "    ", set by block_init() / free_internal() */
    HEAP_READY = 0x20020419   /* sentinel only, set by heap_init(); checked by
                                 check_heap_internal() ("probably uninitilized heap") */
} e_block_state;

/* Block header (one 32-byte unit before each allocation). */
typedef struct heap_block {
    struct heap_block *prev;  /* list links: list_remove(), list_insert_after() */
    struct heap_block *next;
    unsigned int bsize;       /* "bsize" (assert malloc.c:476): size in units, header included */
    int user_request_size;    /* "user_request_size" (fatal malloc.c:142): bytes asked for;
                                 -1 when free ("never used user_area") */
    void *tag;                /* malloc_internal()'s 2nd argument; the public wrappers pass
                                 their caller's return address (sts pr) */
    struct heap *heap;        /* owner; "mixed heap or broken %p" when it is not the heap */
    int serial;               /* heap->alloc_count when allocated (the "break point" number) */
    e_block_state state;      /* in use / free / heap sentinel (names inferred) */
} heap_block;

/* Heap control block. 0x6C bytes: two heaps sit 0x6C apart (0x0C41FDB8, 0x0C41FE24). */
typedef struct heap {
    void *base;               /* heap_init() start */
    unsigned int size;        /* heap_init() size */
    heap_block *first;        /* base rounded up to 32 */
    unsigned int usable;      /* bytes from first, whole units */
    heap_block head;          /* sentinel of the block list; head.state == 0x20020419 */
    heap_block *rover;        /* first-fit search starts here */
    int unknown_34;
    int alloc_count;          /* malloc_internal() increments it */
    int break_count;          /* "break point %d by malloc_internal" when alloc_count reaches it */
    struct mallinfo stats;    /* +0x40: the heap's statistics; the libbase wrappers (elan_heap.c) have heap_stats_get fill it */
    char check;               /* check_heap_internal() only walks the heap when set */
    char unknown_69[3];
} heap;

/* ---- vf4lib/libcore/src/tm_sys.c ----------------------------------- */

/* Standard C broken-down time; tm_sys.c fills one from the RTC and passes
 * it to mktime() (assert tm_sys.c:33 "now_time != -1"). */
typedef struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;     /* rtc month - 1 */
    int tm_year;    /* rtc year - 1900 */
    int tm_wday;    /* set to 0 */
    int tm_yday;    /* set to 0 */
    int tm_isdst;   /* set to -1 */
} tm;

/* The RTC record tm_sys.c copies from (byte offsets as it reads them). */
typedef struct rtc_time {
    unsigned short year;
    unsigned char  month;   /* 1-12 */
    unsigned char  day;
    unsigned char  hour;
    unsigned char  minute;
    unsigned char  second;
    unsigned char  unknown_7;
} rtc_time;

/* ---- file access to the DIMM image (unmapped file before malloc.c) -- */

/* An open file (0x4A4 bytes; 32 handles at 0x0C5A9158). */
typedef struct dimm_file {
    unsigned int start;      /* data offset: directory entry +0x7C, plus 0x100 */
    unsigned int end;        /* start + size */
    unsigned int pos;        /* read position, start..end */
    int size;                /* -1: handle free / closed */
    char name[128];          /* +0x10: path, strncpy(.., 0x7F); sffs_find compares 0x80 */
    char area[1024];         /* +0x90: nothing in sffs.c touches it; use not known */
    char *area_ptr;          /* +0x490: dimm_files_init points it at area */
    int field_0x494;         /* +0x494: dimm_files_init sets -1; use not known */
    void *dma;               /* +0x498: async read in flight (dimm_fread_dma) */
    unsigned int dma_items;  /* +0x49C: items that read covers */
    unsigned int dma_item_size;
} dimm_file;

/* The background sound reload (0x14 bytes, at sound_reload_job): the
 * queue entry being loaded and where its pack stands
 * (sound_reload_queue_step; sound_reload_abort_step winds it down). */
/* One entry of the background reload queue (sound_reload_queue): the
 * bank slot to load and its pack's file name; slot -1 is free, and
 * sound_reload_queue_step stops at the first. */
typedef struct sound_reload_entry {
    int slot;
    char *name;               /* +0x04 */
} sound_reload_entry;

typedef struct sound_reload_rec {
    char state;               /* sound_reload_queue_step: 0 open, 1 wait for the
                                 DTPK job, 2 copy, 3 feed done, 4 wait; the
                                 abort step reads it as how far to unwind    */
    char abort_step;          /* +0x01: sound_reload_abort_step's step       */
    int size;                 /* +0x04: bytes left to copy                   */
    char *addr;               /* +0x08: where the next bytes go              */
    dimm_file *fp;            /* +0x0c: the open pack                        */
    int *list;                /* +0x10: the sound_reload_queue entry {slot, file
                                 name} in hand; -1 ends the queue            */
} sound_reload_rec;

/* ---- vf4lib/libcore/src/load.c ------------------------------------- */

/* load_slot.state values (names inferred). */
typedef enum e_load_state {
    LOAD_FREE = 0,
    LOAD_RAW  = 1,   /* load_req_type() with type 3 */
    LOAD_GZIP = 2    /* type 4: read into gz_buffer, then decode_gzip_data() */
} e_load_state;

/* One queued load (0x58 bytes; 32 slots at 0x0C5B26C8, "<%s> IS OVER %d FILES"). */
typedef struct load_slot {
    e_load_state state;
    char name[64];
    void *read_to;           /* raw: the destination; gzip: gz_buffer */
    void *dst;               /* final destination */
    void *cursor;            /* starts equal to read_to */
    unsigned int size;       /* size argument of the request */
    unsigned int done;       /* cleared by the request; progress */
} load_slot;

/* ---- SimpleFlashFS: the DIMM image's filesystem (see tools/sffs.py) -- */

/* A file header, 0x100 bytes before each file's data in the image. The game
 * copies every live header into sffs_dir[] (sffs_load_dir(), 0x0C0F2E80). */
typedef struct sffs_entry {
    unsigned int remaining;    /* bytes from this header to the volume end;
                                  -1 in erased flash ends the directory walk */
    unsigned int taken;        /* header + data rounded up to the 4 KiB block;
                                  -1 marks free space (skip `remaining`) */
    int alive;                 /* -1 live, 0 deleted: sffs_load_dir() drops it */
    unsigned int size;         /* data size; dimm_fopen() copies it to dimm_file.size */
    unsigned int unknown_10;   /* not a 16-bit sum, CRC-32 or Adler-32 of the data */
    unsigned int mtime;        /* Unix seconds, 2003-10-17 .. 2004-10-26 */
    unsigned int mode;         /* st_mode: 0100644 / 0100664 */
    unsigned int unknown_1c;
    int unused_20[23];         /* -1 */
    unsigned int image_offset; /* +0x7C: -1 on disk; sffs_load_dir() stores the
                                  header's offset; dimm_fopen() reads data from +0x100 */
    char name[128];            /* sffs_find() compares up to 0x80 bytes */
} sffs_entry;

/* ---- libpoly: model sets, obj_<set>.bin + mdl_<set>.bin (tools/mdl.py) -- */

/* obj_ header: file offsets that obj_relocate() (0x0C0E7120) turns into
 * pointers. */
typedef struct obj_header {
    struct obj_node **nodes;   /* zero-ended list */
    char **names;              /* parallel to nodes; copied to node->name */
    short *tex_desc;           /* 16-byte texture descriptors, ended by w == 0 */
    void **list3;              /* optional zero-ended list (0 when absent) */
} obj_header;

/* A node of the model tree. Fields past +0x3C are used only by skinned
 * bodies (types 0x13 and 0x15). */
typedef struct obj_node {
    char *name;
    float pos[3];
    float rot[3];
    float scale[3];
    struct compiled_model_rec *model;
    struct compiled_model_rec *model2;
    int sibling;
    int parent;
    struct obj_header *file;
    int type;
    float unknown_40;
    int unknown_44[15];
    void **list_80;
    int tex_refs_offset;
    int bones;
    void **list_8c;
    int pairs_90;
} obj_node;

/* The 0x40-byte header of a model block in mdl_; an Elan command list
 * (GMP materials and ICH vertex lists) follows. A node without geometry
 * points at { -1, n } instead. */

/* One object bank: an object set loaded in memory (objbank_register fills
 * it from the set's obj_header). objbanks holds 21 at main.bin 0x0C3B64BC;
 * objbank_empty is the record a free bank gets. */
typedef struct objbank {
    int state;                 /* 0 empty, 1 ready, 2 loading, 3 freeing */
    obj_node **nodes;          /* +0x04: the header's nodes */
    char **names;              /* +0x08: the header's names */
    int count;                 /* +0x0C: nodes in the list */
    int set;                   /* +0x10: the object set (obj_files row) */
    int tex_base;              /* +0x14: obj_tex_base at load */
    int mem;                   /* +0x18: object memory (elan heap) */
    int tex_descs;             /* +0x1C: texture descriptors (texdesc_free_start) */
    void **tex_names;          /* +0x20: the header's list3 */
    char *name;                /* +0x24: names[0] */
    int size;                  /* +0x28 */
} objbank;

/* A fog setting (stage_fog.c's records fog_base, fog_offset, fog_target,
 * fog_fade, fog_cur at 0x0C374288-0x0C3742EF; a stage record's +0x38 points
 * at one). fog_frame_update writes fog_cur to the hardware. */
typedef struct fog_rec {
    unsigned short argb[4];    /* alpha, red, green, blue; clamped to 0-255
                                  when written (fog_col_ram_write_bytes) */
    float near_dist;           /* +0x08: fog_table_build's near */
    float far_dist;            /* +0x0C: its far; also the density register
                                  value (fog_density_pack_float) */
    float strength;            /* +0x10: fog_table_build's third argument */
} fog_rec;

/* A stage record (0x70 bytes; 24 of them from 0x0C306208, one per stage,
 * reached through stage_table). stage_load makes one current (stage_cur). */
typedef struct stage_rec {
    char *name;
    uint flags;
    int **draw_lists;
    int field_0xc;
    int obj_file;
    short stg_light_num;
    short rob_light_num;
    int *late_objs;
    char *coli_file;
    char *etc_file;
    short *sound_banks;
    uint *bg_colors;
    struct effect_slot *effect_list;
    int ef_obj_file;
    int start_pos;
    struct fog_rec *fog;
    int objs_on;
    int field_0x40;
    uint *objs;
    int vertex_hook;
    int *ring_out;
    struct vec3 center;
    struct lens_flare_rec *flare;
    char intro_cam;
    float *light_dir;
    float *fl_base;
    uint shadow_scale;
} stage_rec;

/* A reference a debug window or menu item holds (win_ref_address,
 * win_ref_resolve): an address, or with flags bit 0 a function called
 * with arg; unless flags bit 1 is set, the word at the result. */
typedef struct win_ref {
    void *word;
    int arg;
    unsigned int flags;
} win_ref;

/* A value as the debug window code moves it (win_value_load,
 * win_value_store): type 0 an int, 1 a float, 2 the word at the address,
 * 3 the address itself (a string). */
typedef struct win_value {
    int type;
    union { int i; float f; char *s; } u;
} win_value;

/* A value item's limits (win_value_step): the step, and with clamp set
 * the range; ints or floats by the item's format. */
typedef struct win_limits {
    char clamp;                      /* +0x00: 1 clamps to lo..hi */
    union { int i; float f; } step;  /* +0x04 */
    union { int i; float f; } lo;    /* +0x08 */
    union { int i; float f; } hi;    /* +0x0C */
} win_limits;

/* A debug window's menu item (a window's items point at these). */
typedef struct win_item {
    int kind;                  /* +0x00: 1 opens act's window, 2 resolves act, 3 shows the value, 4 also edits it */
    char *label;               /* +0x04 */
    win_ref act;               /* +0x08 */
    unsigned short flags;      /* +0x14: low nibble the size (0 byte, 1 short, 2 word, 3 float); 0x100 from value_base, 0x2000 through a pointer */
    unsigned char bit;         /* +0x16: the bit format 14 shows and toggles */
    unsigned char mode;        /* +0x17: low nibble the format, 0x30 two or three values, 0x80 a string by its word */
    unsigned char names_flags; /* +0x18: 4 a bitmask list, 2 the names through a pointer */
    char **names;              /* +0x1C */
    win_ref values[3];         /* +0x20 */
    win_limits *limits;        /* +0x44 */
} win_item;

/* A debug window (win_sys.c): 110 of them in win_windows, listed by
 * win_list. Positions and sizes are in text cells. */
typedef struct win_rec {
    char *title;               /* +0x00 */
    struct win_item **items;   /* +0x04 */
    unsigned short count;      /* +0x08: items */
    unsigned char name_cols;   /* +0x0A: name-column width */
    win_ref value_ref;         /* +0x0C: gives value_base each update */
    struct win_rec *tmpl;      /* +0x18: items and count come from it */
    void (*open_fn)(struct win_rec *, int);         /* +0x1C: (window, arg) on opening */
    int open_arg;
    void (*focus_fn)(struct win_rec *, int);        /* +0x24: each update while focused */
    int focus_arg;
    void (*update_fn)(struct win_rec *, int);       /* +0x2C: each update while open */
    int update_arg;
    void (*draw_fn)(struct win_rec *, int);         /* +0x34: after the window is drawn */
    int draw_arg;
    void (*close_fn)(struct win_rec *, int);        /* +0x3C: on closing */
    int close_arg;
    unsigned char x;           /* +0x44 */
    unsigned char y;
    unsigned char width;
    unsigned char height;
    unsigned short scroll;     /* +0x48: first item shown */
    unsigned short cursor;     /* +0x4A */
    unsigned int open : 1;     /* +0x4C: unsigned, win_open_count adds it */
    int focus : 1;             /* signed: its tests compile to and #2 only so */
    unsigned int : 6;
    unsigned char z;           /* +0x4D: draw order, 1 at the bottom */
    int value_base;            /* +0x50 */
    unsigned int cycled : 1;   /* +0x54: win_focus_cycle's mark */
} win_rec;

/* The record the four sound debug windows (SOUND_TEST, CV_TEST,
 * ROBSOUND_CHG, BGM_CHG) edit, sound_test_state. Each field but post_off
 * is one of their menu items; the names follow the item labels. */
typedef struct sound_test_rec {
    int code;                  /* +0x00: the request word SOUND_TEST posts */
    char *name;                /* +0x04 */
    unsigned short num;        /* +0x08: the sound_id_table entry */
    char cv_damage_off;        /* +0x0A: rob_voice_request drops voice group 4 */
    char cv_type;              /* +0x0B: CV_TEST's voice group */
    unsigned char cv_num;      /* +0x0C: CV_TEST's entry, from 1 */
    char title_se;             /* +0x0D: set, attract_adv_se_play stays silent */
    char post_off;             /* +0x0E: set, SOUND_TEST posts nothing; no item */
    short rob0_se_num;         /* +0x10: the sound_se_list entry */
    short rob0_vo_num;         /* +0x12: the sound_voice_list entry */
    short bgm_num;             /* +0x14: the sound_music_list entry */
    char *rob0_se_name;        /* +0x18 */
    char *rob0_vo_name;        /* +0x1C */
    char *bgm_name;            /* +0x20 */
} sound_test_rec;

/* ---- libmotion / robot rig: motion banks, skeletons (tools/motion.py, rig.py) -- */

/* One motion bank: main.bin 0x0C18BD18, 21 entries. Motion ids are global;
 * this bank holds ids first .. end - 1. */
typedef struct mot_bank {
    char *motdat;              /* motdat_mask_<chr>.bin: names + keyframe data */
    char *smooth;              /* smooth_<chr>.bin, or 0 */
    char *mothead;             /* mothead_<chr>.bin, or 0 */
    short first;
    short end;
} mot_bank;

/* A rig bone, 0x2C bytes (0x0C18BF54 onward; 13 body rigs of 47, then
 * stage-effect, work and 13 face rigs). */
typedef struct bone_def {
    char *name;                /* gblctr, k_hara, c_mune, j_mune, t_mune, ... */
    unsigned int flags;        /* 0x100 rest translate, 0x200 rest rotate,
                                  0x400 / 0x800 motion translate / rotate,
                                  0x80 IK (0x1000000 aim, 0x800000 chain root,
                                  0x200000 chain middle), 0x2000000 global frame */
    int bend;                  /* IK bend sign: 0 negative */
    float pos[3];              /* rest position; for chain bones the length on x */
    float rot[3];              /* rest rotation, radians, applied Z, Y, X */
    int nchildren;
    unsigned char *children;   /* indices into the same rig */
} bone_def;

/* main.bin 0x0C1664A0: the 13 body rigs. */
typedef struct rig_entry {
    bone_def *bones;
    int count;                 /* 47 */
} rig_entry;

/* A bound motion channel, 0x20 bytes (mot_channel_init(), 0x0C091E60). */
typedef struct mot_channel {
    short nkeys;               /* stored count + 1; keys 0 .. nkeys */
    short cursor;
    short last_frame;          /* frame count - 1: the time of the last key */
    short unused_06;
    unsigned char *times8;     /* key times in frames, motions under 256 frames */
    short *times16;            /* or 16-bit times (mot_sample_times16 reads them signed) */
    unsigned char *codes;      /* 2 bits per key, high bits first */
    unsigned short *values;    /* half floats: value / tangents per key code */
    unsigned short *cursor_values;
    unsigned short *values_end;
} mot_channel;

/* A bone's work record, 0x6C bytes: one per rig bone in a fighter's bone
 * buffer (rob_rec.bones), the face rig's (rob_rec.face_bones) and the stage
 * animation objects' (stage_anim_rec.bones). rob_bones_init fills id, def
 * and children, mot_bind_channels the channels, and the pose passes
 * (rob_eval_ik, rob_eval_matrix, rob_eval_output) the rest each frame. */
typedef struct bone_work_rec {
    short id;                  /* +0x0: the bone's number in its rig */
    short rot[3];              /* +0x2: this frame's rotation, 16-bit angles x, y, z */
    float pos[3];              /* +0x8: this frame's position (hand bones keep their shape value in pos[0]) */
    float keep_pos[3];         /* +0x14: the position kept for the next blend (bone_pos_keep) */
    float blend_pos[3];        /* +0x20: the kept position captured when a blend starts (bone_pos_blend_capture) */
    float rsmooth_pos[3];      /* +0x2c: the position the rsmooth blend goes to (rsmooth_targets_set) */
    float *mtx;                /* +0x38: the bone's output matrix */
    float *rot_mtx;            /* +0x3c: its rotation as a matrix */
    float *blend_mtx;          /* +0x40: the kept matrix captured when a blend starts */
    float *rsmooth_mtx;        /* +0x44: the matrix the rsmooth blend goes to */
    float *keep_mtx;           /* +0x48: the matrix kept for the next blend */
    mot_channel *pos_ch[3];    /* +0x4c: the bound motion's position channels x, y, z (null: none) */
    mot_channel *rot_ch[3];    /* +0x58: its rotation channels x, y, z */
    bone_def *def;             /* +0x64: the rig's definition of the bone */
    struct bone_work_rec **children; /* +0x68: def->nchildren pointers to the children's records */
} bone_work_rec;

/* ---- argument blocks passed by value -------------------------------- */

/* 63 words (252 bytes) spg_timing_load takes by value: 0c1482e0, 0c148480 and
 * 0c148800 each build one on the stack and pass it whole (r4-r7 carry the
 * first four words, the rest is copied to the outgoing stack); what the
 * words mean is not known yet. */
typedef struct args_0c148080 {
    int w[63];
} args_0c148080;

/* 29 words (116 bytes) fb_reg_images_pack takes by value; 0c147b20 builds one.
 * Named as 0c1479c0 reads them: p1..p28 in order, one word (pad9) it
 * never reads, p19 compared signed. */
typedef struct args_0c1479c0 {
    unsigned int p1; unsigned int p2; unsigned int p3; unsigned int p4; unsigned int p5; unsigned int p6; unsigned int p7; unsigned int p8; unsigned int p9;
    unsigned int pad9;
    unsigned int p10; unsigned int p11; unsigned int p12; unsigned int p13; unsigned int p14; unsigned int p15; unsigned int p16; unsigned int p17; unsigned int p18;
    int p19;
    unsigned int p20; unsigned int p21; unsigned int p22; unsigned int p23; unsigned int p24; unsigned int p25; unsigned int p26; unsigned int p27; unsigned int p28;
} args_0c1479c0;

/* ---- gfxlib/list_capture.c --------------------------------------------- */

/* the first 24 bytes of a 44-byte list capture record: the enabled-list mask and the five list starts (the five sizes at +0x18 follow outside this layout) */
typedef struct capture_head {
    undefined4 list_mask;     /* +0x0: enabled lists (state +0x474) */
    undefined4 list0_start;   /* +0x4: list 0 start */
    undefined4 list1_start;   /* +0x8: list 1 start */
    undefined4 list2_start;   /* +0xc: list 2 start */
    undefined4 list3_start;   /* +0x10: list 3 start */
    undefined4 list4_start;   /* +0x14: list 4 start */
} capture_head;

/* five list start addresses, the block the list-start pointer 0c41dffc points at */
typedef struct list_start_block {
    undefined4 list0_start;   /* +0x0: list 0 start */
    undefined4 list1_start;   /* +0x4: list 1 start */
    undefined4 list2_start;   /* +0x8: list 2 start */
    undefined4 list3_start;   /* +0xc: list 3 start */
    undefined4 list4_start;   /* +0x10: list 4 start */
} list_start_block;

/* ---- gfxlib/tex_passes.c ----------------------------------------------- */

/* the head of the CPU-written object list block (0x988 bytes up to vram_pair_pool_used; the per-tile object list pointers from +0x1c and the tile counts at +0x97c..+0x984 lie past this layout) */
typedef struct cpu_ol_target_rec {
    uint obj_list_base;       /* +0x0: the bank's object list base (cpu_ol_target_select: bank[half].obj_list_base); list_area_starts_lay_out lays the lists out from it */
    uint param_start;         /* +0x4: the bank's parameter write start (bank[half].field_0xc); cpu_ol_tile_ptrs_build puts param_write_addr back to it */
    undefined1 field_0x8[8];
    undefined4 param_write_addr;/* +0x10: next free parameter address */
    undefined4 tile_clip;     /* +0x14: columns - 1 in bits 0-7, rows - 1 in bits 16-23 */
    undefined4 block_size_word;/* +0x18: list block sizes (list_block_sizes_from_alloc) */
} cpu_ol_target_rec;

/* ---- gfxlib/vram_pair_pool.c ------------------------------------------- */

/* the two addresses of the VRAM pair pool's block, one per texture memory half */
typedef struct vram_pair_halves {
    undefined4 first_half;    /* +0x0: compared with state +0x478 */
    undefined4 second_half;   /* +0x4: vram_pair_pool_take allocates from it */
} vram_pair_halves;

/* ---- gfxlib/water_surface.c -------------------------------------------- */

/* one of the 8 x 8 wave records of the water patch: amplitude, then two phase and speed pairs */
typedef struct water_wave {
    float amplitude;          /* +0x0: water_amplitudes_set */
    undefined field_0x4;
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
    float speed1;             /* +0x8: added to the phase at +4 each step */
    undefined field_0xc;
    undefined field_0xd;
    undefined field_0xe;
    undefined field_0xf;
    float speed2;             /* +0x10: added to the phase at +0xc each step */
} water_wave;

/* The stage water surface (0x24E0 bytes): flags, grid spacing, two colors and look values, the 17 x 17 spectrum grids, 64 wave records and 17 x 17 height and normal cells. */
typedef struct water_state_rec {
    undefined4 flags;         /* +0x0: water_colors_set; bit 0 keeps the second color */
    undefined grid_spacing;   /* +0x4: float (the agg splits +4..+7 into bytes); water_surface_init's argument, the normal's y step in water_grid_build and water_normals_build */
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
    float field_0x8;
    float color1_r;           /* +0xc: first color red (unit float), with +0x10 / +0x14 the vector water_grid_build reads as c1 */
    float color1_g;           /* +0x10: first color green */
    float color1_b;           /* +0x14: first color blue */
    float color2_r;           /* +0x18: second color (zeros unless flags bit 0) */
    float color2_g;
    float color2_b;
    float look[6];            /* +0x24: water_surface_init stores 16.0, 1.0 and zeros; water_colors_set +0x24 / +0x28 */
    undefined4 field_0x3c;
    undefined4 field_0x40;
    float grid_re[17][17];    /* +0x44: water_step's real grid (heights after the FFT) */
    float grid_im[17][17];    /* +0x4c8: its imaginary grid */
    water_wave waves[64];     /* +0x94c: the wave records water_surface_init randomizes */
    undefined4 cells[17][17][5];/* +0xe4c: the 20-byte cells water_normals_build fills */
} water_state_rec;

/* ---- sdk/alab/naominet_client.c ---------------------------------------- */

/* the network client's settings from the start call and the DIMM board record */
typedef struct naominet_config_rec {
    int player_count;         /* +0x0: players 1..n accepted */
    undefined4 field_0x4;
    undefined4 ip_mode;       /* +0x8: 0: ask tenporouter.loc for the ip, 4: ip given */
    undefined4 board_mem_size;/* +0xc: board record +4; staging area at -0x12000, counters at -0x10080 */
    undefined4 board_top_start;/* +0x10: board record +8 */
    undefined4 name_server1;  /* +0x14: board record +0x54 */
    undefined4 name_server2;  /* +0x18: board record +0x58 */
} naominet_config_rec;

/* head and tail of a doubly linked job list */
typedef struct job_queue_ends {
    undefined4 head;          /* +0x0: job naominet_step runs */
    undefined4 tail;          /* +0x4: last queued job */
} job_queue_ends;

/* the state, player and socket error of the job that last failed */
typedef struct naominet_error_rec {
    undefined4 job_state;     /* +0x0: job +0xc */
    undefined4 player;        /* +0x4: job +0x10 */
    undefined4 sock_error;    /* +0x8: job +0x20 */
} naominet_error_rec;

/* the machine's game id, version, serial, ip text and board firmware bytes sent in the PowerOn request */
typedef struct naominet_machine_rec {
    undefined game_id;        /* +0x0: text, up to 4 characters */
    undefined field_0x1;
    undefined field_0x2;
    undefined field_0x3;
    undefined field_0x4;
    undefined version;        /* +0x5: text, up to 4 characters */
    undefined field_0x6;
    undefined field_0x7;
    undefined field_0x8;
    undefined field_0x9;
    undefined serial;         /* +0xa: text */
    undefined field_0xb;
    undefined field_0xc;
    undefined field_0xd;
    undefined field_0xe;
    undefined field_0xf;
    undefined field_0x10;
    undefined field_0x11;
    undefined field_0x12;
    undefined field_0x13;
    undefined field_0x14;
    undefined field_0x15;
    undefined1 ip_text;       /* +0x16: dotted ip text */
    undefined field_0x17;
    undefined field_0x18;
    undefined field_0x19;
    undefined field_0x1a;
    undefined field_0x1b;
    undefined field_0x1c;
    undefined field_0x1d;
    undefined field_0x1e;
    undefined field_0x1f;
    undefined field_0x20;
    undefined field_0x21;
    undefined field_0x22;
    undefined field_0x23;
    undefined field_0x24;
    undefined field_0x25;
    undefined field_0x26;
    undefined field_0x27;
    undefined field_0x28;
    undefined field_0x29;
    undefined field_0x2a;
    undefined field_0x2b;
    undefined field_0x2c;
    undefined field_0x2d;
    undefined field_0x2e;
    undefined field_0x2f;
    undefined field_0x30;
    undefined field_0x31;
    undefined field_0x32;
    undefined field_0x33;
    undefined field_0x34;
    undefined field_0x35;
    undefined field_0x36;
    undefined field_0x37;
    undefined field_0x38;
    undefined field_0x39;
    undefined field_0x3a;
    undefined field_0x3b;
    undefined field_0x3c;
    undefined field_0x3d;
    undefined field_0x3e;
    undefined field_0x3f;
    undefined field_0x40;
    undefined field_0x41;
    undefined field_0x42;
    undefined field_0x43;
    undefined field_0x44;
    undefined field_0x45;
    undefined field_0x46;
    undefined field_0x47;
    undefined field_0x48;
    undefined field_0x49;
    undefined field_0x4a;
    undefined field_0x4b;
    undefined field_0x4c;
    undefined field_0x4d;
    undefined1 firm_ver_major;/* +0x4e: bit 7 masked off by naominet_board_check */
    undefined1 firm_ver_minor;/* +0x4f: firm_ver second byte */
    undefined boot_ver_major; /* +0x50: boot_ver first byte */
    undefined boot_ver_minor; /* +0x51: boot_ver second byte */
    undefined field_0x52;
    undefined field_0x53;
    undefined4 shift_jis;     /* +0x54: 1: add &encode=Shift_JIS */
} naominet_machine_rec;

/* the head of the score report record naominet_step builds for a job: machine info, player, kind and the two flags (per-player counters follow) */
typedef struct naominet_score_rec {
    undefined4 machine;       /* +0x0: naominet_machine_info */
    undefined4 player;        /* +0x4: player index */
    undefined4 kind;          /* +0x8: 1..5, sent as zk= */
    undefined4 er_flag;       /* +0xc: 1: &er=1 */
    undefined4 di_flag;       /* +0x10: 1: &di=1 */
} naominet_score_rec;

/* the score extras: the hi and si numbers from the id chain, then optional texts */
typedef struct naominet_score_extra_rec {
    undefined4 hi_number;     /* +0x0: sent as &hi= */
    undefined4 si_number;     /* +0x4: sent as &si= */
} naominet_score_extra_rec;

/* pointers to the uri and host texts of the PowerOn reply */
typedef struct naominet_uri_host_rec {
    undefined4 uri;           /* +0x0: text after uri= */
    undefined4 host;          /* +0x4: text after host= */
} naominet_uri_host_rec;

/* pointers to the name, nickname and nickname tag texts of the PowerOn reply */
typedef struct naominet_name_rec {
    undefined4 reply_name;    /* +0x0: text after name= */
    undefined4 nickname;      /* +0x4: text after nickname= */
    undefined4 nickname_tag;  /* +0x8: text after the '#', or 0 */
} naominet_name_rec;

/* the region number and texts of the PowerOn reply */
typedef struct naominet_region_rec {
    undefined4 region;        /* +0x0: number after region0= */
    undefined4 region_text;   /* +0x4: text after region0= */
    undefined4 region_name0;  /* +0x8: text */
    undefined4 region_name1;  /* +0xc: text */
    undefined4 region_name2;  /* +0x10: text */
    undefined4 region_name3;  /* +0x14: text */
} naominet_region_rec;

/* the setting number of the PowerOn reply and its text */
typedef struct naominet_setting_rec {
    undefined4 value;         /* +0x0: number after setting= */
    undefined4 text;          /* +0x4: text after setting= */
} naominet_setting_rec;

/* the place id of the PowerOn reply as a 16-bit value and its text */
typedef struct naominet_place_rec {
    undefined2 value;         /* +0x0: four hex digits */
    undefined field_0x2;
    undefined field_0x3;
    undefined4 text;          /* +0x4: text after place_id= */
} naominet_place_rec;

/* the network client's eight time limits in milliseconds */
typedef struct naominet_timeout_set {
    undefined4 field_0x0;
    undefined4 resolve_ms;    /* +0x4: name lookup retry limit */
    undefined4 poweron_limit1;/* +0x8: PowerOn connection, first limit */
    undefined4 poweron_limit2;/* +0xc: PowerOn connection, second limit */
    undefined4 poweron_limit3;/* +0x10: PowerOn connection, third limit */
    undefined4 score_limit1;  /* +0x14: score connection, first limit */
    undefined4 score_limit2;  /* +0x18: score connection, second limit */
    undefined4 score_limit3;  /* +0x1c: score connection, third limit */
} naominet_timeout_set;

/* the server time of the PowerOn reply and its offset from the RTC */
typedef struct naominet_server_time_rec {
    undefined1 ahead;         /* +0x0: 1: server ahead of the RTC */
    undefined field_0x1;
    undefined2 year;          /* +0x2: year= */
    undefined1 month;         /* +0x4: month= */
    undefined1 day;           /* +0x5: day= */
    undefined1 hour;          /* +0x6: hour= */
    undefined1 minute;        /* +0x7: minute= */
    undefined1 second;        /* +0x8: second= */
    undefined field_0x9;
    undefined field_0xa;
    undefined field_0xb;
    undefined4 offset_seconds;/* +0xc: difference to the RTC */
} naominet_server_time_rec;

/* the time of day of the daily re-authentication */
typedef struct naominet_reauth_rec {
    undefined1 hour;          /* +0x0: 0..23 */
    undefined1 minute;        /* +0x1: 0..59 */
    undefined1 second;        /* +0x2: 0..59 */
} naominet_reauth_rec;

/* ---- sdk/kamui/elan_commands.c ----------------------------------------- */

/* the per-tile ownership bitmap pointers of the two buffers (the frame slot's low bit) */
typedef struct tile_bitmap_ptrs {
    undefined4 buffer0;       /* +0x0: bitmap pointer for buffer 0 (the frame slot's low bit) */
    undefined4 buffer1;       /* +0x4: bitmap pointer for buffer 1 */
} tile_bitmap_ptrs;

/* the ELAN revision's behavior flags and command queue limit */
typedef struct elan_rev_rec {
    undefined4 flags;         /* +0x0: 0x1f revision 1, 0x20 others */
    undefined4 queue_limit;   /* +0x4: command queue level to wait at */
} elan_rev_rec;

/* ---- sdk/kamui/list_buffers.c ------------------------------------------ */

/* the arguments of the list and frame buffer set-up calls and the frame buffer and list sizes worked out from them */
typedef struct render_setup_rec {
    undefined4 list_desc;     /* +0x0: list buffer descriptor */
    undefined4 list0_size;    /* +0x4: list 0 bytes */
    undefined4 list1_size;    /* +0x8: list 1 bytes */
    undefined4 list2_size;    /* +0xc: list 2 bytes */
    undefined4 list3_size;    /* +0x10: list 3 bytes */
    undefined4 list4_size;    /* +0x14: list 4 bytes */
    undefined4 fb_mode;       /* +0x18: 1: interlaced (fb_size_from_display_mode) */
    undefined4 fb0_surface;   /* +0x1c: fb_vram_alloc slot 0 surface */
    undefined4 field_0x20;
    undefined4 fb0_height;    /* +0x24: slot 0 height */
    undefined4 fb0_clear;     /* +0x28: slot 0: 1 zeroes it */
    undefined4 fb1_surface;   /* +0x2c: fb_vram_alloc slot 1 surface */
    undefined4 field_0x30;
    undefined4 fb1_height;    /* +0x34: slot 1 height */
    undefined4 fb1_clear;     /* +0x38: slot 1: 1 zeroes it */
    undefined4 front_surface; /* +0x3c: fb_pair_install first surface */
    undefined4 back_surface;  /* +0x40: fb_pair_install second surface */
    undefined4 field_0x44;
    undefined4 fb_width;      /* +0x48: 640 or 320 */
    undefined4 fb_height;     /* +0x4c: 480, 240 or given */
    undefined4 fb_pixel_bytes;/* +0x50: 2, 3 or 4 */
    undefined4 step_kind;     /* +0x54: 1 list buffers, 2 direct TA */
    undefined4 param_area_size;/* +0x58: TA parameter area bytes */
    undefined4 field_0x5c;
    undefined4 fb_bytes;      /* +0x60: width * height * bytes a pixel */
    undefined4 objlist_bytes; /* +0x64: objlist_bytes_compute */
} render_setup_rec;

/* the last strip header's list type and length, the last vertex block kind and the list bank parity */
typedef struct list_write_rec {
    undefined4 list_type;     /* +0x0: strip_list_type */
    undefined4 header_bytes;  /* +0x4: strip header length */
    undefined4 block_kind;    /* +0x8: list_block_sq_write kind */
    undefined4 bank_parity;   /* +0xc: bank & 1 */
} list_write_rec;

/* the five lists' start addresses in one list buffer bank */
typedef struct list_start_set {
    undefined4 list0;         /* +0x0: list 0 start */
    undefined4 list1;         /* +0x4: list 1 start */
    undefined4 list2;         /* +0x8: list 2 start */
    undefined4 list3;         /* +0xc: list 3 start */
    undefined4 list4;         /* +0x10: list 4 start */
} list_start_set;

/* the five lists' write pointers */
typedef struct list_cursor_set {
    undefined4 list0;         /* +0x0: list 0 write pointer */
    undefined4 list1;         /* +0x4: list 1 write pointer */
    undefined4 list2;         /* +0x8: list 2 write pointer */
    undefined4 list3;         /* +0xc: list 3 write pointer */
    undefined4 list4;         /* +0x10: list 4 write pointer */
} list_cursor_set;

/* the two texture memory addresses of the TA parameter area */
typedef struct ta_param_area_pair {
    undefined field_0x0;
    undefined field_0x1;
    undefined field_0x2;
    undefined field_0x3;
    undefined4 second_half;   /* +0x4: second half's address */
} ta_param_area_pair;

/* the number of header words at the start of each of the five lists */
typedef struct list_header_count_set {
    undefined4 list0;         /* +0x0: list 0 header words */
    undefined4 list1;         /* +0x4: list 1 header words */
    undefined4 list2;         /* +0x8: list 2 header words */
    undefined4 list3;         /* +0xc: list 3 header words */
    undefined4 list4;         /* +0x10: list 4 header words */
} list_header_count_set;

/* ---- sdk/kamui/scene_render.c ------------------------------------------ */

/* the frame write register values for a render into a texture surface */
typedef struct fb_write_rec {
    undefined4 write_ctrl;    /* +0x0: register 0x48 */
    undefined4 line_stride;   /* +0x4: register 0x4c */
    undefined4 write_start;   /* +0x8: register 0x60 */
    undefined4 clip_x;        /* +0xc: register 0x68 */
    undefined4 clip_y;        /* +0x10: register 0x6c */
} fb_write_rec;

/* ---- sdk/kamui/strip_context.c ----------------------------------------- */

/* the words and colors strip_header_build makes the strip header from */
typedef struct strip_header_src {
    undefined4 ctrl_word;     /* +0x0: parameter control word */
    undefined field_0x4;
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
    undefined field_0x8;
    undefined field_0x9;
    undefined field_0xa;
    undefined field_0xb;
    undefined field_0xc;
    undefined field_0xd;
    undefined field_0xe;
    undefined field_0xf;
    undefined4 isp_tsp_word;  /* +0x10: context +0x94 */
    undefined field_0x14;
    undefined field_0x15;
    undefined field_0x16;
    undefined field_0x17;
    undefined4 tex_word;      /* +0x18: context +0x9c */
    undefined field_0x1c;
    undefined field_0x1d;
    undefined field_0x1e;
    undefined field_0x1f;
    float color1_0;           /* +0x20: first color, float 0 */
    float color1_1;           /* +0x24: first color, float 1 */
    float color1_2;           /* +0x28: first color, float 2 */
    float color1_3;           /* +0x2c: first color, float 3 */
    float color2_0;           /* +0x30: second color, float 0 */
    float color2_1;           /* +0x34: second color, float 1 */
    undefined4 color2_2;      /* +0x38: second color, float 2 (used as float) */
    undefined field_0x3c;
    undefined field_0x3d;
    undefined field_0x3e;
    undefined field_0x3f;
    undefined4 odd_list_word; /* +0x40: the one word sent for odd list types */
    undefined4 vol2_tsp_word; /* +0x44: second volume TSP word */
    undefined4 vol2_tex_word; /* +0x48: second volume texture word */
    undefined field_0x4c;
    undefined field_0x4d;
    undefined field_0x4e;
    undefined field_0x4f;
    float vol2_color_0;       /* +0x50: second volume color, float 0 */
    float vol2_color_1;       /* +0x54: second volume color, float 1 */
    float vol2_color_2;       /* +0x58: second volume color, float 2 */
} strip_header_src;

/* ---- sdk/kamui/transfer_queue.c ---------------------------------------- */

/* address and byte length of a block in the ELAN list memory */
typedef struct list_block_ref {
    uint addr;
    uint length;
} list_block_ref;

/* a {head, tail, count} transfer node queue */
typedef struct xfer_node_queue {
    undefined field_0x0;
    undefined field_0x1;
    undefined field_0x2;
    undefined field_0x3;
    undefined field_0x4;
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
    undefined4 count;         /* +0x8: free nodes */
} xfer_node_queue;

/* ---- sdk/kamui/vram_alloc.c -------------------------------------------- */

/* the block lists of the two halves of texture memory: per half a used list, a free list (head and tail each) and the free byte count */
typedef struct vram_half_lists {
    undefined4 half0_used_head;/* +0x0: first used node of half 0 */
    undefined4 half0_used_tail;/* +0x4: last used node of half 0 */
    undefined4 half0_free_head;/* +0x8: first free node of half 0 */
    undefined4 half0_free_tail;/* +0xc: last free node of half 0 */
    undefined4 half0_free_bytes;/* +0x10: free bytes left in half 0 */
    undefined4 half1_used_head;/* +0x14: first used node of half 1 */
    undefined4 half1_used_tail;/* +0x18: last used node of half 1 */
    undefined4 half1_free_head;/* +0x1c: first free node of half 1 */
    undefined4 half1_free_tail;/* +0x20: last free node of half 1 */
    undefined4 half1_free_bytes;/* +0x24: free bytes left in half 1 */
} vram_half_lists;

/* ---- sdk/libam/jamma.c ------------------------------------------------- */

/* the 16 old-I/O input bytes the MIE returns after each request */
typedef struct legacy_io_input_bytes {
    char bytes[16];
} legacy_io_input_bytes;

/* the 32-byte request every JVS routine builds and hands to maple_send (Maple command 0x86): subcommand byte, then its arguments */
typedef struct mie_request_bytes {
    undefined1 subcommand;    /* +0x0: MIE subcommand (0x13, 0x15, 0x17, 0x31, ...) */
    undefined1 args[31];      /* +0x1: its arguments; the routines fill them through a byte pointer */
} mie_request_bytes;

/* ---- sdk/libam/nlam.c -------------------------------------------------- */

/* the board configuration: country, two 0/1 settings packed into one EEPROM byte, and DIP switch 1 */
typedef struct board_settings_rec {
    undefined4 country;       /* +0x0: country number; board_start_core's argument and the coin table index */
    undefined4 field_0x4;
    undefined4 field_0x8;
    undefined4 dip_switch_1;  /* +0xc: 1 when bit 0 of dip_switches is set */
} board_settings_rec;

/* mie_start's result words, followed by the area maple_hw_init places the Maple frame buffer in */
typedef struct mie_start_rec {
    undefined4 result;        /* +0x0: mie_start's result: 0, 2, 4 or 8 */
    undefined4 field_0x4;
    undefined4 field_0x8;
} mie_start_rec;

/* two 0x4c-byte card unit entries (flags, step code, a cleared word, 0x40 more bytes), one per Maple port 1 and 2 */
typedef struct card_unit_pair {
    undefined4 unit0_flags;   /* +0x0: port 1 unit: 0 present, else 2 no answer, 4 bad ident, 8 upload failed */
    undefined field_0x4;
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
    undefined field_0x8;
    undefined field_0x9;
    undefined field_0xa;
    undefined field_0xb;
    undefined field_0xc;
    undefined field_0xd;
    undefined field_0xe;
    undefined field_0xf;
    undefined field_0x10;
    undefined field_0x11;
    undefined field_0x12;
    undefined field_0x13;
    undefined field_0x14;
    undefined field_0x15;
    undefined field_0x16;
    undefined field_0x17;
    undefined field_0x18;
    undefined field_0x19;
    undefined field_0x1a;
    undefined field_0x1b;
    undefined field_0x1c;
    undefined field_0x1d;
    undefined field_0x1e;
    undefined field_0x1f;
    undefined field_0x20;
    undefined field_0x21;
    undefined field_0x22;
    undefined field_0x23;
    undefined field_0x24;
    undefined field_0x25;
    undefined field_0x26;
    undefined field_0x27;
    undefined field_0x28;
    undefined field_0x29;
    undefined field_0x2a;
    undefined field_0x2b;
    undefined field_0x2c;
    undefined field_0x2d;
    undefined field_0x2e;
    undefined field_0x2f;
    undefined field_0x30;
    undefined field_0x31;
    undefined field_0x32;
    undefined field_0x33;
    undefined field_0x34;
    undefined field_0x35;
    undefined field_0x36;
    undefined field_0x37;
    undefined field_0x38;
    undefined field_0x39;
    undefined field_0x3a;
    undefined field_0x3b;
    undefined field_0x3c;
    undefined field_0x3d;
    undefined field_0x3e;
    undefined field_0x3f;
    undefined field_0x40;
    undefined field_0x41;
    undefined field_0x42;
    undefined field_0x43;
    undefined field_0x44;
    undefined field_0x45;
    undefined field_0x46;
    undefined field_0x47;
    undefined field_0x48;
    undefined field_0x49;
    undefined field_0x4a;
    undefined field_0x4b;
    undefined4 unit1_flags;   /* +0x4c: port 2 unit's flags, as unit0_flags */
} card_unit_pair;

/* the coin configuration handed to the BIOS credit helpers: settings, coin setting number, eight prices and the four rate values of the coin setting */
typedef struct coin_settings_rec {
    undefined4 max_credit;    /* +0x0: the per-country maximum credit (9 or 24); first EEPROM byte of the coin configuration */
    undefined4 field_0x4;
    undefined4 separate_credits;/* +0x8: nonzero: each player has own credits (credit_try_spend) */
    undefined4 free_play;     /* +0xc: 1: credit_try_spend takes nothing off */
    undefined4 coin_setting;  /* +0x10: coin setting number (27 custom rates) */
    undefined4 price_0;       /* +0x14: credits a play costs, selection 0 */
    undefined4 price_1;       /* +0x18: selection 1 */
    undefined price_2;        /* +0x1c: selection 2 */
    undefined field_0x1d;
    undefined field_0x1e;
    undefined field_0x1f;
    undefined price_3;        /* +0x20: selection 3 */
    undefined field_0x21;
    undefined field_0x22;
    undefined field_0x23;
    undefined price_4;        /* +0x24: selection 4 */
    undefined field_0x25;
    undefined field_0x26;
    undefined field_0x27;
    undefined price_5;        /* +0x28: selection 5 */
    undefined field_0x29;
    undefined field_0x2a;
    undefined field_0x2b;
    undefined price_6;        /* +0x2c: selection 6 */
    undefined field_0x2d;
    undefined field_0x2e;
    undefined field_0x2f;
    undefined price_7;        /* +0x30: selection 7 */
    undefined field_0x31;
    undefined field_0x32;
    undefined field_0x33;
    undefined4 coin_rate_1;   /* +0x34: second byte of the coin setting */
    undefined4 coin_rate_2;   /* +0x38: third byte of the coin setting */
    undefined4 coin_rate_3;   /* +0x3c: fourth byte of the coin setting */
    undefined4 coin_rate_0;   /* +0x40: first byte of the coin setting */
} coin_settings_rec;

/* ---- sdk/libam/userbkup.c ---------------------------------------------- */

/* RAM copy of the first 16-byte header of the game's SRAM save area (0xa02001f8) */
typedef struct sram_save_header_a_rec {
    undefined4 crc;           /* +0x0: crc16_ccitt of the first copy */
    undefined4 size;          /* +0x4: sram_save_size */
    undefined4 padded_size;   /* +0x8: sram_save_padded_size */
    undefined4 field_0xc;
} sram_save_header_a_rec;

/* RAM copy of the second 16-byte header of the game's SRAM save area (0xa0200208) */
typedef struct sram_save_header_b_rec {
    undefined4 crc;           /* +0x0: crc16_ccitt of the second copy */
    undefined4 size;          /* +0x4: sram_save_size */
    undefined4 padded_size;   /* +0x8: sram_save_padded_size */
    undefined4 field_0xc;
} sram_save_header_b_rec;

/* ---- sdk/libam/usereep.c ----------------------------------------------- */

/* RAM copy of the first 4-byte header of the game's EEPROM area (eeprom_image +0x24) */
typedef struct eeprom_save_header_a_rec {
    undefined1 crc;           /* +0x0: low byte of the 16-bit CRC (stored as a half-word) */
    undefined field_0x1;
    undefined1 size;          /* +0x2: the attached buffer's size */
    undefined1 padded_size;   /* +0x3: the size rounded up to 4 */
} eeprom_save_header_a_rec;

/* RAM copy of the second 4-byte header of the game's EEPROM area (eeprom_image +0x28) */
typedef struct eeprom_save_header_b_rec {
    undefined1 crc;           /* +0x0: low byte of the 16-bit CRC (stored as a half-word) */
    undefined field_0x1;
    undefined1 size;          /* +0x2: the attached buffer's size */
    undefined1 padded_size;   /* +0x3: the size rounded up to 4 */
} eeprom_save_header_b_rec;

/* ---- sdk/libintr/irq_slots.c ------------------------------------------- */

/* the 13 interrupt slot functions the relay routines call (slots 5-11 live in the VBR table and stay 0 here) */
typedef struct irq_slot_fn_table {
    undefined4 render_done_fn;/* +0x0: slot 0, irq_render_done_relay */
    undefined4 vblank_in_fn;  /* +0x4: slot 1, irq_vblank_in_relay */
    undefined4 hblank_fn;     /* +0x8: slot 2, irq_hblank_relay */
    undefined4 rom_dma_end_fn;/* +0xc: slot 3, irq_rom_dma_end_relay */
    undefined4 cart_board_fn; /* +0x10: slot 4, irq_cart_board_relay */
    undefined field_0x14;
    undefined field_0x15;
    undefined field_0x16;
    undefined field_0x17;
    undefined field_0x18;
    undefined field_0x19;
    undefined field_0x1a;
    undefined field_0x1b;
    undefined field_0x1c;
    undefined field_0x1d;
    undefined field_0x1e;
    undefined field_0x1f;
    undefined field_0x20;
    undefined field_0x21;
    undefined field_0x22;
    undefined field_0x23;
    undefined field_0x24;
    undefined field_0x25;
    undefined field_0x26;
    undefined field_0x27;
    undefined field_0x28;
    undefined field_0x29;
    undefined field_0x2a;
    undefined field_0x2b;
    undefined field_0x2c;
    undefined field_0x2d;
    undefined field_0x2e;
    undefined field_0x2f;
    undefined4 tex_overflow_fn;/* +0x30: slot 12, irq_tex_overflow_relay */
} irq_slot_fn_table;

/* ---- sdk/libspr/lines.c ------------------------------------------------ */

/* how far one piece of a split 3D line moves along the segment: x, y and 1/z */
typedef struct line3d_step_vec {
    float x;                  /* +0x0: x step per piece */
    float y;                  /* +0x4: y step per piece */
    float inv_z;              /* +0x8: 1/z step per piece */
} line3d_step_vec;

/* the start of the current piece of a split 3D line: screen x, y and 1/z */
typedef struct line3d_piece_start_pt {
    float x;                  /* +0x0: screen x */
    float y;                  /* +0x4: screen y */
    float inv_z;              /* +0x8: 1/z */
} line3d_piece_start_pt;

/* the end of the current piece of a split 3D line: screen x, y and 1/z */
typedef struct line3d_piece_end_pt {
    float x;                  /* +0x0: screen x */
    float y;                  /* +0x4: screen y */
    float inv_z;              /* +0x8: 1/z */
} line3d_piece_end_pt;

/* ---- sdk/libspr/sprite.c ----------------------------------------------- */

/* 1 / width and 1 / height of the space sprite positions are given in */
typedef struct sprite_coord_recip_pair {
    float x_recip;            /* +0x0: 1 / width */
    float y_recip;            /* +0x4: 1 / height */
} sprite_coord_recip_pair;

/* width and height of the space sprite positions are given in (640 x 480) */
typedef struct sprite_coord_extent_pair {
    float space_width;        /* +0x0: sprite space width */
    float space_height;       /* +0x4: sprite space height */
} sprite_coord_extent_pair;

/* ---- sdk/naomi2_library/elan_lights.c ---------------------------------- */

/* the color of the single directional light: four floats, red, green, blue after the first (all 1.0 in .data) */
typedef struct single_light_color_rec {
    float field_0x0;
    float red;                /* +0x4: red, x 127 into the light command */
    float green;              /* +0x8: green */
    float blue;               /* +0xc: blue */
} single_light_color_rec;

/* the bits ORed into the two color words of the ELAN light header (both 0xff000000 at start-up) */
typedef struct lights_elan_color_bits_rec {
    uint ambient_bits;
    uint alt_bits;
} lights_elan_color_bits_rec;

/* a per-light record (16 of them, 16 bytes each) of three mode values passed to the ELAN light packers ((0, 0, 1) at start-up) */
typedef struct light_elan_mode_rec {
    uint mode_a;
    uint mode_b;
    uint mode_c;
    uint field_0xc;
} light_elan_mode_rec;

/* the ambient color after light_table: four floats, red, green, blue after the first */
typedef struct light_ambient_color_rec {
    float field_0x0;
    float red;                /* +0x4: x 255 into bits 16-23 */
    float green;              /* +0x8: x 255 into bits 8-15 */
    float blue;               /* +0xc: x 255 into bits 0-7 */
} light_ambient_color_rec;

/* ---- sdk/naomi2_library/model_draw.c ----------------------------------- */

/* two 3-word entries (mode 0, mode 1) of TSP bits for model_draw_blend_passes' passes */
typedef struct blend_pass_bits_table {
    undefined field_0x0;
    undefined field_0x1;
    undefined field_0x2;
    undefined field_0x3;
    undefined field_0x4;
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
    undefined field_0x8;
    undefined field_0x9;
    undefined field_0xa;
    undefined field_0xb;
    undefined field_0xc;
    undefined field_0xd;
    undefined field_0xe;
    undefined field_0xf;
    undefined field_0x10;
    undefined field_0x11;
    undefined field_0x12;
    undefined field_0x13;
    undefined4 field_0x14;
} blend_pass_bits_table;

/* the parameter record elan_matrix_cmds_write and the C matrix command builders take: near, far, -1/near and the environment-map u and v offsets */
typedef struct elan_matrix_param_rec {
    undefined field_0x0;
    undefined field_0x1;
    undefined field_0x2;
    undefined field_0x3;
    undefined field_0x4;
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
    undefined field_0x8;
    undefined field_0x9;
    undefined field_0xa;
    undefined field_0xb;
    undefined field_0xc;
    undefined field_0xd;
    undefined field_0xe;
    undefined field_0xf;
    float near;               /* +0x10: near distance of the projection */
    float far;                /* +0x14: far distance of the projection */
    float neg_inv_near;       /* +0x18: -1 / near */
    float env_u_offset;       /* +0x1c: added to environment-mapped u */
    float env_v_offset;       /* +0x20: added to environment-mapped v */
} elan_matrix_param_rec;

/* the two words ORed into every polygon header: one into the parameter control word, one into the TSP word */
typedef struct poly_header_or_pair {
    int control_or;           /* +0x0: ORed into the parameter control word */
    undefined4 tsp_or;        /* +0x4: ORed into the TSP word */
} poly_header_or_pair;

/* {start, size} of the last matrix command block built in the ELAN command buffer */
typedef struct matrix_cmds_span {
    undefined4 block_start;   /* +0x0: address of the block */
    undefined4 block_size;    /* +0x4: bytes written */
} matrix_cmds_span;

/* the x, y, z scale written at the end of the ELAN normal matrix block */
typedef struct normal_block_scale_vec {
    float x;                  /* +0x0: written negated */
    float y;                  /* +0x4: y scale */
    float z;                  /* +0x8: written negated */
} normal_block_scale_vec;

/* ---- sdk/naomi2_library/model_tex.c ------------------------------------ */

/* error bits and seven context words the model routines fill before their error traps */
typedef struct model_error_rec {
    undefined4 error_bits;    /* +0x0: 0x1000000, 0x2000000 or 0x4000000 per trap */
    undefined4 field_0x4;
    undefined4 field_0x8;
    undefined4 field_0xc;
    undefined4 field_0x10;
    undefined4 field_0x14;
    undefined4 field_0x18;
    undefined4 field_0x1c;
} model_error_rec;

/* ---- sdk/naomi2_library/naomi2_library.c ------------------------------- */

/* the six kept tile_split_select arguments: kind, paired flag and the rectangle */
typedef struct tile_split_args_rec {
    undefined4 kind;          /* +0x0: 0 checkerboard, 1 rectangle, 2 rows, 3 columns, 4 off */
    undefined4 paired;        /* +0x4: paired flag */
    undefined4 x0;            /* +0x8: rectangle left */
    undefined4 y0;            /* +0xc: rectangle top */
    undefined4 x1;            /* +0x10: rectangle right */
    undefined4 y1;            /* +0x14: rectangle bottom */
} tile_split_args_rec;

/* ---- sdk/naomi2_library/object_put.c ----------------------------------- */

/* the light direction of the object renderers, with z + 1 in the fourth float */
typedef struct obj_light_dir_vec {
    float x;                  /* +0x0: direction x */
    float y;                  /* +0x4: direction y */
    float z;                  /* +0x8: direction z */
    float field_0xc;
} obj_light_dir_vec;

/* the light direction plus the view vector, for the object renderers' specular term */
typedef struct obj_half_vector_vec {
    float x;                  /* +0x0: x */
    float y;                  /* +0x4: y */
    float z;                  /* +0x8: z */
} obj_half_vector_vec;

/* the view vector of the object renderers ((0, 0, 1) by default) */
typedef struct obj_view_dir_vec {
    float x;                  /* +0x0: x */
    float y;                  /* +0x4: y */
    float z;                  /* +0x8: z */
} obj_view_dir_vec;

/* ---- sdk/naomi_library/draw.c ------------------------------------------ */

/* the NAOMI library's vertex buffer descriptor: the per-list write pointers, the prepared polygon header and {current list, header size} */
typedef struct vertex_buffer_desc_rec {
    pointer list_write_ptrs;  /* +0x0: points at the per-list write pointers */
    pointer header;           /* +0x4: points at the prepared polygon header */
    pointer list_and_size;    /* +0x8: points at {current list index, header size} */
} vertex_buffer_desc_rec;

/* ---- sdk/naomi_library/print.c ----------------------------------------- */

/* the sprite header text_draw_string sends before the glyph quads: four parameter words, base color and a list choice */
typedef struct text_sprite_header_rec {
    undefined4 control_word;  /* +0x0: parameter control word 0xa2000009 */
    undefined4 isp_word;      /* +0x4: 0x80000000 */
    undefined4 tsp_word;      /* +0x8: 0x949004c7 */
    undefined4 tex_word;      /* +0xc: from text_sprite_tex_word */
    undefined4 base_color;    /* +0x10: text_color_set; white at start */
    undefined4 list_choice;   /* +0x14: 0: translucent list, else opaque */
} text_sprite_header_rec;

/* the surface record of the 8 x 1024 font texture */

/* ---- sdk/naomi_library/projection.c ------------------------------------ */

/* the header block the ELAN renderers take in r5: light vectors or modifier volume words, the near limit and the projection's near and far values */
typedef struct obj_send_header_rec {
    union {
        float half_vec[3];    /* +0x0: the half vector x, y, z in model space, scaled to length 436.5 (obj_light_vecs_to_model); the lit, float-color and intensity renderers read it as three floats */
        undefined4 words[3];  /* +0x0: for a modifier volume: the volume parameter control word, 0, modvol_instruction (obj_modvol_send) */
    } head;
    float near_limit;         /* +0xc: -near, the renderers' near clip value (read as raw bits; bit 0 of the register copy is a flag) */
    float light_dir[3];       /* +0x10: light direction x, y, z in model space (obj_light_vecs_to_model; the renderers load +0x10, +0x14, +0x18) */
    float light_scalar;       /* +0x1c: copy of obj_light_scalar (object_put_init) */
    float near_inv;           /* +0x20: 1 / near */
    float near_z;             /* +0x24: near value (obj_sphere_visible) */
    float far_z;              /* +0x28: far value (obj_sphere_visible_near_far) */
} obj_send_header_rec;

/* the four side planes of the view volume, four floats each (normal x, y, z, 0) */
typedef struct view_side_planes_rec {
    undefined field_0x0;
    undefined field_0x1;
    undefined field_0x2;
    undefined field_0x3;
    float field_0x4;
    undefined field_0x8;
    undefined field_0x9;
    undefined field_0xa;
    undefined field_0xb;
    undefined field_0xc;
    undefined field_0xd;
    undefined field_0xe;
    undefined field_0xf;
    undefined field_0x10;
    undefined field_0x11;
    undefined field_0x12;
    undefined field_0x13;
    float field_0x14;
    undefined field_0x18;
    undefined field_0x19;
    undefined field_0x1a;
    undefined field_0x1b;
    undefined field_0x1c;
    undefined field_0x1d;
    undefined field_0x1e;
    undefined field_0x1f;
    float field_0x20;
    undefined field_0x24;
    undefined field_0x25;
    undefined field_0x26;
    undefined field_0x27;
    undefined field_0x28;
    undefined field_0x29;
    undefined field_0x2a;
    undefined field_0x2b;
    undefined field_0x2c;
    undefined field_0x2d;
    undefined field_0x2e;
    undefined field_0x2f;
    float field_0x30;
} view_side_planes_rec;

/* The NAOMI library's graphics work area: palette group count and table, the projection's near and far planes, 1 - near, and the coordinate scale. */
typedef struct graphics_work_rec {
    undefined1 pal_shadow_changed; /* +0x0: set when the palette shadow changes; pal_shadow_flush clears it and sends the shadow */
    undefined1 pal_entry_format;  /* +0x1: the format number pal_entry_format_select keeps */
    undefined field_0x2;
    undefined field_0x3;
    short pal_group_count;
    short mark_depth;         /* +0x6: how many marks work_areas_mark_push has saved */
    undefined field_0x8;
    undefined field_0x9;
    undefined field_0xa;
    undefined field_0xb;
    undefined4 light_path_word; /* +0xc: lights_xform_by_current sets it to 0 (one light) or 0x80 (all lights); no reader found */
    undefined4 light_vertex_fn; /* +0x10: the vertex lighting routine lights_xform_by_current picks (light_vertex_single or light_vertex_multi); no reader found */
    undefined4 lights_to_model_fn; /* +0x14: set beside it: light0_by_inverse_rotation or lights_by_inverse_matrix; no reader found */
    undefined4 pal_group_table;
    float near_z;
    float far_z;
    float near_inv;           /* +0x24: 1 - near */
    float coord_scale;
    undefined4 work_a_out;    /* +0x2c: where the vertex lighting routines write their 16-byte records (x / w, y / w, 1 / w, intensity) in work area a */
    undefined4 work_a_out_end; /* +0x30: the address after the last record they wrote (work_areas_rewind moves work_a_out up to it) */
    uint work_a_base;         /* +0x34: the base of work area a (work_areas_set) */
    undefined4 work_b_out;    /* +0x38: their second output, 8 bytes a vertex, in work area b (normal . half vector, or the packed color) */
    undefined4 work_b_out_end; /* +0x3c: the end of the second output as the routine left it */
    uint work_b_base;         /* +0x40: the base of work area b (work_areas_set) */
    undefined4 field_0x44;
} graphics_work_rec;

/* ---- sdk/naomi_library/system.c ---------------------------------------- */

/* the three corner colors of the background plane, kept so it can be repainted */
typedef struct bg_plane_colors_rec {
    uint top_left;            /* +0x0: color of the top-left corner */
    uint top_right;           /* +0x4: color of the top-right corner */
    uint bottom_left;         /* +0x8: color of the bottom-left corner */
} bg_plane_colors_rec;

/* the start of the frame statistics record render_stats_read fills (frame_stats_capture) */
typedef struct frame_stats_rec {
    undefined4 ta_param_start;/* +0x0: parameter base (3D library state +0x490) */
    undefined field_0x4;
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
    undefined field_0x8;
    undefined field_0x9;
    undefined field_0xa;
    undefined field_0xb;
    undefined4 ta_param_end;  /* +0xc: parameter limit (state +0x494) */
    undefined field_0x10;
    undefined field_0x11;
    undefined field_0x12;
    undefined field_0x13;
    undefined4 ta_param_write_pos;/* +0x14: parameter write position at list end (state +0x550) */
    undefined1 field_0x18[24];
    undefined4 shown_buffer_addr; /* +0x30: the frame buffer on display (render_stats_read) */
    undefined4 hidden_buffer_addr;/* +0x34: the frame buffer being drawn */
    undefined1 field_0x38[16];
    undefined4 render_stalls; /* +0x48: render stalls (render_stats.render_stalls) */
    undefined4 vertex_stalls; /* +0x4c: vertex stalls (render_stats.vertex_stalls) */
    undefined1 field_0x50[88];
    undefined4 vertex_input_ticks; /* +0xa8: vertex input span of the last frame */
    undefined4 drawing_ticks; /* +0xac: drawing span of the last frame */
} frame_stats_rec;

/* the two frame surface pointers the render configuration block points at */
typedef struct frame_surface_pair {
    struct surface_rec *surface0; /* +0x0: first frame surface */
    struct surface_rec *surface1; /* +0x4: second frame surface */
} frame_surface_pair;

/* the five words of the draw context preset applied last */
typedef struct draw_context_preset_words draw_context_preset_words, *Pdraw_context_preset_words;
struct draw_context_preset_words {
    uint list_type;
    uint color_format;
    uint tex_offset_bits;
    uint alpha_on;
    uint gouraud;
};

/* the 3D library configuration block render_buffers_lay_out takes: frame surfaces, texture memory, vertex area and the list shares */
typedef struct render_config_block {
    undefined4 block_size;    /* +0x0: 0x60 */
    undefined4 flags;         /* +0x4: bit 0 (naomi2_lib_start), bit 0x10 two frames in flight */
    frame_surface_pair *frame_surfaces;/* +0x8: render_frame_surfaces */
    undefined4 frame_buffer_count;/* +0xc: 2 */
    undefined4 field_0x10;
    undefined4 field_0x14;
    undefined4 field_0x18;
    undefined4 texmem_size;   /* +0x1c: texture memory size (render_config_texmem_size_put) */
    undefined4 vertex_buf_desc;/* +0x20: vertex buffer descriptor 0x0C418458 */
    undefined4 vertex_area_base;/* +0x24: ta_param_base_put */
    undefined4 vertex_area_size;/* +0x28: 0x100000, then the list size total (ta_list_sizes_apply) */
    float list_shares[5]; /* +0x2c: percentage of the list block for each list: opaque, opaque modifier, translucent, translucent modifier, punch-through (render_buffers_lay_out) */
    undefined field_0x40;
    undefined field_0x41;
    undefined field_0x42;
    undefined field_0x43;
    undefined field_0x44;
    undefined field_0x45;
    undefined field_0x46;
    undefined field_0x47;
    undefined field_0x48;
    undefined field_0x49;
    undefined field_0x4a;
    undefined field_0x4b;
    undefined field_0x4c;
    undefined field_0x4d;
    undefined field_0x4e;
    undefined field_0x4f;
    undefined field_0x50;
    undefined field_0x51;
    undefined field_0x52;
    undefined field_0x53;
    undefined field_0x54;
    undefined field_0x55;
    undefined field_0x56;
    undefined field_0x57;
    undefined field_0x58;
    undefined field_0x59;
    undefined field_0x5a;
    undefined field_0x5b;
    undefined4 bypass_list;   /* +0x5c: 0, or 0x20 / 0x22 / 0x24 for the list left out */
} render_config_block;

/* the display record: screen width and height from the video mode table and the dither and anti-alias flags */
typedef struct display_state_rec {
    undefined field_0x0;
    undefined field_0x1;
    undefined field_0x2;
    undefined field_0x3;
    short screen_width;       /* +0x4: pixels */
    short screen_height;      /* +0x6: lines */
    short dither;             /* +0x8: frame flag bit 0x80 */
    short anti_alias;         /* +0xa: frame flag bit 0x8000; doubles x */
} display_state_rec;

/* ---- sdk/naomi_library/texture.c --------------------------------------- */

/* .rodata: the description of the built-in 8x8 texture (width, height, format, data) */
typedef struct tex_builtin_desc {
    short tex_width;          /* +0x0: 8; the height (8) follows */
    undefined field_0x2;
    undefined field_0x3;
    undefined4 format;        /* +0x4: 0x101 */
} tex_builtin_desc;

/* the texture slot of the built-in 8x8 texture that stands in for a missing surface */

/* ---- sdk/nladimm/nladimm.c --------------------------------------------- */

/* the 0x80-byte DIMM board record dimm_board_info_fetch fills from the board's shared words */
typedef struct dimm_board_info_rec dimm_board_info_rec, *Pdimm_board_info_rec;

struct dimm_board_info_rec {
    uint id_word;
    int module_bytes;
    uint module_base;
    short id_low;
    uchar id_top;
    uchar module_code[2];
    uchar word_14_byte;
    uchar field_0x12;
    uchar word_18_bit16;
    uchar word_18_byte;
    char field_0x15[3];
    uint word_20;
    uint feature_bits;
    uint word_28;
    uint word_2c;
    char field_0x28[32];
    uint own_ip;
    uint word_4c;
    uint word_50;
    uint name_server1;
    uint name_server2;
    char field_0x5c[36];
};

/* the request's 32-byte argument copy, the tail of dimm_request (the global dimm_request_arg names it where the code addresses it on its own) */
typedef struct dimm_request_arg_buf {
    undefined4 field_0x0;
    undefined4 field_0x4;
    undefined4 field_0x8;
    undefined4 field_0xc;
    undefined4 field_0x10;
    undefined4 field_0x14;
    undefined4 field_0x18;
    undefined4 field_0x1c;
} dimm_request_arg_buf;

/* the request dimm_command_post made, as dimm_command_poll checks it (0x30 bytes) */
typedef struct dimm_request_rec {
    undefined4 command;       /* +0x0: command 0-31 (dimm_command_table index) */
    undefined4 reply;         /* +0x4: last reply word (bits 8-9 the state) */
    int stall_polls;          /* +0x8: polls with the reply unchanged */
    undefined4 fail_code;     /* +0xc: 3 timed out, 4 failed */
    dimm_request_arg_buf arg; /* +0x10: the command's argument copy (dimm_command_post, dimm_command_cancel) */
} dimm_request_rec;

/* ---- sdk/nlasock/control.c --------------------------------------------- */

/* the state of the one control call in flight: done flag, operation, result and the request address */
typedef struct ctl_call_block {
    int done;
    int op;
    int result;
    int req_addr;
} ctl_call_block;

/* ---- sdk/nlasock/dimm_link.c ------------------------------------------- */

/* the four arguments of the last board command no handler took */
typedef struct dimm_cmd_args {
    uint cmd_word;
    uint data_offset;
    uint param_lo;
    uint param_hi;
} dimm_cmd_args;

/* ---- sdk/nlasock/peer.c ------------------------------------------------ */

/* the peer request state: done flag, result and the peer area address */
typedef struct peer_request_rec {
    int done;
    int result;
    int area_addr;
} peer_request_rec;

/* the block peer_start_put_table writes to the board before a peer start */
typedef struct peer_start_table_rec {
    ushort start_value;
    char field_0x2[6];
    ushort port;
    char field_0xa[6];
    uint list[16];
} peer_start_table_rec;

/* ---- vf4lib/libaction/src/control.c ------------------------------------ */

/* one choice point of the move script interpreter: the saved input state and the script pointer to resume at */
typedef struct script_choice {
    undefined pc_only;        /* +0x0: 1 when pushed by script_op_11_choice_pc */
    undefined field_0x1;
    undefined2 scan_pos;      /* +0x2: saved history scan position (DAT_0c421b1e) */
    undefined2 input;         /* +0x4: saved frame input (DAT_0c421b34) */
    undefined2 new_press;     /* +0x6: saved new presses (DAT_0c421b32) */
    undefined2 field_0x8;
    undefined2 field_0xa;
    short saved_allow_frames; /* +0xc: script_allow_frames, saved by script_op_10_choice */
    short saved_window;       /* +0xe: script_window, saved by script_op_10_choice */
    undefined4 field_0x10;
    undefined4 field_0x14;
    undefined4 resume_pc;     /* +0x18: script pointer to resume at */
} script_choice;

/* ---- vf4lib/libaction/src/rob_coli.c ----------------------------------- */

/* results of the two nearest-hit calls coli_timing_test times */
typedef struct coli_test_nearest_rec {
    undefined4 segment;       /* +0x0: coli_segment_hit_nearest result */
    undefined4 sphere;        /* +0x4: coli_sphere_hit_nearest result */
} coli_test_nearest_rec;

/* results of the two hit calls coli_timing_test times */
typedef struct coli_test_hits_rec {
    undefined4 segment;       /* +0x0: coli_segment_hit result */
    undefined4 sweep;         /* +0x4: coli_sweep_hit result */
} coli_test_hits_rec;

/* ---- vf4lib/libbase/src/appose.c --------------------------------------- */

/* the pose (0-2, -1 none) chosen for each side in the pre-fight and winner scenes */
typedef struct appose_pose_pair {
    undefined1 side0;         /* +0x0: pose of side 0 */
    undefined1 side1;         /* +0x1: pose of side 1 */
} appose_pose_pair;

/* per side, the bits of the poses already tried (7 = all three) */
typedef struct appose_tried_pair {
    undefined1 side0;         /* +0x0: tried poses of side 0 */
    undefined1 side1;         /* +0x1: tried poses of side 1 */
} appose_tried_pair;

/* ---- vf4lib/libbase/src/bg_color.c ------------------------------------- */

/* The three opaque background corner colors bg_color_custom_apply builds from bg_color_custom_rgb (the third word, at +8, lies past the exported layout). */
typedef struct bg_color_custom_rec {
    pointer color0;           /* +0x0: first corner color */
    undefined * color1;       /* +0x4: second corner color */
} bg_color_custom_rec;

/* ---- vf4lib/libbase/src/card_log.c ------------------------------------- */

/* One of the 32 16-byte item log records in the backup (user_back_data.card_item_log). */
typedef struct card_item_log_rec {
    int card_id;
    uint stamp;
    char chara;
    char side;
    char item;
    char slot;
    char state;
    uchar pad_d[3];
} card_item_log_rec;

/* The nine card fields card_data_log_snapshot copies (card_snapshot_clear zeroes them).
 * card_data_log_restore compares exp/point and rank_pnt/tenpo_pnt member by member; GCC merges
 * each &&-chained pair into one wider compare, which is the word compare the game shows. */
typedef struct card_data_log_snap {
    ushort all_win;           /* +0x00 card header all_win */
    ushort all_lose;          /* +0x02 card header all_lose */
    uchar level;              /* +0x04 card header level */
    uchar exp;                /* +0x05 card body exp */
    char point;               /* +0x06 card body point */
    undefined1 field_0x7;
    uint color_1p;            /* +0x08 card body color_1p.w */
    uint color_2p;            /* +0x0c card body color_2p.w */
    ushort rank_pnt;          /* +0x10 card body rank_pnt */
    ushort tenpo_pnt;         /* +0x12 card body tenpo_pnt */
} card_data_log_snap;                                          /* 0x14 */

/* One of the 32 52-byte card data log records in the backup (user_back_data.card_data_log). */
typedef struct card_data_log_rec {
    int card_id;
    uint stamp;
    char chara;
    char state;
    uchar pad_a[2];
    struct card_data_log_snap snapshots[2];
} card_data_log_rec;

/* ---- vf4lib/libbase/src/cns_nickname.c --------------------------------- */

/* ---- vf4lib/libbase/src/game_stats.c ----------------------------------- */

/* The first two of the six 16-bit SRAM ledger counters kept as the baseline for the daily records (the other four follow as DAT_0c28c218-0c28c21e). */
typedef struct ledger_baseline_rec {
    undefined2 ledger_28;     /* +0x0: ledger_snapshot +0x28 */
    undefined2 ledger_2c;     /* +0x2: ledger_snapshot +0x2c */
} ledger_baseline_rec;

/* ---- vf4lib/libbase/src/light.c ---------------------------------------- */

/* one light set's ambient color, 16 bytes per set (six sets, indexed by the set number); the color is the three floats at +4. */
typedef struct ambient_entry {
    undefined1 field_0x0;
    undefined1 field_0x1;
    undefined1 field_0x2;
    undefined1 field_0x3;
    float color[3];
} ambient_entry;

/* the light record of the extra light (light 14) that extra_light_parallel_set / extra_light_spot_set fill and queue with elan_light_queue. */
typedef struct extra_light_rec {
    undefined1 light_no;      /* +0x0: 14: the light number elan_light_queue sends */
    undefined kind;           /* +0x1: 2 parallel, 4 spot */
    undefined field_0x2;
    undefined field_0x3;
    undefined field_0x4;
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
    float field_0x8;
    float field_0xc;
    float field_0x10;
} extra_light_rec;

/* the 0x44-byte light record of the hit flash (light 13 of rob set 1), copied to light 13 of set 4 each frame. */
typedef struct hit_light_rec {
    undefined2 light_type;    /* +0x0: 3 point light */
    undefined field_0x2;
    undefined field_0x3;
    undefined field_0x4;
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
    float pos_x;              /* +0x8: position */
    float pos_y;
    float pos_z;
    undefined field_0x14;
    undefined field_0x15;
    undefined field_0x16;
    undefined field_0x17;
    undefined field_0x18;
    undefined field_0x19;
    undefined field_0x1a;
    undefined field_0x1b;
    undefined field_0x1c;
    undefined field_0x1d;
    undefined field_0x1e;
    undefined field_0x1f;
    undefined field_0x20;
    undefined field_0x21;
    undefined field_0x22;
    undefined field_0x23;
    undefined field_0x24;
    undefined field_0x25;
    undefined field_0x26;
    undefined field_0x27;
    undefined field_0x28;
    undefined field_0x29;
    undefined field_0x2a;
    undefined field_0x2b;
    undefined field_0x2c;
    undefined field_0x2d;
    undefined field_0x2e;
    undefined field_0x2f;
    float red;                /* +0x30: color, faded by hit_light_decay */
    float green;
    float blue;
    float range_near;         /* +0x3c: 0.0001 */
    float range_far;          /* +0x40: 1.2 */
} hit_light_rec;

/* the position (x, y, z) of the hit that lights the hit flash. */
typedef struct hit_light_pos_rec {
    float x;
    float y;
    float z;
} hit_light_pos_rec;

/* the light-enable record elan_light_masks_queue sends: four 16-bit light masks at +8 (and the colors after the exported 16 bytes). */
typedef struct light_masks_rec {
    undefined field_0x0;
    undefined field_0x1;
    undefined field_0x2;
    undefined field_0x3;
    undefined field_0x4;
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
    short mask0;              /* +0x8: light-enable bits, one per light */
    short mask1;
    short mask2;
    short mask3;
} light_masks_rec;

/* the six light sets of 16 0x44-byte light records (0x440 bytes per set); the exported layout covers the start of light 0 of set 0 (type +0, position +8, direction +0x14). */

/* ---- vf4lib/libbase/src/load_obj.c ------------------------------------- */

/* per side, the texture memory the fighter's body model took (int per side). */
typedef struct rob_body_vram_sides {
    undefined4 side0;
    undefined4 side1;
} rob_body_vram_sides;

/* per side, the texture memory the fighter's item models took (int per side). */
typedef struct rob_items_vram_sides {
    undefined4 side0;
    undefined4 side1;
} rob_items_vram_sides;

/* per side, a request that the next model load take the alternative model. */
typedef struct rob_obj_alt_req_sides {
    undefined1 side0;
    undefined1 side1;
} rob_obj_alt_req_sides;

/* per side, set when the player's card entitles and asks for the alternative model. */
typedef struct rob_alt_model_card_sides {
    undefined1 side0;
    undefined1 side1;
} rob_alt_model_card_sides;

/* ---- vf4lib/libbase/src/select.c --------------------------------------- */

/* per player, set when the player decided a character this select. */
typedef struct sel_just_decided_sides {
    undefined1 side0;
    undefined1 side1;
} sel_just_decided_sides;

/* per player, the grid slot the character cursor was on before this frame. */
typedef struct sel_cursor_prev_sides {
    char side0;
    char side1;
} sel_cursor_prev_sides;

/* two bytes per select grid slot, set when a cursor first lands on the slot; read back nowhere by name. */
typedef struct sel_slot_marks_rec {
    undefined field_0x0;
    undefined field_0x1;
    undefined field_0x2;
    undefined field_0x3;
    undefined field_0x4;
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
    undefined field_0x8;
    undefined field_0x9;
    undefined field_0xa;
    undefined field_0xb;
    undefined field_0xc;
    undefined field_0xd;
    undefined1 field_0xe;
    undefined1 field_0xf;
} sel_slot_marks_rec;

/* The select screens' state: scenes-loaded byte, the AET object and still handles (-1 = none) of the character, stage and 1P mode screens, the per-player cursor bits and timers, the stage preview and the fade counter. */
typedef struct sel_screen_rec {
    undefined1 scenes_loaded; /* +0x0: 1 once sel_scenes_load has loaded the sprite sets; cleared by sel_scenes_release (reached through the scalar row name) */
    undefined field_0x1;
    undefined field_0x2;
    undefined field_0x3;
    undefined4 field_0x4;
    undefined field_0x8;
    undefined field_0x9;
    undefined field_0xa;
    undefined field_0xb;
    undefined field_0xc;
    undefined field_0xd;
    undefined field_0xe;
    undefined field_0xf;
    undefined field_0x10;
    undefined field_0x11;
    undefined field_0x12;
    undefined field_0x13;
    undefined field_0x14;
    undefined field_0x15;
    undefined field_0x16;
    undefined field_0x17;
    undefined field_0x18;
    undefined field_0x19;
    undefined field_0x1a;
    undefined field_0x1b;
    undefined field_0x1c;
    undefined field_0x1d;
    undefined field_0x1e;
    undefined field_0x1f;
    undefined field_0x20;
    undefined field_0x21;
    undefined field_0x22;
    undefined field_0x23;
    undefined field_0x24;
    undefined field_0x25;
    undefined field_0x26;
    undefined field_0x27;
    undefined field_0x28;
    undefined field_0x29;
    undefined field_0x2a;
    undefined field_0x2b;
    undefined field_0x2c;
    undefined field_0x2d;
    undefined field_0x2e;
    undefined field_0x2f;
    undefined4 field_0x30;
    undefined field_0x34;
    undefined field_0x35;
    undefined field_0x36;
    undefined field_0x37;
    undefined field_0x38;
    undefined field_0x39;
    undefined field_0x3a;
    undefined field_0x3b;
    undefined field_0x3c;
    undefined field_0x3d;
    undefined field_0x3e;
    undefined field_0x3f;
    undefined field_0x40;
    undefined field_0x41;
    undefined field_0x42;
    undefined field_0x43;
    undefined cursor_flash_obj_1p;/* +0x44: CHA_FLASH_*_IN / _KETTEI object, 2P at +0x48 (sel_chara_cursor_draw h1[p]) */
    undefined field_0x45;
    undefined field_0x46;
    undefined field_0x47;
    undefined field_0x48;
    undefined field_0x49;
    undefined field_0x4a;
    undefined field_0x4b;
    undefined cursor_base_obj_1p;/* +0x4c: CHA_BASE_1P object, 2P at +0x50 (sel_chara_cursor_draw h2[p]) */
    undefined field_0x4d;
    undefined field_0x4e;
    undefined field_0x4f;
    undefined field_0x50;
    undefined field_0x51;
    undefined field_0x52;
    undefined field_0x53;
    undefined decide_line_obj_1p;/* +0x54: LINE_1 object of sel_decide_lines_show, 2P at +0x58 */
    undefined field_0x55;
    undefined field_0x56;
    undefined field_0x57;
    undefined field_0x58;
    undefined field_0x59;
    undefined field_0x5a;
    undefined field_0x5b;
    undefined4 chara_to_mode_obj;/* +0x5c: CHARA2MODE transition; the 1P mode layers and cursor wait for it to end */
    undefined4 mode_normal_obj;/* +0x60: MOD_NORMAL_IN / _DEF layer */
    undefined4 mode_chall_obj;/* +0x64: MOD_CHALL_DEF / _IN layer */
    undefined4 mode_cursor_obj;/* +0x68: CSR_MOD_* cursor layer of sel_1pmode_cursor_draw */
    undefined4 mode_flash_obj;/* +0x6c: MOD_FLASH_*_IN / _KETTEI layer */
    undefined data_obj_1p;    /* +0x70: SEL_DATA_1P object, 2P at +0x74 (sel_data_layer_show) */
    undefined field_0x71;
    undefined field_0x72;
    undefined field_0x73;
    undefined field_0x74;
    undefined field_0x75;
    undefined field_0x76;
    undefined field_0x77;
    undefined4 vs_batu_obj;   /* +0x78: CHA_VS_BATU, started when both players are in */
    undefined4 opening_obj;   /* +0x7c: SEL_OPNING; the data titles wait until it stops playing */
    undefined4 data_time_obj; /* +0x80: SEL_BASE_DATATIME */
    undefined4 mode_or_obj;   /* +0x84: MOD_OR layer */
    undefined4 ksn_obj_1p;    /* +0x88: KSN_1P object shown during the card renew step, 2P at +0x8C (sel_ksn_show / sel_ksn_release) */
    undefined field_0x8c;
    undefined field_0x8d;
    undefined field_0x8e;
    undefined field_0x8f;
    undefined4 chara_to_stage_obj;/* +0x90: CHARA2STAGE transition; sel_stage_select waits for its end */
    undefined4 mode_line_obj; /* +0x94: LINE_1 started when the 1P mode choice is decided */
    undefined4 data_title_obj_1p;/* +0x98: SEL_DATA_TITLE_1P */
    undefined4 data_title_obj_2p;/* +0x9c: SEL_DATA_TITLE_2P */
    undefined4 stage_preview_obj;/* +0xa0: stage preview object in the preview scene (sel_stage_preview_draw) */
    undefined4 stage_icon_obj;/* +0xa4: stage icon object (sel_stage_icon_show) */
    undefined4 mode_base_obj; /* +0xa8: MOD_BASE_1P / _2P layer */
    undefined4 mode_message_obj;/* +0xac: MOD_MESSAGE_IN, then MOD_MESSAGE */
    undefined field_0xb0;
    undefined field_0xb1;
    undefined field_0xb2;
    undefined field_0xb3;
    undefined field_0xb4;
    undefined field_0xb5;
    undefined field_0xb6;
    undefined field_0xb7;
    undefined field_0xb8;
    undefined field_0xb9;
    undefined field_0xba;
    undefined field_0xbb;
    undefined field_0xbc;
    undefined field_0xbd;
    undefined field_0xbe;
    undefined field_0xbf;
    undefined field_0xc0;
    undefined field_0xc1;
    undefined field_0xc2;
    undefined field_0xc3;
    undefined field_0xc4;
    undefined field_0xc5;
    undefined field_0xc6;
    undefined field_0xc7;
    undefined field_0xc8;
    undefined field_0xc9;
    undefined field_0xca;
    undefined field_0xcb;
    undefined field_0xcc;
    undefined field_0xcd;
    undefined field_0xce;
    undefined field_0xcf;
    undefined field_0xd0;
    undefined field_0xd1;
    undefined field_0xd2;
    undefined field_0xd3;
    undefined field_0xd4;
    undefined field_0xd5;
    undefined field_0xd6;
    undefined field_0xd7;
    undefined field_0xd8;
    undefined field_0xd9;
    undefined field_0xda;
    undefined field_0xdb;
    undefined field_0xdc;
    undefined field_0xdd;
    undefined field_0xde;
    undefined field_0xdf;
    undefined field_0xe0;
    undefined field_0xe1;
    undefined field_0xe2;
    undefined field_0xe3;
    undefined field_0xe4;
    undefined field_0xe5;
    undefined field_0xe6;
    undefined field_0xe7;
    undefined field_0xe8;
    undefined field_0xe9;
    undefined field_0xea;
    undefined field_0xeb;
    undefined4 chara_icon_still_1p;/* +0xec: still started by sel_chara_small_draw when sel_chara_icon_needed */
    undefined4 chara_icon_still_2p;
    undefined4 chara_scene_1p;/* +0xf4: character scene id from sel_chara_info (textures loaded by sel_chara_scene_load); -1 blocks the decision */
    undefined4 chara_scene_2p;
    undefined field_0xfc;
    undefined field_0xfd;
    undefined field_0xfe;
    undefined field_0xff;
    undefined field_0x100;
    undefined field_0x101;
    undefined field_0x102;
    undefined field_0x103;
    undefined4 cursor_flags_1p;/* +0x104: bit 0 cursor settled (scene load), 1 waiting (TAIKI), 2 decided pose shown, 3 decided (KETTEI), 4 both cursors on one slot */
    undefined4 cursor_flags_2p;
    undefined field_0x10c;
    undefined field_0x10d;
    undefined field_0x10e;
    undefined field_0x10f;
    undefined field_0x110;
    undefined field_0x111;
    undefined field_0x112;
    undefined field_0x113;
    int cursor_idle_timer_1p; /* +0x114: set to 10 on a cursor move (17 in the 1P mode chooser), counts down; at 0 the cursor switches to the waiting layer; 2P at +0x118 */
    undefined field_0x118;
    undefined field_0x119;
    undefined field_0x11a;
    undefined field_0x11b;
    undefined field_0x11c;
    undefined field_0x11d;
    undefined1 chara_scenes_unloaded;/* +0x11e: set by sel_stage_init after sel_chara_scenes_unload, which does nothing while it is set */
    undefined1 mode_message_state;/* +0x11f: 0 none, 1 MOD_MESSAGE_IN started (then looped as MOD_MESSAGE) */
    undefined4 stage_preview_scene;/* +0x120: scene id of the previewed stage, -1 when none (textures loaded by sel_stage_preview_load) */
    undefined4 stage_preview_set;/* +0x124: sprite set index tested in sprite_set_loaded before the preview starts */
    int stage_preview_fade;   /* +0x128: set to 8 on a stage cursor move, counts down; preview loads below 7, starts below 4, dims the overlay while nonzero */
    int fade_frame;           /* +0x12c: frame counter of sel_fade_draw, cleared when a fade starts */
    float fade_alpha;         /* +0x130: alpha of sel_fade_draw's overlay, cleared by sel_stage_init and sel_stage_select */
    char fade_done;           /* +0x134: set by sel_fade_draw when the fade ends; sel_stage_screen_draw waits for it */
    undefined field_0x135;
    undefined field_0x136;
    undefined field_0x137;
} sel_screen_rec;

/* ---- vf4lib/libbase/src/sound_table.c ---------------------------------- */

/* the pointers sound_tables_install gives the sound layer: two voice tables at +4, +8, then the sound-effect tables at +0xC and +0x10. */
typedef struct chara_sound_tables_rec {
    undefined field_0x0;
    undefined field_0x1;
    undefined field_0x2;
    undefined field_0x3;
    pointer voice0;           /* +0x4: chara_voice_table_set(0) */
    pointer voice1;           /* +0x8: chara_voice_table_set(1) */
    pointer se0;              /* +0xc: chara_se_table_set(0) */
} chara_sound_tables_rec;

/* ---- vf4lib/libcore/src/clip.c ----------------------------------------- */

/* the stored user clip rectangle and the clip mode in use. */
typedef struct clip_rect_rec {
    undefined4 x0;
    undefined4 y0;
    undefined4 x1;
    undefined4 y1;
    undefined4 clip_mode;     /* +0x10: user clip mode on the polygon headers */
} clip_rect_rec;

/* ---- vf4lib/libcore/src/fatal.c ---------------------------------------- */

/* the file, line and message of the last fatal error. */
typedef struct fatal_record_rec {
    undefined4 src_file;
    undefined4 line;
    undefined4 text;          /* +0x8: fatal_at_va's stack buffer */
} fatal_record_rec;

/* ---- vf4lib/libdebug/src/debug_menu.c ---------------------------------- */

/* the debug menu's CPU_FALSE counters: late frames and frames lost. */
typedef struct cpu_false_stats_rec {
    undefined4 late;          /* +0x0: frames over budget */
    undefined4 frames;        /* +0x4: whole frames of CPU time on those */
} cpu_false_stats_rec;

/* the debug menu's REN_FALSE counters: render times of one frame and of more. */
typedef struct render_false_stats_rec {
    undefined4 one;           /* +0x0: render took one frame */
    undefined4 more;          /* +0x4: render took more than one */
} render_false_stats_rec;

/* the LEVEL_CHANGE window's item ids: four slots for costume 0, then costume 1 (the last two bytes lie past the exported 6). */
typedef struct level_change_items_rec {
    undefined1 c0_slot0;      /* +0x0: costume 0 */
    undefined1 c0_slot1;
    undefined1 c0_slot2;
    undefined1 c0_slot3;
    undefined1 c1_slot0;      /* +0x4: costume 1 */
    undefined1 c1_slot1;
    undefined1 c1_slot2;
    undefined1 c1_slot3;
} level_change_items_rec;

/* the frame-check tool's record: flags, the saved state of both characters, the reaction row and guard/hit case. */
typedef struct frame_check_rec {
    undefined4 flags;         /* +0x0: bit 0 open */
    undefined field_0x4;
    undefined field_0x5;
    undefined field_0x6;
    undefined1 reaction_row;  /* +0x7: row of the 20 reactions */
    undefined1 reaction_row_run;/* +0x8: row frame_advantage ran */
    char hit_case;            /* +0x9: guard / hit case 0-3 */
    undefined1 hit_case_run;  /* +0xa: case frame_advantage ran */
    undefined field_0xb;
    undefined field_0xc;
    undefined field_0xd;
    undefined field_0xe;
    undefined field_0xf;
    undefined field_0x10;
    undefined field_0x11;
    undefined field_0x12;
    undefined field_0x13;
    undefined field_0x14;
    undefined field_0x15;
    undefined field_0x16;
    undefined field_0x17;
    undefined field_0x18;
    undefined field_0x19;
    undefined field_0x1a;
    undefined field_0x1b;
    undefined field_0x1c;
    undefined field_0x1d;
    undefined field_0x1e;
    undefined field_0x1f;
    undefined field_0x20;
    undefined field_0x21;
    undefined field_0x22;
    undefined field_0x23;
    undefined field_0x24;
    undefined field_0x25;
    undefined field_0x26;
    undefined field_0x27;
    undefined field_0x28;
    undefined field_0x29;
    undefined field_0x2a;
    undefined field_0x2b;
    undefined field_0x2c;
    undefined field_0x2d;
    undefined field_0x2e;
    undefined field_0x2f;
    undefined field_0x30;
    undefined field_0x31;
    undefined field_0x32;
    undefined field_0x33;
    undefined field_0x34;
    undefined field_0x35;
    undefined field_0x36;
    undefined field_0x37;
    undefined field_0x38;
    undefined field_0x39;
    undefined field_0x3a;
    undefined field_0x3b;
    undefined field_0x3c;
    undefined field_0x3d;
    undefined field_0x3e;
    undefined field_0x3f;
    undefined field_0x40;
    undefined field_0x41;
    undefined field_0x42;
    undefined field_0x43;
    undefined field_0x44;
    undefined field_0x45;
    undefined field_0x46;
    undefined field_0x47;
    undefined field_0x48;
    undefined field_0x49;
    undefined field_0x4a;
    undefined field_0x4b;
    undefined field_0x4c;
    undefined field_0x4d;
    undefined field_0x4e;
    undefined field_0x4f;
    undefined field_0x50;
    undefined field_0x51;
    undefined field_0x52;
    undefined field_0x53;
    undefined field_0x54;
    undefined field_0x55;
    undefined field_0x56;
    undefined field_0x57;
    undefined field_0x58;
    undefined field_0x59;
    undefined field_0x5a;
    undefined field_0x5b;
    undefined field_0x5c;
    undefined field_0x5d;
    undefined field_0x5e;
    undefined field_0x5f;
    undefined field_0x60;
    undefined field_0x61;
    undefined field_0x62;
    undefined field_0x63;
    undefined field_0x64;
    undefined field_0x65;
    undefined field_0x66;
    undefined field_0x67;
} frame_check_rec;

/* One side's player message (0x10C bytes, player_messages[side]): set by player_message_text_set, picked by player_message_pick, drawn by player_message_draw. */
typedef struct player_message {
    char set;                 /* +0x0: nonzero once a text is set; judge_mes_draw stores the card record's flag here */
    char sel;                 /* +0x1: the text to show (player_message_pick; -1 none) */
    char texts[4][61];        /* +0x2: up to four texts (snprintf'd with 0x3d) */
    undefined1 field_0xf6;
    undefined1 field_0xf7;
    float width;              /* +0xf8: the widest text's width (param_4 * 8) */
    float heights[4];         /* +0xfc: each text's height (2 * param_5 + 1) */
} player_message;

/* One side's entry of the MOTION_TEST window (0x2C bytes, motion_test +0xC + side * 0x2C). */
typedef struct motion_test_side {
    undefined4 field_0x0;
    undefined4 field_0x4;
    ushort motion;            /* +0x8: the motion to play (side 0's is the debug menu's mot_num); motion_test_restart clears bit 15 and skips unloaded ids */
    short prev_motion;        /* +0xa: the motion last started; a change of motion restarts */
    int start_x;              /* +0xc: start position (rob +0x84 at open), where motion_test_restart puts the rob */
    int start_y;
    int start_z;
    int saved_x;              /* +0x18: the rob's position kept by the test */
    int saved_y;
    int saved_z;
    float length;             /* +0x24: the motion's length in frames */
    float frame;              /* +0x28: the current frame (side 0's is the debug menu's ctrl_coma) */
} motion_test_side;

/* The MOTION_TEST window's record (0x64 bytes, up to frame_check): flags, the saved debug switches and camera mode, the play mode and frame step, then the two sides' entries. */
typedef struct motion_test_rec {
    undefined4 flags;         /* +0x0: bit 0 state saved by motion_test_open, bit 1 pad 1 steps the active frames (mode 4) */
    undefined1 saved_blend_switch;/* +0x4: DAT_0c292659 before the test */
    undefined1 saved_deb_sw;  /* +0x5: deb_sw before the test */
    undefined1 play_mode;     /* +0x6: 0 once, 1 loop, 2 / 3 step on pad 0, 4 play with pad 1 stepping */
    undefined1 saved_camera_mode;/* +0x7: camera.mode before the test (set to FIX) */
    float frame_step;         /* +0x8: frames added per step */
    motion_test_side side[2]; /* +0xc */
} motion_test_rec;

/* ---- vf4lib/libdebug/src/win_sys.c ------------------------------------- */

/* The 8 border characters of a window that is not focused (top, bottom, left, right edges, then the top-left, top-right, bottom-left, bottom-right corners); the focused set follows at 0x0C1E2B40. */
typedef struct win_border_glyph_set {
    undefined top;            /* +0x0: top edge */
    undefined bottom;         /* +0x1: bottom edge */
    undefined left;           /* +0x2: left edge */
    undefined right;          /* +0x3: right edge */
    undefined1 top_left;      /* +0x4: top-left corner; top-right, bottom-left, bottom-right follow */
} win_border_glyph_set;

/* ---- vf4lib/libeffect/src/effect_break.c ------------------------------- */

/* One of ROME's 11 breakable-object sources (0x1C bytes): animation id, piece list, done mask, position and a started flag. */
typedef struct rome_break_source {
    undefined anim_id;        /* +0x0: short: the model / motion id given to the object */
    undefined field_0x1;
    undefined field_0x2;
    undefined field_0x3;
    pointer pieces;           /* +0x4: piece model list, -1 ended */
    undefined done_mask;      /* +0x8: word or-ed into break_link_done_mask when done */
    undefined field_0x9;
    undefined field_0xa;
    undefined field_0xb;
    undefined field_0xc;
    undefined field_0xd;
    undefined field_0xe;
    undefined field_0xf;
    undefined field_0x10;
    undefined field_0x11;
    undefined field_0x12;
    undefined field_0x13;
    undefined field_0x14;
    undefined field_0x15;
    undefined field_0x16;
    undefined field_0x17;
    undefined4 flags;         /* +0x18: bit 0 started */
} rome_break_source;

/* ---- vf4lib/libeffect/src/effect_flare.c ------------------------------- */

/* A stage's lens flare record (0x2C bytes): element table, glare color and fog values, light position, glare angle and on flag. */
typedef struct lens_flare_rec {
    undefined4 elements;      /* +0x0: element model table */
    undefined4 field_0x4;
    undefined4 field_0x8;
    undefined4 glare_near;    /* +0xc: x the glare into fog near_dist */
    undefined4 glare_far;     /* +0x10: fog far_dist */
    undefined4 glare_strength;/* +0x14: x the glare into fog strength */
    undefined4 pos_x;         /* +0x18: light position */
    undefined4 pos_y;
    undefined4 pos_z;
    undefined4 glare_angle;   /* +0x24: degrees */
    undefined4 on;            /* +0x28: draw the elements */
} lens_flare_rec;

/* ---- vf4lib/libmotion/src/mot_tools.c ---------------------------------- */

/* One of the four motion slots (0x14 bytes): motion, smooth and mothead buffers, the loaded id range, bank and a mothead pending byte. */
typedef struct mot_slot {
    pointer mot_buf;          /* +0x0: {address, size} of the motdat buffer */
    undefined smooth_buf;     /* +0x4: word: {address, size} or 0 */
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
    pointer mothead_buf;      /* +0x8: {address, size} */
    short first_id;           /* +0xc: end id at +0x0E, bank +0x10, pending +0x11 */
} mot_slot;

/* The local-space point rob_eval_output aims a bone at. */
typedef struct rob_eval_aim_vec {
    float x;
    float y;
    float z;
} rob_eval_aim_vec;

/* ---- vf4lib/libpoly/src/pol_uti.c -------------------------------------- */

/* The last object memory allocation: address, size and the caller's tag string. */
typedef struct object_mem_rec {
    undefined4 addr;          /* +0x0: 0 when the size was 0 */
    undefined4 size;
    undefined4 tag;           /* +0x8: "get_object_mem()" */
} object_mem_rec;

/* ---- vf4lib/librobdisp/src/osage.c ------------------------------------- */

/* The wind vector the hair / cloth chains are pushed by this frame. */
typedef struct osage_wind_vec {
    undefined4 x;
    undefined4 y;
    undefined4 z;
} osage_wind_vec;

/* The offset added to every chain root descriptor's position. */
typedef struct osage_root_offset_vec {
    undefined4 x;
    undefined4 y;
    undefined4 z;
} osage_root_offset_vec;

/* ---- vf4lib/librobdisp/src/stage.c ------------------------------------- */

/* ---- vf4lib/libbase/src/user_back.c ------------------------------------ */

/* One 28-byte row of user_back_data's ranking tables (time_ranking_rows,
 * point_ranking_rows): written by time_ranking_insert / point_ranking_insert,
 * drawn by rank_row_time_draw / rank_row_name_count_draw. */
typedef struct ranking_row_rec {
    char name[21];
    char chara;
    uchar field_0x16[2];
    int value;
} ranking_row_rec;

/* A mission score record (0x38 bytes): the course rankings (three per course) and the challenge map's node hiscores in user_back_data; msnm_ranking_record_fill fills one. */
typedef struct mission_score_rec {
    char name[21];
    char chara;
    uchar pad_16[2];
    int time;
    uint score;
    int extra;
    int ranks[5];
} mission_score_rec;

/* One day of the coin ledger (36 bytes, user_back_data.ledger_daily_records): the six counter increments ledger_daily_update keeps, then the date (testmode's daily coin table). */
typedef struct ledger_day_rec {
    int coin_1p;
    int coin_2p;
    int coin_total;
    int credit_coin;
    int credit_service;
    int credit_total;
    int year;                 /* +0x18 */
    int month;
    int day;
} ledger_day_rec;

/* The rankings in the backup image (user_back_data.rankings, +0x18): the time and point ranking rows and the best win streak by character. Member comments give user_back_data offsets. */
typedef struct ranking_block_rec {
    /* +0x18: user_back_data +0x18: the 50 clear time ranking rows of 28 bytes (name, character +21, frames +24); time_ranking_insert writes them, rank_row_time_draw draws them; part of vf4lib/libbase/src/user_back.c */
    ranking_row_rec time_ranking_rows[50];
    /* +0x590: user_back_data +0x590: the 50 points ranking rows of 28 bytes (name, character +21, points +24); point_ranking_insert writes them, rank_row_name_count_draw draws them; part of vf4lib/libbase/src/user_back.c */
    ranking_row_rec point_ranking_rows[50];
    int best_win_streak[16]; /* +0xb08: the best win streak by character (game_judge_init) */
} ranking_block_rec;

/* The bookkeeping block of the backup image (user_back_data.bookkeep, +0xb48, 0xad0 bytes): play counts, times and results; play_time_account and testmode's bookkeeping pages reach it from its own base, and backup_data_clear_run copies it whole from the defaults. Member comments give user_back_data offsets. */
typedef struct bookkeep_rec {
    uint games_total;
    uint games_1p;
    uint games_vs;
    uint continues;
    uint challengers;
    uint total_time;
    uint play_time;
    uint field_0xb64;
    uint p1_round_time_sum;
    uint vs_round_time_sum;
    uint p1_rounds;
    uint vs_rounds;
    uchar field_0xb78[8];
    uint field_0xb80;
    uint select_time;
    uint select_runs;
    uint replay_time;
    uint replay_runs;
    uint draw_rounds;
    uchar field_0xb98[8];
    uint best_1p_round;
    uint best_vs_round;
    short chara_max_combo[16];
    short chara_max_damage[16];
    short chara_max_wall_combo[16];
    short chara_max_wall_damage[16];
    int course_results[5][16];
    int course_results_no_continue[5][16];
    int chara_play_counts[2][16];
    int stage_select_counts[30];
    uint vs_chara_counts[16][16];
    uchar field_0x13a0[40];
    uint stage_intro_time;
    uchar field_0x13cc[4];
    uint round_intro_time;
    uint winner_time;
    uint stage_intro_runs;
    uint appose_time;
    uint appose_runs;
    uint round_intro_runs;
    uint winner_runs;
    uint load_time;
    uint load_runs;
    uint judge_time;
    uint judge_runs;
    uint vs_play_time;
    uchar field_0x1400[72];
    int card_level_counts[16];
    uchar field_0x1488[136];
    uint p1_time;
    uint shortest_1p;
    uint longest_1p;
    uint vs_time;
    uint shortest_vs;
    uint longest_vs;
    uint stage_vs_rounds_flag40[30];
    uint stage_vs_rounds[30];
} bookkeep_rec;

/* The challenge block of the backup image (user_back_data.challenge, +0x24c0, 0xdf4 bytes): the challenge map hiscores, the course rankings and testmode's challenge totals; backup_data_clear_run copies it whole. Member comments give user_back_data offsets. */
typedef struct challenge_block_rec {
    undefined4 field_0x24c0;
    /* +0x24c4: the 36 0x38-byte hiscore records of the challenge map's nodes, stage x (stage + 1) / 2 + branch (chlg_map_node_get): name, character +0x15, time +0x18, score +0x1c; msnm_course_result_store keeps them */
    mission_score_rec chlg_map_hiscores[36];
    /* +0x2ca4: in user_back_data: per course 0xA8 bytes of three 0x38-byte ranking records (name, character +21, score +0x1C); msnm_course_totals_compute, msnm_ranking_shift_down and msnm_ranking_name_set keep them, rank_row_score_draw draws them; part of vf4lib/libbase/src/user_back.c */
    mission_score_rec course_ranking[8][3];
    int chlg_clear_num;       /* +0x31e4: testmode's challenge totals: CLEAR NUM */
    int chlg_clear_time_max;
    int chlg_clear_time_min;
    int chlg_clear_time_avg;
    int chlg_course_ranks[8][5]; /* +0x31f4: by course, the C, B, A2, A3, A ranks */
    int chlg_course_gets[8];
} challenge_block_rec;

/* The ALL.Net billing counts in the backup image (user_back_data.billing, +0x32bc), by player; testmode's billing page reaches them from their own base. Member comments give user_back_data offsets. */
typedef struct alab_billing_rec {
    int starts[2]; /* +0x32bc: play starts by player, alab_play_start */
    int continues[2]; /* +0x32c4: continues by player, alab_play_continue */
    int ends[2]; /* +0x32cc: play ends by player, alab_play_end */
} alab_billing_rec;

/* The game settings at the head of the backup image: a format word compared with the defaults, then the operator settings (win color, match points, game time, energy, difficulty, stage select, continue, win streak limit, gravity, button type, start chapter, flags). */
typedef struct user_back_data_rec {
    uint format_stamp;
    uchar win_color;
    char vs_match_point;
    char _1p_match_point;
    uchar game_time_vs;
    uchar game_time_1p;
    uchar energy_vs;
    uchar energy_1p;
    char rank_difficult;
    uchar stage_select;
    uchar field_0xd;
    uchar no_continue;
    char win_streak_limit;
    uchar gravity_mode;
    uchar button_type;
    uchar start_chapter;
    uchar field_0x13;
    uint option_bits;
    struct ranking_block_rec rankings;
    struct bookkeep_rec bookkeep;
    uchar card_use[2];
    uchar field_0x161a[16];
    uchar net_connect;
    uchar card_start;
    uchar field_0x162c[4];
    int decided_no_card;
    int decided_one_card;
    int decided_more_cards;
    int streak_challenge_count;
    int streak_challenge_frames;
    int close_hour;
    int close_minute;
    uint discarded_codes[10];
    uint discarded_code_next;
    int close_by_weekday;
    int close_weekday_hour[7];
    int close_weekday_minute[7];
    int advertise_connect;
    uint card_log_stamp;
    struct card_item_log_rec card_item_log[32];
    struct card_data_log_rec card_data_log[32];
    int net_log_slot_next;
    int net_timeout;
    int net_test_timeout;
    struct ledger_day_rec ledger_daily_records[30];
    int ledger_day;
    uint ledger_record;
    uchar win_bg_trans;
    uchar win_bg_color;
    uchar mission_off_on;
    uchar mission_rate;
    uchar replay_message;
    uchar mission_rate_test;
    ushort jisaku_no_touch_matches;
    ushort jisaku_no_lever_matches;
    ushort jisaku_no_g_matches;
    ushort jisaku_no_touch_high_matches;
    ushort jisaku_no_lever_high_matches;
    ushort jisaku_no_g_high_matches;
    ushort lottery_starts;
    ushort lottery_spins;
    ushort bonus_time_starts;
    ushort bonus_time_games;
    ushort bonus_time_shortest;
    ushort bonus_time_longest;
    ushort syuchu_marks;
    ushort prize_item_counts[8];
    ushort prize_matches;
    ushort prize_items_given;
    ushort prize_matches_bonus_time;
    ushort prize_matches_syuchu;
    ushort prize_matches_kind3;
    ushort prize_matches_plain;
    ushort mission_offered[30];
    ushort mission_cleared[30];
    struct item_pending_table_rec item_pending_table;
    struct challenge_block_rec challenge;
    uchar net_ver;
    uchar s_counter_damage;
    uchar b_counter_damage;
    uchar challenge_only;
    uchar alab_freq;
    uchar alab_enable;
    uchar effect_skip;
    uchar field_0x32bb;
    struct alab_billing_rec billing;
} user_back_data_rec;

/* ---- vf4lib/libbase/src/replay.c --------------------------------------- */

/* One of the 11 replay highlights kept for the ending: the recorded frames
   (a malloc'd 0xb040-byte buffer, rep_highlights_alloc), the frame range and
   mark, then the stage and both robs' character and costume (-1 when empty).
   rep_highlight_store's local ent struct has the same layout. */
typedef struct rep_highlight_rec {
    void *frames;
    short from;
    short to;
    short mark;
    char stage;
    char rob0_chara;
    char rob1_chara;
    char rob0_costume;
    char rob1_costume;
    char pad;
} rep_highlight_rec;

/* ---- vf4lib/libbase/src/static_heap.c ---------------------------------- */

/* A static heap block record that mem_block_set fills: the block's address, its size and a label string. */
typedef struct static_heap_rec {
    void *addr;
    uint size;
    char *label;
} static_heap_rec;

/* ---- vf4lib/libbase/src/system_common.c -------------------------------- */

/* The 0x58-byte system parameter record hardware_boot clears: the country (twice, with non-zero flags), its initialized flag, a copy of the board record and the media words media_board_probe stores. */
typedef struct system_param_value_rec {
    undefined1 field_0x0;
    undefined1 initialized_country;/* +0x1: set to 1 by system_param_country_init; the assert text names it */
    undefined field_0x2;
    undefined field_0x3;
    undefined4 country;       /* +0x4: 0-4 from the first word of board_config_ptr(); returned by system_param_country_get */
    undefined4 country_nonzero;/* +0x8: 1 when country is not 0 */
    undefined4 country_copy;  /* +0xc: the same country, stored again; no reader seen */
    undefined4 country_nonzero_copy;/* +0x10: the same flag, stored again; no reader seen */
    undefined4 field_0x14;
    undefined4 field_0x18;
    undefined4 field_0x1c;
    undefined4 field_0x20;
    undefined4 field_0x24;
    undefined4 media_kind;    /* +0x28: media_board_probe word 0: 2 DIMM board, 1 when 0xa05f7014 answers, else 0 */
    undefined field_0x2c;
    undefined field_0x2d;
    undefined field_0x2e;
    undefined field_0x2f;
    undefined4 media_limit;   /* +0x30: media_board_probe word 2: DIMM size << 20, or the ROM limit; returned by media_limit_get */
    undefined1 main_board_id[17];/* +0x34: the main board serial (board_ids_read, board_serial_numbers' first string), printed as "MAIN BOARD ID %s" */
    undefined1 dimm_board_id[17];/* +0x45: the DIMM board serial, printed as "DIMM BOARD ID %s" */
    undefined field_0x56;
    undefined field_0x57;
} system_param_value_rec;

/* ---- vf4lib/libsprite/src/aet_play.c ----------------------------------- */

/* One key of an AET track (16 bytes): aet_key_curve_eval blends two keys as a cubic Hermite curve. */
typedef struct aet_key {
    float frame;                  /* +0x00: key time; aet_key_curve_eval searches on it */
    float value;                  /* +0x04: aet_key_curve_eval */
    float in_slope;               /* +0x08: tangent into this key (aet_key_curve_eval: k->in_slope) */
    float out_slope;              /* +0x0c: tangent out of this key (aet_key_curve_eval: k[-1].out_slope) */
} aet_key;          /* 0x10 */

/* An AET track: its keys (aet_key_curve_eval). */
typedef struct aet_track {
    int key_count;                /* +0x00: aet_key_curve_eval */
    struct aet_key *keys;         /* +0x04: aet_key_curve_eval; relocated by aet_track_relocate */
} aet_track;          /* 0x8 */

/* A named frame of a layer (aet_layer_marker_find); no file uses them. */
typedef struct aet_marker {
    float frame;                  /* +0x00: aet_obj_marker_frame, aet_obj_markers_set, aet_obj_start */
    char *name;                   /* +0x04: aet_comp_marker_find, aet_layer_marker_find, aet_obj_markers_print; relocated by aet_marker_relocate */
} aet_marker;          /* 0x8 */

/* One picture of an image (0x14): its texture slot and UVs. */
typedef struct aet_picture {
    int tex_index;                /* +0x00: index into the scene's texture table: aet_image_layer_draw, aet_still_image_draw, aet_scene_textures_load / _free */
    float u0;                     /* +0x04: aet_quad_build */
    float v0;                     /* +0x08: aet_quad_build */
    float u1;                     /* +0x0c: aet_quad_build */
    float v1;                     /* +0x10: aet_quad_build */
} aet_picture;          /* 0x14 */

/* An image item of a scene (0x18), drawn by a type-0 layer. */
typedef struct aet_image {
    int width;                    /* +0x00: aet_bounds_layer_add, aet_image_layer_draw, aet_still_image_draw */
    int height;                   /* +0x04: as width */
    int picture_rate;             /* +0x08: pictures per frame (aet_image_picture_set: (int)(rate * frame)) */
    int picture_count;            /* +0x0c: aet_image_picture_set, aet_scene_textures_load / _free */
    struct aet_picture *pictures; /* +0x10: both image draws, aet_scene_textures_load / _free; relocated by aet_image_relocate */
    int picture;                  /* +0x14: the current picture: written by aet_image_picture_set, read by both image draws */
} aet_image;          /* 0x18 */

/* What a layer draws: an image (type 0) or a composition (type 1). */
typedef union aet_layer_item {
    struct aet_image *image;      /* type 0 layer: aet_layer_draw, aet_still_layer_draw; relocated by aet_layer_relocate */
    struct aet_comp *comp;        /* type 1 layer: aet_comp_layer_find, aet_comp_marker_find, aet_layer_marker_find, aet_still_build, aet_layer_draw */
} aet_layer_item;

/* An AET layer (0x8C). +0x14 .. +0x34 are written at run time; the rest is file data (docs/asset-formats.md, 2D scenes; live-checked). */
typedef struct aet_layer {
    char *name;                   /* +0x00: aet_comp_layer_find; relocated by aet_layer_relocate */
    float start_frame;            /* +0x04: aet_layer_tracks_eval, aet_layer_draw */
    float end_frame;              /* +0x08: aet_layer_tracks_eval */
    float frame_offset;           /* +0x0c: aet_layer_draw: child frame = (frame - start) * time_scale + offset */
    float time_scale;             /* +0x10: aet_layer_draw */
    float anchor_x;               /* +0x14: the eight track values aet_layer_tracks_eval writes each frame start here; aet_layer_transform */
    float anchor_y;               /* +0x18: aet_layer_transform */
    float pos_x;                  /* +0x1c: aet_layer_transform */
    float pos_y;                  /* +0x20: aet_layer_transform */
    float scale_x;                /* +0x24: percent; aet_layer_transform */
    float scale_y;                /* +0x28: percent; aet_layer_transform */
    float rotation;               /* +0x2c: degrees; aet_layer_transform */
    float opacity;                /* +0x30: percent; aet_layer_draw, aet_still_layer_draw */
    char visible;                 /* +0x34: written by aet_layer_tracks_eval, read by aet_layer_draw */
    char blend;                   /* +0x35: 4 additive (aet_blend_select); both image draws */
    undefined1 field_0x36;        /* +0x36 */
    char no_send;                 /* +0x37: skips the strip send; both image draws */
    ushort flags;                 /* +0x38: bit 0: shown only from start_frame to end_frame (aet_layer_tracks_eval) */
    char type;                    /* +0x3a: 0 image, 1 composition, 2 measured box: aet_layer_draw, the finds, aet_still_* */
    undefined1 field_0x3b;        /* +0x3b */
    aet_layer_item item;          /* +0x3c: the image or composition (aet_layer_item) */
    struct aet_layer *parent;     /* +0x40: aet_layer_transform applies the parent first; relocated by aet_layer_relocate */
    aet_track tracks[8];          /* +0x44: anchor x / y, position x / y, scale x / y, rotation, opacity: aet_layer_tracks_eval, aet_layer_relocate */
    int marker_count;             /* +0x84: aet_comp_marker_find, aet_layer_marker_find, aet_obj_markers_print, aet_layer_relocate */
    struct aet_marker *markers;   /* +0x88: as marker_count */
} aet_layer;          /* 0x8c */

/* An AET composition (0x2C): a list of layers; the last one of a scene is the main one. */
typedef struct aet_comp {
    undefined4 field_0x0;         /* +0x00 */
    undefined4 field_0x4;         /* +0x04 */
    float start_frame;            /* +0x08: aet_comp_visible_update, aet_obj_start */
    float end_frame;              /* +0x0c: as start_frame */
    undefined4 field_0x10;        /* +0x10 */
    undefined4 field_0x14;        /* +0x14 */
    undefined4 field_0x18;        /* +0x18 */
    undefined4 field_0x1c;        /* +0x1c */
    char visible;                 /* +0x20: written by aet_comp_visible_update, read by aet_comp_draw */
    undefined1 field_0x21;        /* +0x21 */
    undefined1 field_0x22;        /* +0x22 */
    undefined1 field_0x23;        /* +0x23 */
    int layer_count;              /* +0x24: aet_comp_draw, aet_still_build, the finds, aet_comp_relocate */
    struct aet_layer *layers;     /* +0x28: aet_comp_draw, aet_still_comp_draw, the finds, aet_comp_relocate */
} aet_comp;          /* 0x2c */

/* An entry of a scene's third table (0x18); 0 entries in every file, aet_table3_item_relocate does nothing. */
typedef struct aet_table3_item {
    undefined4 field_0x0;         /* +0x00 */
    undefined4 field_0x4;         /* +0x04 */
    undefined4 field_0x8;         /* +0x08 */
    undefined4 field_0xc;         /* +0x0c */
    undefined4 field_0x10;        /* +0x10 */
    undefined4 field_0x14;        /* +0x14 */
} aet_table3_item;          /* 0x18 */

/* An AET scene (0x18). */
typedef struct aet_scene {
    int comp_count;               /* +0x00: aet_main_comp_get (the last composition is the main one), aet_scene_relocate */
    struct aet_comp *comps;       /* +0x04: as comp_count */
    int image_count;              /* +0x08: aet_scene_textures_load / _free, aet_scene_relocate */
    struct aet_image *images;     /* +0x0c: as image_count */
    int table3_count;             /* +0x10: aet_scene_relocate; 0 in every file */
    struct aet_table3_item *table3;/* +0x14: aet_scene_relocate (aet_table3_item_relocate does nothing) */
} aet_scene;          /* 0x18 */

/* A scene's texture table and sprite-set list (the header's fourth table). */
typedef struct aet_scene_tex {
    int *tex_table;               /* +0x00: texture numbers by picture: aet_scene_tex_table_get */
    int *sprite_sets;             /* +0x04: sprite set numbers ended by -1: aet_scene_sprite_sets_get */
} aet_scene_tex;          /* 0x8 */

/* The top of a loaded AET file: four offsets, made pointers by aet_file_relocate. */
typedef struct aet_file_header {
    int *scene_count;             /* +0x00: aet_set_scene_count, aet_file_relocate */
    struct aet_scene **scenes;    /* +0x04: aet_scene_get, aet_file_relocate */
    char **table2;                /* +0x08: one string per scene (its name in every file): aet_scene_table2_get, aet_file_relocate */
    struct aet_scene_tex *scene_tex;/* +0x0c: aet_scene_tex_table_get, aet_scene_sprite_sets_get, aet_file_relocate */
} aet_file_header;          /* 0x10 */

/* The state part of an aet_sets record (+0x80), which the game reaches through a base of its own. */
typedef struct aet_set_info {
    int state;                    /* +0x00: 4 loaded: aet_set_load, aet_scene_get, aet_set_clear */
    char loaded;                  /* +0x04: aet_set_load, aet_set_clear */
    undefined1 field_0x5;         /* +0x05 */
    undefined1 field_0x6;         /* +0x06 */
    undefined1 field_0x7;         /* +0x07 */
    struct aet_file_header *file; /* +0x08: the relocated file: aet_set_load, aet_scene_get, the table getters, aet_set_clear */
    struct aet_stack_entry *stack_entry;/* +0x0c: aet_set_load, aet_set_free */
} aet_set_info;          /* 0x10 */

/* One AET set record (0x90, aet_sets[30]). */
typedef struct aet_set_rec {
    char name[128];               /* +0x00: the file name: aet_set_load */
    aet_set_info info;            /* +0x80: the game's +0x80 base (aet_set_info) */
} aet_set_rec;          /* 0x90 */

/* A vertex of an AET quad (0x1C). */
typedef struct aet_quad_vertex {
    float x;                      /* +0x00: aet_quad_build, aet_still_quad_cache_store, aet_still_move_apply */
    float y;                      /* +0x04: as x */
    float z;                      /* +0x08: as x */
    float u;                      /* +0x0c: aet_quad_build, aet_still_quad_cache_store */
    float v;                      /* +0x10: as u */
    uint color;                   /* +0x14: ARGB: aet_quad_build, aet_still_color_apply, aet_still_quad_cache_store */
    undefined4 field_0x18;        /* +0x18: copied only by aet_still_quad_cache_store */
} aet_quad_vertex;          /* 0x1c */

/* An AET quad (0x88): aet_quad_build fills it, aet_strip_send sends it. */
typedef struct aet_quad {
    aet_quad_vertex v[4];         /* +0x00: built by aet_quad_build */
    int tex;                      /* +0x70: both image draws, aet_strip_send, aet_still_quad_cache_store */
    char alpha;                   /* +0x74: aet_quad_build, aet_strip_send, aet_still_quad_cache_store */
    undefined1 field_0x75;        /* +0x75 */
    undefined1 field_0x76;        /* +0x76 */
    undefined1 field_0x77;        /* +0x77 */
    undefined1 field_0x78;        /* +0x78 */
    undefined1 field_0x79;        /* +0x79 */
    undefined1 field_0x7a;        /* +0x7a */
    undefined1 field_0x7b;        /* +0x7b */
    undefined4 field_0x7c;        /* +0x7c: copied from the start block's field_0xc / field_0x14 by the image draws; never read */
    float z;                      /* +0x80: depth: both image draws, aet_still_quad_cache_store */
    uint color;                   /* +0x84: aet_quad_build, both image draws */
} aet_quad;          /* 0x88 */

/* A measured box (aet_obj_bounds_measure). */
typedef struct aet_box {
    vec3 min;                     /* +0x00: aet_obj_bounds_measure, aet_bounds_layer_add */
    vec3 max;                     /* +0x0c: as min */
} aet_box;          /* 0x18 */

/* The AET draw pass record (aet_draw_state, 0xB8). */
typedef struct aet_draw_state_rec {
    uchar measuring;              /* +0x00: 1 in a bounds pass: set by aet_obj_bounds_measure, read by aet_bounds_layer_add (uchar is what matches) */
    undefined1 field_0x1;         /* +0x01 */
    undefined1 field_0x2;         /* +0x02 */
    undefined1 field_0x3;         /* +0x03 */
    aet_quad last_quad;           /* +0x04: aet_last_quad_keep */
    char last_skip;               /* +0x8c: aet_objs_draw, aet_last_skip_put */
    char last_blend;              /* +0x8d: aet_objs_draw, aet_last_blend_put */
    undefined1 field_0x8e;        /* +0x8e */
    undefined1 field_0x8f;        /* +0x8f */
    float last_alpha;             /* +0x90: aet_objs_draw, aet_last_alpha_put */
    aet_box box;                  /* +0x94: the measured box: aet_obj_bounds_measure, aet_bounds_layer_add */
    uchar field_0xac;             /* +0xac: set to 0xFF by aet_objs_draw, never read */
    undefined1 field_0xad;        /* +0xad */
    undefined1 field_0xae;        /* +0xae */
    undefined1 field_0xaf;        /* +0xaf */
    int blend_override;           /* +0xb0: nonzero forces normal blending: aet_objs_draw, both image draws */
    int quads_drawn;              /* +0xb4: reset by aet_objs_draw, counted by aet_image_layer_draw */
} aet_draw_state_rec;          /* 0xb8 */

/* The start block of an AET object (0x44): aet_param_init fills the defaults, aet_param_*_set the rest, aet_obj_start copies it. */
typedef struct aet_obj_param {
    uint scene_id;                /* +0x00: set << 16 | scene: aet_param_scene_set, aet_obj_start, the by-scene / by-set stops and counts */
    ushort flags;                 /* +0x04: 1 default, 2 loop, 4 hold at end, 8 paused, 0x10 reverse, 0x20 hidden: aet_param_flags_set, aet_objs_step, aet_obj_flag_* */
    undefined1 field_0x6;         /* +0x06 */
    undefined1 field_0x7;         /* +0x07 */
    int *tex_table;               /* +0x08: aet_param_tex_table_set, aet_obj_start, aet_image_layer_draw */
    undefined4 field_0xc;         /* +0x0c: zeroed by aet_param_init, copied to the quad's field_0x7c by aet_image_layer_draw */
    float x;                      /* +0x10: aet_param_pos_set, aet_obj_pos_set, aet_obj_draw */
    float y;                      /* +0x14: as x */
    float rotation;               /* +0x18: degrees, -1 none (aet_param_init); aet_obj_draw */
    uint color;                   /* +0x1c: RGB, 0xFFFFFF none: aet_param_color_set, aet_image_layer_draw */
    char *start_marker;           /* +0x20: aet_obj_start, aet_obj_markers_set */
    char *end_marker;             /* +0x24: as start_marker */
    char *loop_marker;            /* +0x28: aet_obj_start, aet_objs_step */
    char *layer_name;             /* +0x2c: play one layer only: aet_param_layer_set, aet_obj_start */
    float depth;                  /* +0x30: the current depth: aet_objs_draw, aet_image_layer_draw, aet_depth_step */
    float depth_top;              /* +0x34: aet_param_depth_range_set, aet_objs_draw */
    float depth_bottom;           /* +0x38: aet_param_depth_range_set, aet_depth_step */
    float depth_step;             /* +0x3c: as depth_bottom */
    char filter;                  /* +0x40: aet_param_filter_set, aet_objs_draw */
    uchar alpha;                  /* +0x41: alpha x 255: aet_param_alpha_set, aet_image_layer_draw */
    undefined1 field_0x42;        /* +0x42 */
    undefined1 field_0x43;        /* +0x43 */
} aet_obj_param;          /* 0x44 */

/* A playing AET object (0x78, aet_objs[64]). */
typedef struct aet_obj_rec {
    char in_use;                  /* +0x00: every aet_obj* routine */
    undefined1 field_0x1;         /* +0x01 */
    undefined1 field_0x2;         /* +0x02 */
    undefined1 field_0x3;         /* +0x03 */
    aet_obj_param param;          /* +0x04: the start block, copied in by aet_obj_start */
    int handle;                   /* +0x48: aet_obj_start, aet_obj_index, aet_obj_is_playing, aet_obj_clear */
    struct aet_scene *scene;      /* +0x4c: aet_obj_start, aet_obj_clear */
    struct aet_comp *comp;        /* +0x50: the main composition: aet_obj_start, aet_obj_draw, the marker lookups */
    struct aet_layer *layer;      /* +0x54: the named layer: aet_obj_start, aet_obj_draw, aet_obj_marker_frame */
    int frame;                    /* +0x58: aet_objs_step, aet_obj_frame_get / _add, aet_obj_draw (live-checked: an int) */
    int start_frame;              /* +0x5c: aet_obj_start, aet_obj_markers_set, aet_objs_step, aet_obj_start_frame_get */
    int end_frame;                /* +0x60: as start_frame, aet_obj_end_frame_get */
    int first_end_frame;          /* +0x64: aet_obj_start, aet_objs_step */
    int field_0x68;               /* +0x68: written with the start frame by aet_obj_start, never read */
    int loop_frame;               /* +0x6c: aet_obj_start, aet_objs_step */
    int end_reached;              /* +0x70: aet_objs_step, aet_obj_end_reached, aet_obj_markers_set */
    uint flags;                   /* +0x74: 0x40 hidden: aet_obj_hide_set, aet_objs_draw */
} aet_obj_rec;          /* 0x78 */

/* The start block of an AET still (0x4C): aet_still_param_init and aet_still_param_*_set; aet_still_start copies it. */
typedef struct aet_still_param {
    uint scene_id;                /* +0x00: aet_still_param_scene_set, aet_still_start, aet_stills_stop_by_scene */
    int *tex_table;               /* +0x04: aet_still_param_tex_table_set, aet_still_start, aet_still_image_draw */
    undefined4 field_0x8;         /* +0x08: zeroed by aet_still_param_init only */
    float x;                      /* +0x0c: aet_still_param_pos_set, aet_still_move_to, aet_still_build */
    float y;                      /* +0x10: as x */
    undefined4 field_0x14;        /* +0x14: zeroed by aet_still_param_init, copied to the quad's field_0x7c by aet_still_image_draw */
    float rotation;               /* +0x18: aet_still_rotation_set, aet_still_rotate_matrix */
    char *layer_name;             /* +0x1c: aet_still_param_layer_set, aet_still_start */
    float scale[3];               /* +0x20: aet_still_scale_set, aet_still_scale_matrix */
    float depth;                  /* +0x2c: aet_still_image_draw, aet_still_depth_step */
    float depth_top;              /* +0x30: aet_still_param_depth_range_set, aet_still_depth_range_set */
    float depth_bottom;           /* +0x34: as depth_top, aet_still_depth_step */
    float depth_step;             /* +0x38: as depth_bottom */
    uint color;                   /* +0x3c: aet_still_param_color_set, aet_still_color_set, aet_still_color_apply */
    float move_x;                 /* +0x40: aet_still_move_to, aet_still_move_apply */
    float move_y;                 /* +0x44: as move_x */
    char filter;                  /* +0x48: aet_still_param_filter_set, aet_stills_draw */
    uchar alpha;                  /* +0x49: alpha x 255: aet_still_param_alpha_set, aet_still_color_apply */
    undefined1 field_0x4a;        /* +0x4a */
    undefined1 field_0x4b;        /* +0x4b */
} aet_still_param;          /* 0x4c */

/* An AET still (0x6C, aet_stills[64]): drawn once into a quad cache, then moved, turned, scaled or recolored. */
typedef struct aet_still_rec {
    char in_use;                  /* +0x00: every aet_still* routine */
    undefined1 field_0x1;         /* +0x01 */
    undefined1 field_0x2;         /* +0x02 */
    undefined1 field_0x3;         /* +0x03 */
    aet_still_param param;        /* +0x04: the start block, copied in by aet_still_start */
    int handle;                   /* +0x50: aet_still_start, aet_still_get, aet_still_index, aet_still_clear */
    struct aet_scene *scene;      /* +0x54: aet_still_start, aet_still_clear */
    struct aet_comp *comp;        /* +0x58: aet_still_start, aet_still_build */
    struct aet_layer *layer;      /* +0x5c: aet_still_build, aet_still_quad_cache_store, the move / scale applies, aet_still_update_draw */
    uint flags;                   /* +0x60: 1 build, 2 move, 4 rotate, 8 scale, 0x10 color, 0x20 composition layer, 0x40 hidden: set by the *_set routines, consumed by aet_still_update_draw */
    int last_quad;                /* +0x64: the highest cached quad (layer count - 1): aet_still_build, aet_still_comp_draw, aet_still_move_apply, aet_still_update_draw */
    int quad_slot;                /* +0x68: the next cache slot, counting down: aet_still_build, aet_still_quad_cache_store */
} aet_still_rec;          /* 0x6c */

/* A still's quad cache (0x738, aet_still_caches[64]). */
typedef struct aet_still_cache_rec {
    char in_use;                  /* +0x00: aet_still_cache_clear, the apply routines */
    undefined1 field_0x1;         /* +0x01 */
    undefined1 field_0x2;         /* +0x02 */
    undefined1 field_0x3;         /* +0x03 */
    int handle;                   /* +0x04: aet_still_cache_get, aet_still_cache_index */
    aet_quad quads[10];           /* +0x08: aet_still_quad_cache_store, the move / color applies, aet_still_update_draw */
    vec3 corners[10][4];          /* +0x558: the corners before the transform: aet_still_quad_cache_store, aet_still_scale_matrix */
} aet_still_cache_rec;          /* 0x738 */


/* readability round, agent R2 */
/* one frame of a player's input history (0x28 bytes): control_history[player][64], control +0x80; written by input_sample, read by control_history_scan, script_step_back and the script ops */
typedef struct control_history_rec control_history_rec, *Pcontrol_history_rec;

struct control_history_rec {
    uint rob_flags;                          /* +0x0: the rob's flags (+0x18) at the frame (key evidence: input_sample, script_op_66_if_frame_flag) */
    ushort prev_buttons;                     /* +0x4: the buttons of the frame before (key evidence: input_sample) */
    ushort releases;                         /* +0x6: buttons released (key evidence: input_sample, control_history_scan) */
    ushort dirs;                             /* +0x8: sticky raw directions (key evidence: input_sample, script_op_24_switch_held_dir) */
    ushort presses;                          /* +0xa: new presses (key evidence: input_new_presses result, control_history_scan) */
    ushort buttons;                          /* +0xc: held buttons and directions (key evidence: input_sample, control_history_scan) */
    ushort flag_0x80;                        /* +0xe: bit 7 of the control flags at the frame (key evidence: input_sample, control_history_scan) */
    ushort counters[11];                     /* +0x10: counter flags, then the held / repeat counters of input_combo_counters (key evidence: input_sample, script_cond, move_branch_cond) */
};

/* a fighter's control set (0x20 bytes; ctrl_<chr>.bin, one per stance, docs/asset-formats.md "Control sets") */
typedef struct control_set_rec control_set_rec, *Pcontrol_set_rec;

struct control_set_rec {
    undefined *scripts[6];                   /* +0x0: scripts: 0 down, 1 hold release, 2 neutral, 4 / 5 the rob +0x24 contexts (key evidence: control_context_update, control_ctx_down, control_ctx_rob24_bit0 / 1) */
    undefined **table;                       /* +0x18: script table of ops 12 / 77 (key evidence: script_op_12_continue_with, script_op_77_move_if_card_flag) */
    uint count;                              /* +0x1c: entries in table (key evidence: script_op_12_continue_with) */
};

/* a player's control object (0x9c bytes): control_objects[player], rob +0x48, back pointer +0x84 */
typedef struct control_rec control_rec, *Pcontrol_rec;

struct control_rec {
    uint flags;                              /* +0x0: control flag bits (key evidence: control_context_update, control_frame_try) */
    char side;                               /* +0x4: player side, the rob's +0 (key evidence: control_init) */
    char context_prev;                       /* +0x5: the last frame's input context (key evidence: control_context_update) */
    char context;                            /* +0x6: input context, indexes control_context_funcs (key evidence: control_context_update, control_command_step) */
    uchar buffer_kind;                       /* +0x7: kind bits of the buffered script (key evidence: control_buffer_use, control_buffer_move) */
    uchar buffer_kind_2;                     /* +0x8: kind h4 of script_op_26_buffer (key evidence: script_op_26_buffer, control_slot_clear) */
    uchar pending_kind;                      /* +0x9: kind bits of the pending script (key evidence: control_command_step, control_buffer_move) */
    uchar branch_kind;                       /* +0xa: byte 0 of the pending branch's entry (key evidence: script_wait_frame, control_guard_press) */
    char field_0xb[1];
    short scan_limit;                        /* +0xc: history position script_step_back stops at (key evidence: script_step_back, script_op_25_history_mark) */
    short scan_pos;                          /* +0xe: history scan position (key evidence: control_scan_from, control_history_scan) */
    short mark;                              /* +0x10: history mark set by a script result 1 (key evidence: control_init, move_script_run) */
    short mark_keep;                         /* +0x12: history mark set by a script result 1 or 2 (key evidence: control_init, move_script_run) */
    short mark_saved;                        /* +0x14: mark kept by a script result 5; -1 none (key evidence: control_init, move_script_run) */
    short history_idx;                       /* +0x16: newest history entry (ring of 64) (key evidence: input_sample) */
    short dir_scan_pos;                      /* +0x18: scan position when a direction press started (key evidence: input_new_presses, control_guard_press) */
    short dir_frames;                        /* +0x1a: frames left of a direction press (key evidence: input_new_presses) */
    char field_0x1c[2];
    ushort prev_buttons;                     /* +0x1e: the buttons of the frame before (key evidence: input_sample) */
    ushort releases;                         /* +0x20: buttons released (key evidence: input_sample) */
    ushort presses;                          /* +0x22: new presses (key evidence: input_sample) */
    ushort dirs;                             /* +0x24: sticky raw directions (key evidence: input_sample) */
    ushort buttons;                          /* +0x26: held buttons and directions (key evidence: input_sample) */
    ushort cpu_buttons;                      /* +0x28: buttons the CPU player sets, read when rob flags bit 2 is clear (key evidence: input_sample, vp_main) */
    ushort prev_presses;                     /* +0x2a: the new presses of the frame before (key evidence: input_sample) */
    ushort counters[11];                     /* +0x2c: counter flags, then the held / repeat counters (key evidence: input_sample, move_branch_cond) */
    ushort dir_bits;                         /* +0x42: directions gathered during a direction press (key evidence: input_new_presses) */
    short field_0x44;                        /* +0x44: 0xf1 before a script runs (key evidence: control_script_try, control_dir_script_run) */
    short field_0x46;                        /* +0x46: 0xff before a script runs (key evidence: control_script_try, control_dir_script_run) */
    short wait;                              /* +0x48: frames the continuation waits; 0x7fff hold (key evidence: script_wait_frame, script_wait_frames) */
    short branch_wait;                       /* +0x4a: frames the pending branch waits; 0x7fff hold (key evidence: script_wait_frame, control_command_step) */
    short pending_wait;                      /* +0x4c: frames the pending script lasts (key evidence: script_op_26_buffer, control_command_step) */
    short buffer_wait;                       /* +0x4e: frames of the buffered script (key evidence: script_op_26_buffer, control_buffer_move) */
    uint rob_flags;                          /* +0x50: the rob's flags (+0x18) at the newest frame (key evidence: input_sample) */
    uint state;                              /* +0x54: script register "state" (key evidence: script_op_20_if_state_eq, docs/asset-formats.md) */
    char field_0x58[4];
    control_set_rec **control_sets;          /* +0x5c: the fighter's control sets by stance (rob +0x550) (key evidence: control_context_update, docs/asset-formats.md) */
    undefined **branch_pending;              /* +0x60: pending branch, a position in a branch's instruction-pointer list (key evidence: control_command_step, control_branch_pending_get) */
    undefined *dir_branch_list;              /* +0x64: the rob's branch list when a direction press started (key evidence: input_new_presses) */
    undefined *dir_branch_list_done;         /* +0x68: that branch list once the direction press ended (key evidence: input_new_presses, control_command_step) */
    undefined *continuation;                 /* +0x6c: script resumed next frame (key evidence: script_op_12_continue_with, control_command_step) */
    undefined *field_0x70;                   /* +0x70: script pointer set by ops 59 / 73 (key evidence: script_op_59_move_or_hold, script_op_73_set_rob1e8) */
    undefined *pending_script;               /* +0x74: pending script (key evidence: script_op_26_buffer, control_command_step) */
    undefined *buffer_script;                /* +0x78: buffered script (key evidence: control_buffer_move) */
    undefined *buffer_script_2;              /* +0x7c: script w8 of script_op_26_buffer (key evidence: script_op_26_buffer, control_slot_clear) */
    control_history_rec *history;            /* +0x80: the player's 64-entry input history (key evidence: control_init) */
    struct rob_rec *rob;                     /* +0x84: the fighter (key evidence: control_init) */
    int slot_marks[2];                       /* +0x88: slot kinds a script filled (key evidence: control_slot_mark, control_marked_slots_clear) */
    uint resume_move;                        /* +0x90: move word kept by script_op_46_move_masked (key evidence: control_hold_release_check) */
    uint resume_mask;                        /* +0x94: input kept with it (key evidence: script_op_46_move_masked, control_hold_release_check) */
    undefined *resume_script;                /* +0x98: resume script (key evidence: script_op_47_set_resume, control_hold_release_check) */
};

/* a player's input log (0xa8 bytes): input_log[player]; five 16-entry rings */
typedef struct input_log_rec input_log_rec, *Pinput_log_rec;

struct input_log_rec {
    uint flags;                              /* +0x0: flag bits (key evidence: input_log_clear, move_request_kind2_demote) */
    ushort word;                             /* +0x4: word input_log_word_set stores (key evidence: input_log_word_set) */
    ushort dirs[16];                         /* +0x6: direction ring (key evidence: input_log_push, input_log_dir_get) */
    ushort buttons[16];                      /* +0x26: button ring (key evidence: input_log_push) */
    ushort presses[16];                      /* +0x46: new press ring (key evidence: input_log_push, input_log_press_get) */
    ushort held[16];                         /* +0x66: held input ring (key evidence: input_log_push, input_log_held_get) */
    short extra[16];                         /* +0x86: extra word ring (key evidence: input_log_extra_set) */
    char idx;                                /* +0xa6: ring index 0-15 (key evidence: input_log_push) */
    char field_0xa7[1];
};

/* readability round, agent R4 */
/* one collision sphere of a character (26 at character record +0x3C, 0x14 bytes each): rob_coli_init, rob_coli_spheres_update */
typedef struct chara_sphere_rec chara_sphere_rec, *Pchara_sphere_rec;

struct chara_sphere_rec {
    vec3 pos;                                /* +0x0: center in the slot matrix's frame (mtx_xform_point) */
    float radius;                            /* +0xc: radius (rob_coli_init copies it to the collision record's rad_a) */
    int slot;                                /* +0x10: slot matrix index (rob +0x54 + slot * 0x40) */
};

/* readability round, agent R5 */
/* vp_rob_snap_rec: the CPU player's copy of its fighter's state, taken at the end of each
   frame (vp_rob_state_snapshot); vp_work.snap, one per side at 0x0C4205D0 (0x10 bytes) */
typedef struct vp_rob_snap_rec vp_rob_snap_rec, *Pvp_rob_snap_rec;
struct vp_rob_snap_rec {
    char action_type;                        /* +0x0: rob action.part.type */
    char field_0x1[1];
    ushort move_no;                          /* +0x2: rob move_no */
    uint state;                              /* +0x4: rob state */
    uint flags24;                            /* +0x8: rob flags24 */
    uint flags28;                            /* +0xc: rob flags28 */
};
/* vp_hame_rec: one row of the CPU player's log of opponent moves
   (vp_hame_count_add / vp_hame_count_decay / vp_hame_known): the move vp_mode_guard_success
   guarded, counted up when the CPU was then hit; vp_work.log, 64 per side
   at 0x0C4202E0 (0x4 bytes) */
typedef struct vp_hame_rec vp_hame_rec, *Pvp_hame_rec;

struct vp_hame_rec {
    ushort move;                             /* +0x0: opponent move number; 0xFFFF free */
    short count;                             /* +0x2: +2 by vp_hame_count_add, -1 by vp_hame_count_decay */
};
/* vp_level_params_rec: the CPU player's parameters for one level (0x12 bytes); rows of
   0..30 in each set of vp_level_param_sets (vp_level_params_get), vp_work.level_params.
   Rates are percentages tested against vp_rand() % 100 */
typedef struct vp_level_params_rec vp_level_params_rec, *Pvp_level_params_rec;

struct vp_level_params_rec {
    uchar flags;                             /* +0x0: bit 0 sidestep allowed, bit 1 guard low when not at a disadvantage (vp_defend_disadv_none), bit 2 switch guard height (vp_mode_guard_wait), bit 3 counter on the opponent's start-up (vp_intentional_counter_check) and longer guard (vp_mode_guard_start) */
    char stroke_delay;                       /* +0x1: frames before a stroke starts (plus 0..9) */
    char guard_next_frames;                  /* +0x2: frames held in vp_mode_guard_next */
    char guard_react;                        /* +0x3: guard reaction frames (plus -5..4) */
    char guard_rate;                         /* +0x4: guard against an attack seen */
    char yoke3_escapes;                      /* +0x5: escape sequences after a ynt 3 sidestep (and the vp_defend_disadv_* choice) */
    char guard2_escapes;                     /* +0x6: escape sequences for a gnt 2 guard */
    char yoke2_escapes;                      /* +0x7: escape sequences after a ynt 2 sidestep; escape inputs + 1 */
    char disadv_flip_rate;                   /* +0x8: negate the frame disadvantage (vp_mode_act_select) */
    char combo_rate;                         /* +0x9: start a combo */
    char combo_rate2;                        /* +0xa: continue a combo */
    char punish_rate;                        /* +0xb: punish after a guard */
    char field_0xc;
    char mash_rate;                          /* +0xd: mash out of a stagger or throw */
    char cut_rate;                           /* +0xe: answer an attack while guarding */
    char followup_rate;                      /* +0xf: input a hit follow-up */
    char ukemi_pick_rate;                    /* +0x10: tech roll kind pick (vp_ukemi_pick) */
    char ukemi_rate;                         /* +0x11: tech roll at all (vp_mode_yarare_down) */
};

/* readability round, agent R8 */
/* R8: types of librobdisp osage.c / osage_coli.c / rob_eye.c */
typedef struct rob_eye_rec rob_eye_rec, *Prob_eye_rec;

struct rob_eye_rec {
    int model_id;
    float offset[3];
};

typedef struct osage_coli_shape osage_coli_shape, *Posage_coli_shape;

struct osage_coli_shape {
    float pos[3];
    float radius;
    float lo;
    float hi;
};

typedef struct osage_coli_rec osage_coli_rec, *Posage_coli_rec;

struct osage_coli_rec {
    short type;
    short slot;
    struct osage_coli_shape shape;
};

typedef struct cloth_point cloth_point, *Pcloth_point;

struct cloth_point {
    float pos[3];
    float normal[3];
};

typedef struct cloth_node_rec cloth_node_rec, *Pcloth_node_rec;

struct cloth_node_rec {
    uint flags;
    float pos[3];
    float normal[3];
    float vel[3];
    float rest[3];
    float force_dir[3];
    float len_up;
    float len_down;
    float len_left;
    float len_right;
};

typedef struct cloth_bone_weight cloth_bone_weight, *Pcloth_bone_weight;

struct cloth_bone_weight {
    int slot;
    float weight;
    int bind;
    int field_0xc;
};

typedef struct cloth_anchor_rec cloth_anchor_rec, *Pcloth_anchor_rec;

struct cloth_anchor_rec {
    struct cloth_bone_weight bones[4];
    char field_0x40[64];
    float pos[3];
    float dir[3];
};

typedef struct cloth_header_rec cloth_header_rec, *Pcloth_header_rec;

struct cloth_header_rec {
    int cols;
    int rows;
    int field_0x8;
    int anchors;
    int nodes;
    int field_0x14;
    int front_map;
    int back_map;
    int bind_mtx;
};

typedef struct cloth_param_rec cloth_param_rec, *Pcloth_param_rec;

struct cloth_param_rec {
    int map_order;
    float strength;
    float air;
    float side_add;
    int field_0x10;
    struct osage_coli_rec *coli;
};

typedef struct cloth_slot_rec cloth_slot_rec, *Pcloth_slot_rec;

struct cloth_slot_rec {
    int on;
    int obj_id;
    int bone;
    struct obj_node *obj;
    float strength;
    float air;
    float side_add;
    int field_0x1c;
    struct cloth_point *points;
    int field_0x24;
    struct cloth_header_rec *header;
    struct osage_coli_rec *coli;
    void *source;
    int map_order;
};

typedef struct osage_chain_slot osage_chain_slot, *Posage_chain_slot;

struct osage_chain_slot {
    int on;
    int root_slot;
    int *cmds;
    void *nodes;
    void *params;
    int field_0x14;
    int model_id;
};

typedef struct chain_root_desc chain_root_desc, *Pchain_root_desc;

struct chain_root_desc {
    int slot;
    float offset[3];
    short rot_x;
    short rot_y;
    short rot_z;
    short rot_type;
    short limit;
    short field_0x1a;
    float weight;
};

typedef struct osage_joint_desc osage_joint_desc, *Posage_joint_desc;

struct osage_joint_desc {
    float length;
    int model_id;
    int slot;
    int param;
};

typedef struct chain_joint_desc chain_joint_desc, *Pchain_joint_desc;

struct chain_joint_desc {
    float length;
    int model_id;
    short rot_y;
    short rot_z;
    float stiffness;
    int slot;
};

typedef struct bone_chain_node bone_chain_node, *Pbone_chain_node;

struct bone_chain_node {
    int field_0x0;
    float *mtx;
    struct vec3 pos;
    struct vec3 vel;
    float m[16];
    struct bone_chain_node *link;
    float link_len;
};

typedef struct cloth_source_rec cloth_source_rec, *Pcloth_source_rec;

struct cloth_source_rec {
    int obj_id;
    int bone;
    float strength;
    float air;
    float side_add;
    struct osage_coli_rec *coli;
};

/* readability round, agent R9 */
/* R9: one of a rob's four item slots (rob +0x56C points at four; rob_item_slots, rob_items_setup) */
typedef struct rob_item_slot rob_item_slot, *Prob_item_slot;
struct rob_item_slot {
    uchar flags;        /* +0x0: bit 0 the costume's own item (kind / data), bit 1 a card item (card_kind / card_item / card_data) */
    uchar kind;         /* +0x1: draw kind of the costume item (rob_item_draw: 0 object, 1 bone model) */
    char card_kind;     /* +0x2: draw kind of the card item */
    uchar card_item;    /* +0x3: card item id */
    int data;           /* +0x4: the costume item's record */
    int card_data;      /* +0x8: the card item's record */
};

/* R9: a character / costume's color table record, [chara][costume] at 0x0C1DF904 (color_table_list_get, custom_color_list_get) */
typedef struct chara_color_table_rec chara_color_table_rec, *Pchara_color_table_rec;

struct chara_color_table_rec {
    int rows;           /* +0x0: rows in table */
    undefined4 *table;  /* +0x4: rows of 8 color list pointers, one per column */
    int **custom;       /* +0x8: per part (0-4) 19 colors x 4 slots of color list pointers */
};

/* R9: a 12-byte entry of the second costume color list (texcol_scratch_b; costume_color_lists_copy copies them, texcol_lists_combine matches them to the 16-byte entries by id); -1 id ends a list */
typedef struct texcol_xy_entry texcol_xy_entry, *Ptexcol_xy_entry;

struct texcol_xy_entry {
    int id;             /* +0x0 */
    uchar x[4];         /* +0x4: copied to the output entry's +0x8 */
    uchar y[4];         /* +0x8: copied to the output entry's +0xC */
};

/* R9: a moving item's definition (item_anim_defs rows point at them; item_anim_sway, item_anim_follow, item_anim_aim) */
typedef struct item_anim_def item_anim_def, *Pitem_anim_def;

struct item_anim_def {
    uchar flags;            /* +0x00: bit 0 sways in place (item_anim_sway), else follows (item_anim_follow) */
    char field_0x1[3];
    int mtx_slot;           /* +0x04: the rob matrix (rob +0x54) the item's matrix is written to */
    int base_slot;          /* +0x08: the rob matrix it follows / turns from */
    short circle_step;      /* +0x0C: circle_phase step a frame */
    short bob_step;         /* +0x0E: bob_phase step a frame */
    float rate_xz;          /* +0x10: easing toward the target in x / z */
    float rate_y;           /* +0x14: easing toward the target in y */
    float circle_radius;    /* +0x18 */
    float bob_height;       /* +0x1C: bob amplitude; half of it also lowers the minimum height */
    float height;           /* +0x20: height above the followed matrix */
    float floor_height;     /* +0x24: minimum height above the stage center (stage_cur->center.y) */
    float scale;            /* +0x28 */
    short yaw_amp;          /* +0x2C: yaw sway amplitude (0: face the aim) */
    short yaw_bias;         /* +0x2E */
    short tilt_x_amp;       /* +0x30 */
    short tilt_x_bias;      /* +0x32 */
    short tilt_z_amp;       /* +0x34 */
    short tilt_z_bias;      /* +0x36 */
    short aim_step;         /* +0x38: 0 no aiming, else the most pitch / yaw turns a frame */
};

/* R9: a moving item record, four per side in item_anims (0x60 bytes; item_anim_reset, item_anim_slot_add, item_anim_follow) */
typedef struct item_anim_rec item_anim_rec, *Pitem_anim_rec;

struct item_anim_rec {
    struct item_anim_def *def;  /* +0x00: 0 when unused */
    uchar item;                 /* +0x04: card item id */
    char field_0x5;
    short circle_phase;         /* +0x06 */
    short bob_phase;            /* +0x08 */
    short tilt_phase;           /* +0x0A */
    short yaw;                  /* +0x0C */
    short yaw_phase;            /* +0x0E */
    float pos[3];               /* +0x10: item_anim_reset puts y at 50.0 */
    float tilt_mtx[16];         /* +0x1C */
    short aim_pitch;            /* +0x5C */
    short aim_yaw;              /* +0x5E */
};

/* R9: a side's card mark record, two in card_marks (0x9C bytes; card_mark_reset, card_mark_update, card_mark_draw) */
typedef struct card_mark_rec card_mark_rec, *Pcard_mark_rec;

struct card_mark_rec {
    char mark;                  /* +0x00: value 0-14, -1 none (card_mark_models index) */
    char field_0x1;
    short circle_phase;         /* +0x02 */
    short bob_phase;            /* +0x04 */
    short sway_phase;           /* +0x06 */
    short field_0x8;            /* +0x08: a random phase card_mark_reset sets */
    short spin;                 /* +0x0A */
    short spin_phase;           /* +0x0C */
    short field_0xe;
    float pos[3];               /* +0x10 */
    float sway_mtx[16];         /* +0x1C */
    char field_0x5c[0x40];
};

/* readability round, agent F3 */
typedef struct hit_damage_row_rec {
    char motion;
    char shake;
    char field_0x2[2];
    uint result_flags;
    uint light_flags;
    uint sound_mask;
    float damage_scale;
} hit_damage_row_rec;

typedef struct throw_pick_rec {
    uint mask;
    uint event;
    int code;
    char move_idx;
    char field_0xd[3];
} throw_pick_rec;

typedef struct move_cmd49_arg_rec {
    uint mask;
    char damage_min;
    char damage_limit;
    char damage_percent;
    char no_launch;
    char slow_frames;
    char light_color;
    char sound_kind;
    char field_0xb;
    int sound_id;
    float slow_rate;
} move_cmd49_arg_rec;

typedef struct move_event39_rec {
    uint mask;
    ushort motion[2];
    short mode;
    ushort move;
} move_event39_rec;

/* readability round, agent F4 */
typedef struct vp_stroke_rec vp_stroke_rec, *Pvp_stroke_rec;

struct vp_stroke_rec {
    char after_input;
    char attacks;
    char field_0x2[2];
    short *seq;
};

typedef struct vp_combo_entry_rec vp_combo_entry_rec, *Pvp_combo_entry_rec;

struct vp_combo_entry_rec {
    ushort flags;
    char field_0x2[2];
    uint chara_mask;
    struct vp_stroke_rec **strokes;
};

typedef struct vp_action_rec vp_action_rec, *Pvp_action_rec;

struct vp_action_rec {
    uint flags;
    short drank;
    short frames;
    struct vp_stroke_rec *stroke;
    struct vp_combo_entry_rec *combos;
};

typedef struct vp_rate_action_rec vp_rate_action_rec, *Pvp_rate_action_rec;

struct vp_rate_action_rec {
    short rate;
    char field_0x2[2];
    int action;
};

typedef struct vp_rank_action_rec vp_rank_action_rec, *Pvp_rank_action_rec;

struct vp_rank_action_rec {
    short rank;
    char field_0x2[2];
    struct vp_action_rec *action;
};

typedef struct vp_chara_actions_rec vp_chara_actions_rec, *Pvp_chara_actions_rec;

struct vp_chara_actions_rec {
    struct vp_rank_action_rec **attack_lists;
    int *escape_seqs;
    short *escape_inputs;
    struct vp_action_rec **punish_lists;
    struct vp_rate_action_rec *close_attacks;
    struct vp_rate_action_rec *close_attacks_vs_crouch;
    struct vp_action_rec *punish_vs_crouch;
    int drink_attack;
    char common_attack_rate;
    char field_0x21[3];
};

typedef struct vp_mode_history_rec vp_mode_history_rec, *Pvp_mode_history_rec;

struct vp_mode_history_rec {
    short frames;
    char field_0x2[2];
    int mode;
};

typedef struct vp_renkei_work_rec vp_renkei_work_rec, *Pvp_renkei_work_rec;

struct vp_renkei_work_rec {
    struct vp_combo_entry_rec combos[2];
    struct vp_action_rec combo_action;
    struct vp_action_rec step_action;
};

typedef struct vp_renkei_step_rec vp_renkei_step_rec, *Pvp_renkei_step_rec;

struct vp_renkei_step_rec {
    struct vp_stroke_rec *stroke;
    uint next_cond;
};

typedef struct vp_renkei_combo_rec vp_renkei_combo_rec, *Pvp_renkei_combo_rec;

struct vp_renkei_combo_rec {
    char attack_rate;
    char stance;
    char field_0x2[2];
    struct vp_stroke_rec *stroke;
    int kind;
    uint chara_mask;
    struct vp_stroke_rec **strokes;
};

typedef struct vp_renkei_rec vp_renkei_rec, *Pvp_renkei_rec;

struct vp_renkei_rec {
    char attack_rate;
    char stance;
    char field_0x2[2];
    struct vp_renkei_step_rec steps[3];
    struct vp_renkei_combo_rec combo;
};


typedef struct enemy_script_step_rec enemy_script_step_rec, *Penemy_script_step_rec;

struct enemy_script_step_rec {
    uint input;
    int frames;
};

typedef struct enemy_ai_attack_rec enemy_ai_attack_rec, *Penemy_ai_attack_rec;

struct enemy_ai_attack_rec {
    ushort move;
    char field_0x2[2];
    struct enemy_script_step_rec *script;
    int kind;
};

typedef struct enemy_record_rec enemy_record_rec, *Penemy_record_rec;

struct enemy_record_rec {
    char field_0x0[8];
    float dist;
    short active_end;
    short recovery_end;
    short active_start;
    short attack_damage;
    short field_0x14;
    short field_0x16;
    short field_0x18;
    char field_0x1a;
    char field_0x1b;
    char marker[4];
};

typedef struct enemy_move_log_rec enemy_move_log_rec, *Penemy_move_log_rec;

struct enemy_move_log_rec {
    ushort move;
    char field_0x2[2];
    float dist;
};

typedef struct enemy_level_params_rec enemy_level_params_rec, *Penemy_level_params_rec;

struct enemy_level_params_rec {
    short attack_interval;
    short throw_interval;
    short guard_interval;
    uchar attack_rate;
    uchar throw_rate;
    uchar guard_rate;
    uchar sidestep_rate;
    uchar reversal_rate;
    uchar sidestep_count;
    uchar mash_rate;
    uchar recovery_rate;
};

typedef struct enemy_work_rec enemy_work_rec, *Penemy_work_rec;

struct enemy_work_rec {
    char field_0x0[4];
    uint situation;
    uint situation_prev;
    int state;
    int attack_mode;
    uint script_flags;
    uint *script_tag;
    uint *script;
    uint *script_start;
    short field_0x24;
    char field_0x26[2];
    short timer_0x28;
    short recover_frames;
    short land_frames;
    short start_wait;
    short timer_0x30;
    short attack_cooldown;
    short throw_cooldown;
    short guard_cooldown;
    short attack_interval;
    short throw_interval;
    short guard_interval;
    char field_0x3e[2];
    struct enemy_move_log_rec attacks[16];
    struct enemy_move_log_rec repeated_attacks[8];
    ushort throws[16];
    ushort repeated_throws[8];
    int step;
    uint output_input;
    int entry_index;
    int frames_left;
    char level;
    uchar base_attack_rate;
    uchar base_throw_rate;
    uchar base_guard_rate;
    uchar base_sidestep_rate;
    uchar base_reversal_rate;
    uchar base_sidestep_count;
    uchar base_mash_rate;
    uchar base_recovery_rate;
    uchar attack_rate;
    uchar throw_rate;
    uchar guard_rate;
    uchar sidestep_rate;
    uchar reversal_rate;
    uchar sidestep_count;
    uchar mash_rate;
    uchar recovery_rate;
    char field_0x151[3];
};

/* readability round, agent F5 */
typedef struct morph_anim_key morph_anim_key, *Pmorph_anim_key;

struct morph_anim_key {
    float time;           /* +0x00 */
    float tangent_in;     /* +0x04 */
    float tangent_out;    /* +0x08 */
    float value;          /* +0x0c */
};

typedef struct morph_anim_rec morph_anim_rec, *Pmorph_anim_rec;

struct morph_anim_rec {
    int key_count;        /* +0x00 */
    int obj;              /* +0x04 */
    float speed;          /* +0x08 */
    float time;           /* +0x0c */
    morph_anim_key *keys; /* +0x10 */
};

typedef struct morph_anim_slot morph_anim_slot, *Pmorph_anim_slot;

struct morph_anim_slot {
    int frames;           /* +0x00 */
    float scale;          /* +0x04 */
    morph_anim_rec *anim; /* +0x08 */
};

typedef struct osage_coli_edit_rec osage_coli_edit_rec, *Posage_coli_edit_rec;

struct osage_coli_edit_rec {
    int on;                       /* +0x00 */
    struct osage_coli_rec recs[8]; /* +0x04 */
};

typedef void (*obj_draw_handler)(uint obj);

typedef struct rob_zanzou_hist_rec rob_zanzou_hist_rec, *Prob_zanzou_hist_rec;

struct rob_zanzou_hist_rec {
    struct vec3 edge_a[4];        /* +0x00 */
    struct vec3 edge_b[4];        /* +0x30 */
};

typedef struct item_zanzou_hist_rec item_zanzou_hist_rec, *Pitem_zanzou_hist_rec;

struct item_zanzou_hist_rec {
    struct vec3 pos[3];           /* +0x00 */
};

/* readability round, agent F6 */
typedef struct hair_color_row hair_color_row, *Phair_color_row;

struct hair_color_row {
    uchar item;
    char column;
    char row;
    char buffer1_row;
    char column2;
    char row2;
};

typedef struct hair_face_color_row hair_face_color_row, *Phair_face_color_row;

struct hair_face_color_row {
    uchar item;
    char column;
    char row;
};

typedef struct hair_item_pair_color_row hair_item_pair_color_row, *Phair_item_pair_color_row;

struct hair_item_pair_color_row {
    uchar head_item;
    uchar face_item;
    char pair[3][2];
};


typedef struct item_anim_entry item_anim_entry, *Pitem_anim_entry;

struct item_anim_entry {
    int chara;
    char costume_mask;
    uchar item;
    char field_0x6[2];
    struct item_anim_def *def;
};


typedef struct chara_item_entry chara_item_entry, *Pchara_item_entry;

struct chara_item_entry {
    char slot;
    char kind[2];
    char field_0x3;
    int data[2];
};

typedef struct chara_item_row chara_item_row, *Pchara_item_row;

struct chara_item_row {
    short count;
    short field_0x2;
    char *entries;
    char *own_items[2];
};


typedef struct mot_slot_rec mot_slot_rec, *Pmot_slot_rec;

struct mot_slot_rec {
    struct static_heap_rec *mot_heap;
    struct static_heap_rec *smooth_heap;
    struct static_heap_rec *mothead_heap;
    short first;
    short end;
    uchar bank;
    char mothead_pending;
    char field_0x12[2];
};


typedef struct stage_placed_obj stage_placed_obj, *Pstage_placed_obj;

struct stage_placed_obj {
    uint flags;
    float pos[3];
    uint obj;
};

/* readability round, agent M1 */
typedef struct mission_rec mission_rec, *Pmission_rec;

struct mission_rec {
    char *name;
    int value;
    char *suffix;
    uint flags;
    void (*check)(int);
    int reward;
};

/* readability round, agent M2 */
typedef struct msn_text_layout_rec msn_text_layout_rec, *Pmsn_text_layout_rec;

struct msn_text_layout_rec {
    uint align;
    char measuring;
    char field_0x5[3];
    float line_count;
    float width_max;
    float offset_x;
    float offset_y;
};

typedef struct msn_text_state_rec msn_text_state_rec, *Pmsn_text_state_rec;

struct msn_text_state_rec {
    int color;
    uint custom_colors[4];
    uint custom_colors2[4];
    char mirror;
    char field_0x25[3];
    float alpha;
    struct msn_text_layout_rec layout;
};

typedef struct msnm_match_settings_rec msnm_match_settings_rec, *Pmsnm_match_settings_rec;

struct msnm_match_settings_rec {
    int energy;
    int round_time;
    int rounds_to_win;
};

typedef struct msnm_try_rec msnm_try_rec, *Pmsnm_try_rec;

struct msnm_try_rec {
    int type;
    int target_count;
    int param;
    struct msnm_match_settings_rec match;
    int cpu_level;
    int opponent;
    int stage;
    int score_kind;
    int grade4_cond;
    int grade3_cond;
    int grade1_cond;
    int field_0x34;
    int field_0x38;
    int special;    /* +0x3c: the mission's special text, -1 none (msn_text_build) */
};

typedef struct msnm_try_setting_rec msnm_try_setting_rec, *Pmsnm_try_setting_rec;

struct msnm_try_setting_rec {
    int stage;
    int opponent;
    int variant;
    int hyper_kinds[4];
    int hyper_entries[4];
};

typedef struct msnm_try_type_rec msnm_try_type_rec, *Pmsnm_try_type_rec;

struct msnm_try_type_rec {
    int (*frame)(struct rob_rec *rob, struct rob_rec *opp, struct msnm_try_rec *try_rec, struct msnm_progress_rec *progress);
    int (*round_end)(struct rob_rec *rob, struct rob_rec *opp, struct msnm_try_rec *try_rec, struct msnm_progress_rec *progress);
    uint stage_mask;
    uint opponent_mask;
    int value;
    int rule;
};

typedef struct msnm_combo_variant_rec msnm_combo_variant_rec, *Pmsnm_combo_variant_rec;

struct msnm_combo_variant_rec {
    uint opponent_mask;
    uint flags;
    int row_kinds[6];
};

typedef struct msnm_progress_rec msnm_progress_rec, *Pmsnm_progress_rec;

struct msnm_progress_rec {
    int cnt_num;
    int win;
    int win_streak;
    int score;
    uchar flag;
    char field_0x11[3];
    int count;
    int mode;
    uchar round_flag;
    char field_0x1d[3];
    int round_cnt;
    int nuke_dir_flags;
    int field_0x28;
};

typedef struct msnm_cmd_entry msnm_cmd_entry, *Pmsnm_cmd_entry;

struct msnm_cmd_entry {
    int mask;
    ushort move;
    char field_0x6[2];
};

typedef struct msnm_try_disp_rec msnm_try_disp_rec, *Pmsnm_try_disp_rec;

struct msnm_try_disp_rec {
    short field_0x0;
    short moves[10];
    short field_0x16;
    int variant;
    uchar *alias_table;
    int row_kinds[6];
    struct msnm_cmd_entry rows[6][21];
    int row_steps[6];
    short target_moves[3];
    short first_move;
    uchar *pattern;
    char field_0x44c[4];
};

typedef struct msnm_course_results_rec msnm_course_results_rec, *Pmsnm_course_results_rec;

struct msnm_course_results_rec {
    int columns[8];
    int kinds[8];
    int scores[8];
    int grades[8];
    int times[8];
};

typedef struct msnm_quit_rec msnm_quit_rec, *Pmsnm_quit_rec;

struct msnm_quit_rec {
    int choice;
    int timer;
    int hold;
    uchar chosen;
    uchar open;
    char field_0xe[2];
};

typedef struct msnm_input_stats_rec msnm_input_stats_rec, *Pmsnm_input_stats_rec;

struct msnm_input_stats_rec {
    int phase;
    char release_found;
    char field_0x5;
    char field_0x6[2];
    int lever_dirs;
    uint lever_dir_mask;
    int tally[7];
};

typedef struct msnm_combo_rec msnm_combo_rec, *Pmsnm_combo_rec;

struct msnm_combo_rec {
    char new_move;
    char cut_short;
    uchar broken;
    char field_0x3;
    int count;
    ushort moves[100];
    int leave_frame;
    int damage_sum;
    int flag4000_moves;
};

typedef struct msnm_score_rec msnm_score_rec, *Pmsnm_score_rec;

struct msnm_score_rec {
    char field_0x0[36];
    int cnt_num;
    int field_0x28;
    int field_0x2c;
    int base;
    int win_time;
    int time_rate;
    int field_0x3c;
    int time_score;
    int field_0x44;
    int bonus;
    int field_0x4c;
    int total;
    int total_step;
};

typedef struct chlg_tally_rec chlg_tally_rec;

struct chlg_tally_rec {
    int value;
    int target;
};

typedef struct chlg_result_rec chlg_result_rec;

struct chlg_result_rec {
    int state;
    int timer;
    int mode;
    int bar_state;
    int bar_count;
    int field_0x14[6];
    struct chlg_tally_rec tally[5];
    int step;
    int coin_layers[10];
    struct mission_score_rec old_hiscore;
    char new_record;
    char field_0xb9[3];
    struct mission_score_rec player;
};

typedef union msnm_result_area msnm_result_area, *Pmsnm_result_area;

union msnm_result_area {
    struct msnm_score_rec score_parts;
    struct chlg_result_rec chlg;
};

typedef struct msnm_work_rec msnm_work_rec, *Pmsnm_work_rec;

struct msnm_work_rec {
    uchar active;
    uchar side;
    uchar loaded;
    uchar scene0_loaded;
    uchar saved_side;
    uchar saved_active;
    char field_0x6[2];
    int saved_stage;
    int saved_opponent;
    int stage;
    int opponent;
    int status;
    struct msnm_try_rec cur_try;
    struct msnm_try_setting_rec setting;
    struct msnm_progress_rec progress;
    struct msnm_try_disp_rec disp;
    int round_count;
    int wins_ko;
    int wins_perfect;
    int wins_ring_out;
    int wins_time_up;
    int round_times[2];
    int round_lives[2];
    int round_opp_lives[2];
    int round_results[2];
    int score;
    int grade;
    int grade_average;
    int rank;
    int points;
    int time_total;
    int reward;
    uchar item_given;
    char field_0x555[3];
    int item;
    char text[256];
    int column;
    int row;
    struct msnm_course_results_rec course;
    char field_0x704[8];
    struct msnm_try_rec next_tries[2];
    struct msnm_try_setting_rec next_settings[2];
    char field_0x7e4[16];
    char join_side;
    char field_0x7f5[3];
    int result_kind;
    int state;
    int choice;
    int input_wait_move;
    int input_wait;
    int menu_timer;
    uchar route_choosing;
    char field_0x811[3];
    struct msnm_quit_rec quit;
    struct msnm_input_stats_rec input_stats[2];
    struct msnm_combo_rec combos[2];
    union msnm_result_area result;
    float stage_dim_level;
};

typedef struct msnm_rule_params_rec msnm_rule_params_rec, *Pmsnm_rule_params_rec;

struct msnm_rule_params_rec {
    int field_0x0;
    float field_0x4;
    float field_0x8;
    float field_0xc;
    float field_0x10;
    float field_0x14;
    float field_0x18;
    float field_0x1c;
    float rule6_life_rate;
    char field_0x24[6];
    short rule5_damage;
    short rule1_damage;
    char field_0x2e[2];
    float rule2_scale;
    short rule2_add;
    char field_0x36[2];
};

typedef struct msnm_score_rates_rec msnm_score_rates_rec, *Pmsnm_score_rates_rec;

struct msnm_score_rates_rec {
    float time_kind_rate;
    float count_kind_rate;
    float time_rate;
    float ko_bonus;
    float perfect_bonus;
    float even_time_bonus;
    float low_life_bonus;
};

typedef struct msnm_score_table_rec msnm_score_table_rec, *Pmsnm_score_table_rec;

struct msnm_score_table_rec {
    float base[5];
    struct msnm_score_rates_rec rates[3];
};

/* readability round, agent M4 */
typedef struct msn_count_place_rec msn_count_place_rec;

struct msn_count_place_rec {
    char by_side;
    char field_0x1[3];
    float pos[2][2];
};


typedef struct chlg_chara_icon_rec chlg_chara_icon_rec;

struct chlg_chara_icon_rec {
    int field_0x0;
    int icon;
};





typedef struct chlg_layer_desc chlg_layer_desc;

struct chlg_layer_desc {
    char *layer;
    int flags;
    float x;
    float y;
    float depth;
    float alpha;
    uint color;
};

/* readability round, agent C1 */
typedef struct card_md5_rec card_md5_rec, *Pcard_md5_rec;

struct card_md5_rec {
    uint state[4];
    uint bit_count[2];
    uchar block[64];
};

/* readability round, agent C3 */
typedef struct nickname_result_rec nickname_result_rec, *Pnickname_result_rec;

struct nickname_result_rec {
    char god;
    uchar level[8];
    uchar pt_diff[8][3];
};

typedef struct nickname_record_rec nickname_record_rec, *Pnickname_record_rec;

struct nickname_record_rec {
    uchar games;
    char disp_type;
    uchar disp_name;
    uchar level[8];
    uchar card_mark[8];
    char levelup[8];
    uchar pt_diff[8][3];
    char field_0x33[1];
    ushort pt[8][3];
    uint flags;
    struct nickname_result_rec result[2];
    char field_0xaa[2];
};

typedef struct nickname_recv_rec nickname_recv_rec, *Pnickname_recv_rec;

struct nickname_recv_rec {
    uchar games;
    char disp_type;
    uchar disp_name;
    char god;
    uchar level[8];
    ushort pt[8][3];
};

/* readability round, agent C4 */
typedef struct card_mode_slot card_mode_slot, *Pcard_mode_slot;

struct card_mode_slot {
    char mode;
    char step;
    char sub_step;
    char retries;
};

typedef struct card_message_rec card_message_rec, *Pcard_message_rec;

struct card_message_rec {
    char id;
    char field_0x1;
    short frames;
    int word;
};

typedef struct card_renew_state_rec card_renew_state_rec, *Pcard_renew_state_rec;

struct card_renew_state_rec {
    uint result_bits;
    int cancel;
    int done;
    ushort new_play_id;
    ushort old_play_id;
};

typedef struct card_renew_text_rec card_renew_text_rec, *Pcard_renew_text_rec;

struct card_renew_text_rec {
    int frames;
    int countdown;
    int flag;
    int text_no;
};

typedef struct card_renew_times_rec card_renew_times_rec, *Pcard_renew_times_rec;

struct card_renew_times_rec {
    int old_access_key;
    int new_access_key;
    int outcome;
};

/* readability round, agent E1 */
typedef struct aquarium_bubble {
    uchar flags;
    char field_0x1;
    short frames;
    short frame;
    short turn;
    struct vec3 pos;
} aquarium_bubble;

typedef struct are_billboard {
    short frame;
    short field_0x2;
    struct vec3 pos;
} are_billboard;

typedef struct ban_flag {
    short turn;
    short field_0x2;
    float scale;
    struct vec3 pos;
} ban_flag;

typedef struct are_fan {
    struct vec3 pos;
    short angle;
    short spin;
    short spin_set;
    char field_0x12[6];
} are_fan;

typedef struct oneshot_anim {
    int *ids;
    struct vec3 offset;
    short rot[3];
    short field_0x16;
    uint flags;
    float scale;
    struct vec3 pos;
    short turn;
    short field_0x2e;
} oneshot_anim;

typedef struct audience_place {
    short angle;
    short field_0x2;
    struct vec3 pos;
} audience_place;

typedef struct audience_react_list {
    char count;
    char field_0x1[3];
    char *sets;
} audience_react_list;

typedef struct audience_member {
    char type;
    char reaction;
    char set;
    char frame;
    float frame_pos;
    float rate_ofs;
    float scale;
    audience_anim_set *anim;
    audience_place *place;
    audience_react_list *up_list;
    audience_react_list *down_list;
} audience_member;

typedef struct bird_model_set {
    uchar flags;
    char field_0x1[3];
    int first_obj;
    int *models;
    int *clump4_models;
    int *clump8_models;
    short frames;
    short field_0x16;
    uint clump4_x_range;
    uint clump8_x_range;
    uint clump4_y_range;
    uint clump8_y_range;
} bird_model_set;

typedef struct bird_flock_prm {
    short set;
    short field_0x2;
    int yaw_range;
    int pitch_range;
    float turn_max;
    float turn_min;
    float turn_step;
    float speed;
    float field_0x1c;
} bird_flock_prm;

typedef struct bird_flock_def {
    short count[3];
    short field_0x6;
    int yaw_range;
    int pitch_range;
    float *box;
    bird_flock_prm *prm;
} bird_flock_def;

typedef struct bird_flock_member {
    uchar flags;
    char field_0x1;
    short frame;
    short frames;
    short wait;
    short yaw;
    short pitch;
    float frame_pos;
    float rate;
    float speed;
    float turn;
    int *models;
    float *box;
    bird_flock_prm *prm;
    struct vec3 pos;
    struct vec3 dir;
    struct vec3 want;
} bird_flock_member;

typedef struct bird_path_step {
    short op;
    short yaw_rate;
    short pitch_rate;
    short field_0x6;
    float speed;
    float frame_rate;
    float goal;
    short goal_yaw;
    short field_0x16;
} bird_path_step;

typedef struct bird_path_def {
    short set;
    short count[3];
    struct vec3 pos;
    short spread[3];
    short yaw;
    short pitch;
    short field_0x1e;
    float y_min;
    float y_max;
    short push;
    short field_0x2a;
    bird_path_step *script;
    bird_path_step *script_loop;
} bird_path_def;

typedef struct bird_path_member {
    uchar flags;
    char field_0x1;
    short frame;
    short frames;
    short yaw;
    short prev_yaw;
    short pitch;
    short step_frame;
    short field_0xe;
    float frame_pos;
    float rate;
    float goal;
    bird_path_step *step;
    bird_path_step *script_loop;
    int *models;
    float y_min;
    float y_max;
    struct vec3 pos;
    struct vec3 spread;
} bird_path_member;

typedef struct fish_school_prm {
    short set;
    short yaw_range;
    short pitch_range;
    short field_0x6;
    float turn;
    float speed;
    float slow_step;
    float scatter_turn;
    float dash_max;
    float dash_step;
} fish_school_prm;

typedef struct fish_school_def {
    short count;
    short field_0x2;
    int yaw_range;
    int pitch_range;
    struct vec3 pos;
    short spread[3];
    short field_0x1e;
    float *box;
    fish_school_prm *prm;
} fish_school_def;

typedef struct fish_school_member {
    uchar flags;
    char state;
    char turn_frames;
    char field_0x3;
    short frame;
    short frames;
    short wait;
    short yaw;
    short pitch;
    short field_0xe;
    float speed;
    float turn;
    int *models;
    float *box;
    fish_school_prm *prm;
    struct vec3 pos;
    struct vec3 dir;
    struct vec3 want;
    struct vec3 step;
} fish_school_member;

typedef struct fish_path_def {
    short set;
    short count[3];
    struct vec3 pos;
    short spread[3];
    short yaw;
    short pitch;
    short pitch_spread;
    float speed;
    float lo;
    float hi;
    short max_turn;
    short push;
    struct fish_path_seg *script;
} fish_path_def;

/* readability round, agent E2 */
typedef struct stage_screen_tex_rec stage_screen_tex_rec, *Pstage_screen_tex_rec;

struct stage_screen_tex_rec {
    uint flags;
    short target;
    short tex_index;
    short width;
    short height;
    uint format;
};

typedef struct stage_screen_rec stage_screen_rec, *Pstage_screen_rec;

struct stage_screen_rec {
    uint flags;
    struct stage_screen_tex_rec tex;
    void (*view)(struct stage_screen_rec *);
    void (*after)(struct stage_screen_rec *);
    float rect[4];
    struct stage_rec *stage;
    struct rob_rec *robs[2];
    void (*callbacks[3])(void *);
};

typedef struct stage_screen_slot stage_screen_slot, *Pstage_screen_slot;

struct stage_screen_slot {
    uint field_0x0;
    struct stage_screen_rec screen;
};

typedef struct stage_screen_desc stage_screen_desc, *Pstage_screen_desc;

struct stage_screen_desc {
    uint flags;
    struct stage_screen_tex_rec *tex;
    void (*view)(struct stage_screen_rec *);
    void (*after)(struct stage_screen_rec *);
    void (*callbacks[3])(void *);
};

typedef struct stage_obj_place stage_obj_place, *Pstage_obj_place;

struct stage_obj_place {
    uint flags;
    struct vec3 pos;
    int obj;
};

typedef struct stage_screen_light_src stage_screen_light_src, *Pstage_screen_light_src;

struct stage_screen_light_src {
    int id;
    float dir[3];
    float value[4];
};

typedef struct water_grid_vtx water_grid_vtx, *Pwater_grid_vtx;

struct water_grid_vtx {
    float h;
    struct vec3 n;
    float u;
    float v;
    uint color;
};

typedef struct rain_particle_rec rain_particle_rec, *Prain_particle_rec;

struct rain_particle_rec {
    struct vec3 pos;
    struct vec3 vel;
    int life;
    float frame;
    float frame_step;
    int active;
    int field_0x28;
    int kind;
    float spawn_speed;
};

typedef struct rain_emitter_rec rain_emitter_rec, *Prain_emitter_rec;

struct rain_emitter_rec {
    struct vec3 pos;
    struct vec3 vel;
    int state;
    int count;
    int phase;
    int particle_count;
    struct rain_particle_rec particles[30];
};

typedef struct rain_drip_set rain_drip_set, *Prain_drip_set;

struct rain_drip_set {
    struct rob_rec *rob;
    struct rain_emitter_rec limbs[2];
};

typedef struct rain_splash_set rain_splash_set, *Prain_splash_set;

struct rain_splash_set {
    struct rob_rec *rob;
    struct rain_emitter_rec limbs[2];
    struct rain_emitter_rec spheres[10];
};

typedef struct rain_psys_particle rain_psys_particle, *Prain_psys_particle;

struct rain_psys_particle {
    struct vec3 pos;
    struct vec3 vel;
    float field_0x18;
    float gravity;
    char field_0x20[68];
    int field_0x64;
    int field_0x68;
    float field_0x6c;
    struct rain_psys_rec *owner;
};

typedef struct rain_psys_rec rain_psys_rec, *Prain_psys_rec;

struct rain_psys_rec {
    struct vec3 origin;
    struct vec3 vel_scale;
    char field_0x18[64];
    int frame;
    int mode;
    int field_0x60;
    struct rain_psys_particle particles[150];
    void (*seed)(struct rain_psys_particle *);
};

typedef struct rain_fan_rec rain_fan_rec, *Prain_fan_rec;

struct rain_fan_rec {
    int field_0x0;
    uchar count;
    char field_0x5[3];
    int frames[6][30];
    struct compiled_model_rec *models[30];
    float *field_0x350;
};

typedef struct sand_cell_rec sand_cell_rec, *Psand_cell_rec;

struct sand_cell_rec {
    struct vec3 pos;
    float base_y;
    int press_frame;
    int field_0x14;
    short flags;
    short field_0x1a;
    int vtx[2];
};

typedef struct sand_joint_rec sand_joint_rec, *Psand_joint_rec;

struct sand_joint_rec {
    int bone;
    int bone2;
    float radius;
    float y_offset;
    char kind;
    char field_0x11[3];
};

typedef struct sand_grain_rec {
    float pos[3];
    float vel[3];             /* +0x0c */
    float size;               /* +0x18 */
    short spin_x;             /* +0x1c */
    unsigned short spin_x_step;
    short spin_y;             /* +0x20 */
    unsigned short spin_y_step;
} sand_grain_rec;

typedef struct sand_body_rec sand_body_rec, *Psand_body_rec;

struct sand_body_rec {
    struct vec3 pos;
    struct vec3 dir;
    struct vec3 prev_dir;
    float hlen;
    float len;
    float travel;
    float ratio;
    float radius;
    float pressed;
    float puff_amount;
    int state_frames;
    uint heading;
    uint sector;
    uint flags;
    uchar state;
    uchar kind;
    char field_0x52[2];
    struct vec3 start_pos;
    struct vec3 move;
    float move_len;
    float start_pressed;
    float move_ratio;
    float curve;
    float puff_power;
    float field_0x80;
    float leave_power;
};

typedef struct sand_body_table sand_body_table, *Psand_body_table;

struct sand_body_table {
    int count;
    struct sand_body_rec bodies[10];
};

typedef struct sand_puff_rec sand_puff_rec, *Psand_puff_rec;

struct sand_puff_rec {
    struct vec3 pos;
    struct vec3 vel;
    float gravity;
    float cloud;
    uchar kind;
    char field_0x21;
    short life;
    int spin;
};

/* readability round, agent E3 */
typedef struct lens_flare_element lens_flare_element, *Plens_flare_element;

struct lens_flare_element {
    int model;
    float fraction;
    float scale;
};

typedef struct stage_anim_rec stage_anim_rec, *Pstage_anim_rec;

/* One of the four animated stage objects (stage_anim_objs, 0x30 bytes each) of the
 * fence and break effects; objects 2 and 3 (stage_anim_obj2) are the ones ROME's
 * break and the breaking floor's tile slots use. */
struct stage_anim_rec {
    bone_work_rec *bones;     /* +0x0: 0xDEC-byte buffer, the 33 bone records rob_bones_init sets up */
    mot_channel *channels;    /* +0x4: 0x1800 bytes, mot_channel_pool_set */
    int *links;               /* +0x8: 0x80 bytes, rob_bone_links_pool_set */
    float *mtx;               /* +0xc: the piece matrices (0x840-byte block) */
    int *models;              /* +0x10: the break source's model list (rome_break_obj_start) */
    short flags;              /* +0x14: 1 running, 2 shown, 4 done, 8 keep shown, 0x20 spawn, 0x40 may spawn */
    short turn;               /* +0x16: y angle */
    float frame_time;         /* +0x18: frames; pose at time / 30 */
    short anim_id;            /* +0x1c: mot_bind id */
    short anim_frames;        /* +0x1e: the bound motion's frame count (stage_anim_obj_bind) */
    short frame_count;        /* +0x20: frames run */
    uchar source;             /* +0x22: the break source index (rome_break_obj_start) */
    char field_0x23;
    float pos[3];             /* +0x24: position */
};

typedef struct rome_break_src_rec rome_break_src_rec, *Prome_break_src_rec;

struct rome_break_src_rec {
    short anim_id;
    char field_0x2[2];
    int *models;
    uint done_mask;
    float pos[3];
    uint flags;
};

typedef struct sky_layer_rec sky_layer_rec, *Psky_layer_rec;

struct sky_layer_rec {
    uchar flags;
    char field_0x1;
    short start;
    short frames;
    char field_0x6[2];
    int model;
    short rot[3];
    char field_0x12[2];
    float scale[3];
    float pos[3];
};

typedef struct sky_obj_rec sky_obj_rec, *Psky_obj_rec;

struct sky_obj_rec {
    uchar flags;
    char field_0x1;
    short frame;
    short frames;
    char field_0x6[2];
    uint mask;
    int model;
    int model_done;
    float time;
    float time_prev;
    float field_0x1c;
    float field_0x20;
    float field_0x24;
    float *light_pos;
    int *seq_a;
    int *seq_b;
    struct sky_layer_rec **layers;
    short **sounds;
};

typedef struct sky_obj_def_rec sky_obj_def_rec, *Psky_obj_def_rec;

struct sky_obj_def_rec {
    short frames;
    char field_0x2[2];
    struct sky_layer_rec **layers;
    short **sounds;
    float field_0xc;
    float field_0x10;
    uint mask;
    int model;
    int model_done;
    float light_pos[12];
};

typedef struct sky_flash_piece_rec sky_flash_piece_rec, *Psky_flash_piece_rec;

struct sky_flash_piece_rec {
    int model;
    short rot[3];
    char field_0xa[2];
    float scale[3];
    float pos[3];
};

typedef struct sky_flash_variant_rec sky_flash_variant_rec, *Psky_flash_variant_rec;

struct sky_flash_variant_rec {
    int model;
    char light;
    char count;
    char field_0x6;
    char field_0x7;
    float *light_pos;
    struct sky_flash_piece_rec *pieces;
};

typedef struct snow_cell_rec snow_cell_rec, *Psnow_cell_rec;

struct snow_cell_rec {
    float x;
    float y;
    float z;
    int press_frame;
    int link[4];
    uchar links;
    char field_0x21;
    ushort state;
    int vert[2];
};

typedef struct snow_body_rec snow_body_rec, *Psnow_body_rec;

struct snow_body_rec {
    vec3 pos;
    vec3 dir;
    float len;
    float rad;
    float press;
    float press_total;
    int frames;
    uint flags;
    uchar state;
    uchar tag;
    char field_0x32[2];
    vec3 start;
    vec3 move;
    float move_len;
    float press_start;
    float press_total_start;
    float ratio;
    float curve;
    float spray;
    float spray_enter;
    float spray_leave;
};

typedef struct snow_spray_rec snow_spray_rec, *Psnow_spray_rec;

struct snow_spray_rec {
    float pos[3];
    vec3 vel;
    float gravity;
    float vel_y_min;
    uchar kind;
    char field_0x21;
    short variant;
    char field_0x24[12];
    int angle;
    int life;
};

typedef struct snow_press_ofs_rec snow_press_ofs_rec, *Psnow_press_ofs_rec;

struct snow_press_ofs_rec {
    int dx;
    int dz;
    int field_0x8[4];
    float field_0x18;
};

typedef struct leaf_source_rec leaf_source_rec, *Pleaf_source_rec;

struct leaf_source_rec {
    int type;
    int id;
    float radius;
    float strength;
    float limit_len;
    float limit_y;
    vec3 pos;
    vec3 push;
    vec3 dir;
};

/* readability round, agent B1 */
typedef struct life_gauge_rec life_gauge_rec, *Plife_gauge_rec;
struct life_gauge_rec {
    short hit_timer;
    short drain_timer;
    float life;
    float damage;
    float combo_damage;
};

typedef struct mes_label_rec mes_label_rec, *Pmes_label_rec;

struct mes_label_rec {
    float x;
    float y;
    int sprite;
};

typedef struct game_mes_rec game_mes_rec, *Pgame_mes_rec;

struct game_mes_rec {
    struct mes_label_rec *labels;
    int sprites[4];
};

typedef struct mes_pop_rec mes_pop_rec, *Pmes_pop_rec;

struct mes_pop_rec {
    short timer;
    short state;
};

typedef struct win_streak_part_rec win_streak_part_rec, *Pwin_streak_part_rec;

struct win_streak_part_rec {
    int sprite;
    float x;
    float y;
};

/* readability round, agent N1 */
/* N1 types begin */
typedef struct net_check_log_rec {
    char date_text[32];
    char ip_text[64];
    char serial_text[64];
    char note_text[256];
    int response_frames;
    int timeout_frames;
    int mode;
    int system_param;
    uint board_info[5];
    ushort place_id;
    char field_0x1c6[2];
    struct tm server_time;
    struct tm request_time;
    struct tm local_time;
    int clock_offset;
    char field_0x238[2048];
    char auth_stat_text[3];
    char upload_base_text[257];
    char place_text[9];
    char year_text[5];
    char month_text[3];
    char day_text[3];
    char hour_text[3];
    char minute_text[3];
    char second_text[3];
    char field_0xb59[3];
} net_check_log_rec;
typedef struct net_cmd_rec {
    char *name;
    int cmd;
} net_cmd_rec;

typedef struct alab_work_rec {
    char started;
    char field_0x1[3];
    int auth_state;
    int status;
    int dimm_state;
    int starts[2];
    int continues[2];
    int ends[2];
} alab_work_rec;

typedef struct alab_auth_info_rec {
    char *upload_base_text;
    uint reply_word1;
    int place_id;
    char *place_text;
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
} alab_auth_info_rec;
/* N1 types end */

/* readability round, agent S1 */
typedef struct sprite_cell sprite_cell, *Psprite_cell;

struct sprite_cell {
    int page_id;
    float u0;
    float v0;
    float u1;
    float v1;
};

typedef struct sprite_set_heap_rec sprite_set_heap_rec, *Psprite_set_heap_rec;

struct sprite_set_heap_rec {
    int set_no;
    struct static_heap_rec *heap;
};

struct sprite_font {
    int sprite_no;
    int first_row;
    float cell_width;
    float cell_height;
    float cell_u;
    float cell_v;
    int size;
};

/* readability round, agent B2 */
typedef struct task_node_rec task_node_rec, *Ptask_node_rec;

typedef struct task_rec task_rec, *Ptask_rec;

struct task_node_rec {
    uint resident;
    struct task_rec *task;
    struct task_node_rec *before;
    struct task_node_rec *after;
    void (*init)(void);
    void (*exec)(void);
    void (*free)(void);
    uint color;
    char *name;
};

struct task_rec {
    uint flags;
    void (*exec)(void);
    struct task_node_rec *node;
    float self_usec;
    float total_usec;
    float max_usec;
    float sum_usec;
    float count;
    float average_usec;
    char *name;
};

/* readability round, agent B3 */
typedef struct cns_nn_pos_rec cns_nn_pos_rec, *Pcns_nn_pos_rec;
struct cns_nn_pos_rec {
    float x;
    float y;
};

typedef struct cns_nn_style_rec cns_nn_style_rec, *Pcns_nn_style_rec;

struct cns_nn_style_rec {
    int frame;
    uint color;
    uint second_color;
};

typedef struct cns_nn_plate_rec cns_nn_plate_rec, *Pcns_nn_plate_rec;

struct cns_nn_plate_rec {
    int kind;
    int level;
    int kind_sprite;
    int part_sprites[4];
    int inner_sprite;
    int frame_sprite;
    struct cns_nn_style_rec *style;
};

typedef struct cns_nn_state_rec cns_nn_state_rec, *Pcns_nn_state_rec;

struct cns_nn_state_rec {
    char shown[3];
    undefined1 field_0x3;
    struct cns_nn_plate_rec plates[3];
    int loaded_sprites[64];
    float offset_x;
    float offset_y;
    int bg_obj;
    int frame_count;
};

typedef struct name_entry_letter_rec name_entry_letter_rec, *Pname_entry_letter_rec;

struct name_entry_letter_rec {
    short pick_sprite;
    undefined1 field_0x2[2];
    int box_sprite;
    float x;
    float y;
    short label_sprite;
    char code;
    undefined1 field_0x13;
};

typedef struct three_button_hold_rec three_button_hold_rec, *Pthree_button_hold_rec;

struct three_button_hold_rec {
    int count;
    int timer;
    int fired;
    int rising;
    int state;
    int field_0x14;
    int field_0x18;
};

typedef struct sel_chara_grid_rec sel_chara_grid_rec, *Psel_chara_grid_rec;

struct sel_chara_grid_rec {
    uchar flags;
    char chara;
    undefined1 field_0x2[2];
    char *name;
    int field_0x8;
};

typedef struct sel_stage_grid_rec sel_stage_grid_rec, *Psel_stage_grid_rec;

struct sel_stage_grid_rec {
    uchar flags;
    char stage;
    undefined1 field_0x2[2];
    char *name;
};

typedef struct sel_stage_info_rec sel_stage_info_rec, *Psel_stage_info_rec;

struct sel_stage_info_rec {
    int preview_scene;
    int preview_aet_set;
    int preview_sprite_set;
    char *icon_name;
    char *name;
    int grid_x;
    int grid_y;
};

/* readability round, agent B4 */
typedef struct pose_rule_rec pose_rule_rec, *Ppose_rule_rec;
struct pose_rule_rec {
    uint chara_mask;
    uchar flags;
    char field_0x5[3];
};

typedef struct cam_clip_rec cam_clip_rec, *Pcam_clip_rec;

struct cam_clip_rec {
    struct cam_curve_rec **channels;
    ushort *channel_heads;
    float *time_range;
    uint flags;
};

typedef struct both_pose_lose_rec both_pose_lose_rec, *Pboth_pose_lose_rec;

struct both_pose_lose_rec {
    short motion;
    short cams[4];
};

typedef struct cam_curve_rec cam_curve_rec, *Pcam_curve_rec;

struct cam_curve_rec {
    ushort count;
    char field_0x2[2];
    short *keys;
};

typedef struct demo_sprite_rec demo_sprite_rec, *Pdemo_sprite_rec;

struct demo_sprite_rec {
    int id;
    float x;
    float y;
};

typedef struct demo_layout_rec demo_layout_rec, *Pdemo_layout_rec;

struct demo_layout_rec {
    char type;
    char field_0x1[3];
    float x;
    float y;
    struct demo_sprite_rec *list;
};

/* readability round, agent B5 */
typedef struct jvs_board_input_rec jvs_board_input_rec, *Pjvs_board_input_rec;

struct jvs_board_input_rec {
    struct button_word buttons;
    char analog[8];
};

typedef struct player_input_rec player_input_rec, *Pplayer_input_rec;

struct player_input_rec {
    uint repressed;
    uint long_held;
    uint repeated;
    uint pressed;
    uint released;
    uint now;
    uint history[7];
    int hold_frames[16];
    int repress_frames[16];
    char repress_state[16];
    char repeat_frames[16];
    char repeat_state[16];
};

typedef struct ending_roll_row ending_roll_row, *Pending_roll_row;

struct ending_roll_row {
    int sprite;
    int field_0x4;
    float gap;
};

typedef struct static_heap_ent static_heap_ent, *Pstatic_heap_ent;

struct static_heap_ent {
    struct static_heap_rec *block;
    uint size;
    char *label;
};

typedef struct fog_event_rec fog_event_rec, *Pfog_event_rec;

struct fog_event_rec {
    short start;
    short end;
    uint flags;
    short r;
    short g;
    short b;
    short field_0xe;
};

typedef struct stage_intro_cam_rec stage_intro_cam_rec, *Pstage_intro_cam_rec;

struct stage_intro_cam_rec {
    char cam_no;
    char field_0x1[3];
    struct fog_event_rec *fog_events;
};

typedef struct ledger_snapshot_rec ledger_snapshot_rec, *Pledger_snapshot_rec;

struct ledger_snapshot_rec {
    char field_0x0[40];
    uint coin_1p;
    uint coin_2p;
    char field_0x30[8];
    uint coin_total;
    uint credit_coin;
    uint credit_service;
    uint credit_total;
    char field_0x48[176];
};

typedef struct light_src_rec light_src_rec, *Plight_src_rec;

struct light_src_rec {
    ushort type;
    ushort angle_min;
    ushort angle_max;
    char field_0x6[2];
    float pos[3];
    float dir[3];
    float field_0x20[3];
    float field_0x2c;
    float color[3];
    float dist_min;
    float dist_max;
};

typedef struct elan_light_rec elan_light_rec, *Pelan_light_rec;

struct elan_light_rec {
    char id;
    char type;
    char field_0x2[2];
    float field_0x4;
    float color[3];
    float pos[3];
    float dir[3];
    char diffuse;
    char specular;
    char routing;
    char spec_fade;
    char field_0x30;
    char field_0x31[3];
    float dist_min;
    float dist_max;
    int angle_min;
    int angle_max;
};

typedef struct elan_light_masks_rec elan_light_masks_rec, *Pelan_light_masks_rec;

struct elan_light_masks_rec {
    int flags;
    char field_0x4;
    char field_0x5;
    short field_0x6;
    short masks[4];
    float ambient0[4];
    float field_0x20[4];
    float ambient1[4];
};

typedef struct stage_light_table_rec stage_light_table_rec, *Pstage_light_table_rec;

struct stage_light_table_rec {
    float ambient[3];
    struct light_src_rec *lights[16];
};

typedef struct coli_poly_rec coli_poly_rec, *Pcoli_poly_rec;

struct coli_poly_rec {
    int field_0x0;
    uint flags;
    char field_0x8[2];
    uchar vertex_count;
    char field_0xb;
    short field_0xc;
    short field_0xe;
    int field_0x10;
    int field_0x14;
    float normal[3];
    float dist;
    float *verts[5];
};

typedef struct rep_rob_frame_rec rep_rob_frame_rec, *Prep_rob_frame_rec;

struct rep_rob_frame_rec {
    char hand_shape[2];
    uchar face_flags;
    char field_0x3;
    short heading;
    short facing_offset;
    ushort motion_flags;
    short move_no;
    short frame;
    short field_0xe;
    ushort side_flags;
    char field_0x12[2];
    uint state;
    uint flags24;
    uint flags28;
    uint flags2c;
    uint flags;
    uint action;
    vec3 place_pos;
    undefined *field_0x38;
    float frame_pos;
    float frame_speed;
    float root_offset_x;
    float root_offset_z;
    float root_offset_y;
};

typedef struct rep_frame_rec rep_frame_rec, *Prep_frame_rec;

struct rep_frame_rec {
    struct rep_rob_frame_rec robs[2];
    uint hit_light_flags;
    struct hit_light_pos_rec hit_light_pos;
    short sounds[4];
    char fence_segment;
    char field_0xb9;
    char field_0xba[2];
};

/* readability round, agent P1 */
typedef struct zanzou_trail_entry zanzou_trail_entry, *Pzanzou_trail_entry;

struct zanzou_trail_entry {
    vec3 edge_a;
    vec3 field_0xc;
    vec3 edge_b;
    vec3 field_0x24;
};

typedef struct zanzou_trail_rec zanzou_trail_rec, *Pzanzou_trail_rec;

struct zanzou_trail_rec {
    int on;
    short cur;
    short count;
    short value;
    short field_0xa;
    float field_0xc;
    float field_0x10;
    float edge_a_z_prev;
    float edge_a_z;
    struct obj_node *node;
    struct compiled_model_rec *model[2];
    struct zanzou_trail_entry *hist;
};

typedef struct tex_desc_rec tex_desc_rec, *Ptex_desc_rec;

struct tex_desc_rec {
    short width;
    short height;
    uint format;
    char *data;
    int field_0xc;
};

typedef struct txn_entry txn_entry, *Ptxn_entry;

struct txn_entry {
    short tex_no;
    char brightness;
    char saturation;
    short hue;
    short swap_tex_no;
    uchar field_0x8;
    uchar color_a[3];
    uchar color_on;
    uchar color_b[3];
};

typedef struct txn_tune_entry txn_tune_entry, *Ptxn_tune_entry;

struct txn_tune_entry {
    int tex_no;
    float brightness;
    float saturation;
    short hue;
    short field_0xe;
};

typedef struct txn_swap_entry txn_swap_entry, *Ptxn_swap_entry;

struct txn_swap_entry {
    int tex_no;
    int swap_tex_no;
};

typedef struct txn_color_entry txn_color_entry, *Ptxn_color_entry;

struct txn_color_entry {
    int tex_no;
    char field_0x4;
    uchar color_a[3];
    uchar color_on;
    uchar color_b[3];
};

/* readability round, agent X1 */
typedef struct strbuf_rec strbuf_rec, *Pstrbuf_rec;
struct strbuf_rec {
    char *base;
    char *end;
    char *write;
    char *read;
};

typedef struct dimm_dma_rec dimm_dma_rec, *Pdimm_dma_rec;

struct dimm_dma_rec {
    int busy;
    uint key;
    uint dst;
    uint offset;
    uint remaining;
    uint chunk_dst;
    uint chunk_offset;
    uint chunk_size;
    uint done;
};



typedef struct stage_sound_rec stage_sound_rec, *Pstage_sound_rec;

struct stage_sound_rec {
    short bank_no;
    short bgm_id;
    short se_bank_no;
    short se_a;
    short se_b;
    char field_0xa[2];
    void (*start)(void);
    void (*frame)(void);
    void (*stop)(void);
};

/* readability round, agent X2 */
typedef void (*timer_done_handler)(int arg);
typedef void (*irq_hook_fn)(void *arg);
typedef struct irq_hook irq_hook;
struct irq_hook {
    irq_hook_fn fn;
    struct irq_hook *next;
    void *arg;
    uint priority;
};
typedef struct g2_dma_regs g2_dma_regs;
struct g2_dma_regs {
    uint g2_addr;
    uint mem_addr;
    uint length;
    uint direction;
    uint trigger;
    uint enable;
    uint start;
    uint suspend;
};
typedef void (*g2_dma_done_fn)(int arg);
typedef struct g2_dma_channel g2_dma_channel;
struct g2_dma_channel {
    int number;
    uint g2_addr;
    uint mem_addr;
    uint length;
    uint direction;
    uint trigger;
    int field_0x18;
    g2_dma_done_fn done_fn;
    int done_arg;
    int hook_id;
    char in_use;
    char running;
    char done;
    char field_0x2b;
};
typedef struct g2_dma_state g2_dma_state;
struct g2_dma_state {
    uint regs_base;
    struct g2_dma_channel channel[4];
    char copy_masks_irq;
    char field_0xb5[3];
    int field_0xb8;
    int start_count[4];
    int done_count[4];
    int stop_count[4];
    int g2_fifo_waits;
    int aica_fifo_waits;
};
typedef struct token_list token_list;
struct token_list {
    int count;
    int cursor;
    char *tokens[128];
    char *text;
    char buf[256];
};
typedef struct form_map_pair form_map_pair;
struct form_map_pair {
    char *key;
    char *value;
};
typedef struct form_map form_map;
struct form_map {
    int count;
    struct form_map_pair pairs[60];
    int buf_size;
    char *buf;
    char buf_space[2048];
};
typedef struct ipv4_sock_addr ipv4_sock_addr;
struct ipv4_sock_addr {
    char len;
    char family;
    short port;
    int ip;
    int zero[2];
};
typedef struct http_slot http_slot;
struct http_slot {
    int sock;
    struct ipv4_sock_addr addr;
    int connect_started;
    ushort port;
    char host[258];
    int state;
    int board_wait;
    int length;
    int left;
    int header_size;
    int body_size;
    char *cursor;
    char text[4096];
    char scratch[4096];
};
typedef void (*http_dump_fn)(void *data, int size);

/* readability round, agent K1 */
typedef struct surface_rec surface_rec, *Psurface_rec;

struct surface_rec {
    uint kind;
    uint texel_size_class;
    uint pixel_format;
    uint width;
    uint height;
    uint bytes;
    uint format_flags;
    uint tex_address;
    char field_0x20[8];
};

typedef struct vram_node vram_node, *Pvram_node;

struct vram_node {
    ushort flags;
    short field_0x2;
    struct vram_node *prev;
    struct vram_node *next;
    int start;
    int size;
    void *owner;
};

typedef struct vram_half_rec vram_half_rec, *Pvram_half_rec;

struct vram_half_rec {
    struct vram_node *used_head;
    struct vram_node *used_tail;
    struct vram_node *free_head;
    struct vram_node *free_tail;
    uint free_bytes;
};

typedef struct packed_chunk_rec packed_chunk_rec, *Ppacked_chunk_rec;

struct packed_chunk_rec {
    ushort flags;
    short field_0x2;
    struct packed_chunk_rec *next;
    int base;
    struct surface_rec *slots[16];
};

typedef struct packed_class_rec packed_class_rec, *Ppacked_class_rec;

struct packed_class_rec {
    uint class_mark;
    uint stride;
    uint field_0x8;
    uint chunk_bytes;
    uint slot_count;
};

/* readability round, agent K2 */
typedef struct elan_light_params_rec elan_light_params_rec, *Pelan_light_params_rec;

struct elan_light_params_rec {
    int color;
    int dir_fixed[3];
    float pos[3];
    uint mode[3];
    int on;
    float near_dist;
    float far_dist;
    float cone_angle[2];
    int field_0x3c;
};

typedef struct xfer_job_rec xfer_job_rec, *Pxfer_job_rec;

struct xfer_job_rec {
    struct xfer_job_rec *next;
    uint src_addr;
    uint src_len;
    uint dest;
    uint dma_ctrl;
    uint dma_src;
    uint dma_len;
    uint mode;
    uint kind;
    uint from_dest;
};

/* readability round, agent K3 */
/* K3 types */
typedef struct strip_context_rec strip_context_rec, *Pstrip_context_rec;
struct strip_context_rec {
    uint change_mask;
    uint header_type;
    uint list_type;
    uint color_format;
    uint uv16;
    uint depth_compare;
    uint cull_mode;
    uint field_0x1c;
    uint tex_offset_bits;
    uint two_volume;
    uint z_write_off;
    uint src_blend;
    uint dst_blend;
    uint src_select;
    uint dst_select;
    uint fog_mode;
    uint gouraud;
    uint alpha_on;
    uint tex_alpha_off;
    uint uv_clamp;
    uint uv_flip;
    uint tex_filter;
    uint tsp_bit12_on;
    uint mip_bias;
    uint shade_op;
    uint colors_clamped;
    uint pal_group;
    struct surface_rec *surface;
    float face_color[4];
    float offset_color[4];
    uint ctrl_word;
    uint isp_tsp_word;
    uint tsp_word;
    uint tex_word;
    uint volume_word;
    float bounds[4];
    uint isp_bit20_on;
    uint strip_len_code;
    uint clip_kind;
};

/* K3 end */

/* readability round, agent Q1 */
typedef struct sock_slot_rec sock_slot_rec, *Psock_slot_rec;

struct sock_slot_rec {
    int done;
    int op;
    int result;
    int req_addr;
    int copy_back[4];
    int busy_word;
};

/* readability round, agent Q2 */
typedef struct dimm_command_def dimm_command_def, *Pdimm_command_def;

struct dimm_command_def {
    int arg_size;
    int poll_limit;
    int arg_dimm_offset;
    int arg_shared_offset;
};

typedef struct dimm_board_extra_rec dimm_board_extra_rec, *Pdimm_board_extra_rec;

struct dimm_board_extra_rec {
    char field_0x0[32];
    char cmd6_arg[4];
    char field_0x24[12];
    char cmd10_arg[4];
    char field_0x34[36];
};

/* readability round, agent G1 */
typedef struct tex_slot_rec tex_slot_rec, *Ptex_slot_rec;
struct tex_slot_rec {
    struct surface_rec surface;
    int status;
    int in_use;
    uint size_word;
    uint tex_control_word;
    int slot_id;
};

typedef struct tex_target_bank tex_target_bank, *Ptex_target_bank;

struct tex_target_bank {
    uint param_base;
    uint region_array;
    uint obj_list_base;
    uint field_0xc;
    uint bg_plane;
    uint size;
};

typedef struct fb_write_regs fb_write_regs, *Pfb_write_regs;

struct fb_write_regs {
    uint write_ctrl;
    uint stride_units;
    uint write_start;
    uint x_clip;
    uint y_clip;
};

typedef struct tex_target_rec tex_target_rec, *Ptex_target_rec;

struct tex_target_rec {
    tex_target_bank bank[2];
    int list_starts[6];
    int block_sizes[5];
    fb_write_regs fb;
    uint vram_block;
    uint vram_block_size;
    uint tile_clip;
    uint list_alloc_word;
    uint tiles_x;
    uint tiles_y;
    uint tile_count;
    uint width;
    uint height;
    tex_slot_rec *tex;
};

typedef struct render_target_rec render_target_rec, *Prender_target_rec;

struct render_target_rec {
    tex_slot_rec *slot;
    tex_target_rec target;
    uint list_mask;
    int texidx;
    uint bg_words[3];
};

typedef struct pass_entry_rec pass_entry_rec, *Ppass_entry_rec;

struct pass_entry_rec {
    uint list_mask;
    char field_0x4[40];
    tex_target_rec *target;
    int ring_index;
    uint bg_words[3];
};

/* readability round, agent G2 */
typedef struct capture_rec {
    uint list_mask;
    uint list_start[5];
    uint list_size[5];
} capture_rec;


typedef struct model_xform_pair model_xform_pair, *Pmodel_xform_pair;
struct model_xform_pair {
    float *mtx;
    struct compiled_model_rec *model;
};

typedef int (*irq_slot_store_fn)(int slot, uint fn);

typedef void (*irq_event_ack_fn)(void);

typedef struct irq_ipr_field {
    char reg_offset;
    char shift;
    ushort mask;
} irq_ipr_field;

/* readability round, agent L1 */
typedef struct pal_group_rec {
    short first_block;
    char field_0x2;
    uchar block_count;
    short use_count;
    short field_0x6;
} pal_group_rec;

typedef struct mtx_stack_rec {
    short depth;
    short limit;
    float *base;             /* its first slot */
    float *top;              /* its top slot */
} mtx_stack_rec;

/* the matrix stacks' records: the active stack's working copy, then the
 * saved records of the main, view and projection stacks; mtx_stack_select
 * swaps a saved record into the active one, mtx_stacks_start sets them up */
typedef struct mtx_stack_record_set {
    mtx_stack_rec active;     /* +0x00 */
    mtx_stack_rec main;       /* +0x0c */
    mtx_stack_rec view;       /* +0x18 */
    mtx_stack_rec proj;       /* +0x24 */
} mtx_stack_record_set;

typedef struct video_mode_rec {
    int video_out;
    int timing;
    short width;
    short height;
} video_mode_rec;

/* readability round, agent L2 */
typedef struct light_color_rec light_color_rec, *Plight_color_rec;

struct light_color_rec {
    float field_0x0;
    float red;
    float green;
    float blue;
};

typedef struct light_table_entry light_table_entry, *Plight_table_entry;

struct light_table_entry {
    short on;
    short type;
    char field_0x4[16];
    struct light_color_rec color;
    char field_0x24[16];
    float pos[3];
    float dir[3];
    float atten[3];           /* +0x4c: attenuation terms: constant, linear, quadratic (the lit senders divide by their polynomial in the distance when it is above 1) */
    float cone_cos;
    float view_pos[3];
    float view_dir[3];
    float model_pos[3];
    float model_dir[3];
    char field_0x8c[4];
    float range_inv;          /* +0x90: set with range by light_range_set */
    char field_0x94[4];
    float range_sq;           /* +0x98: range squared: the lit senders compare it with the squared distance to the light */
    float cone_scale;
    float range;
    float cone_sin;
    char field_0xa8[8];
};

typedef struct texidx_map_entry texidx_map_entry, *Ptexidx_map_entry;

struct texidx_map_entry {
    int use_count;
    int slot_no;
};

typedef int (*tex_load_hook_fn)(short *header);

/* readability round, agent L4 */
typedef struct model_part_span_rec model_part_span_rec, *Pmodel_part_span_rec;

struct model_part_span_rec {
    int offset;
    int size;
};

typedef struct compiled_model_rec compiled_model_rec, *Pcompiled_model_rec;
struct compiled_model_rec {
    int format;
    char field_0x4[4];
    float sphere[4];
    int size;
    int field_0x1c;
    int triangle_count;
    int vertex_count;
    int field_0x28;
    int field_0x2c;
    char field_0x30[12];
    int buf_size;
    struct model_part_span_rec parts[4];
};


typedef struct model_cmd_iter_rec model_cmd_iter_rec, *Pmodel_cmd_iter_rec;

struct model_cmd_iter_rec {
    struct compiled_model_rec *model;
    int list;
    int cur;
    int end;
    int vertex_size;
    int vertices_left;
};

/* readability round, agent A2 */
typedef struct block_heap_unit block_heap_unit, *Pblock_heap_unit;

struct block_heap_unit {
    struct block_heap_unit *next;
    uint size;
    int field_0x8[14];
};

typedef struct block_heap_rec block_heap_rec, *Pblock_heap_rec;

struct block_heap_rec {
    struct block_heap_unit anchor;
    struct block_heap_unit *free_list;
    uint usable_bytes;
    struct block_heap_unit *first_block;
};

typedef struct naominet_job_rec naominet_job_rec, *Pnaominet_job_rec;

struct naominet_job_rec {
    struct naominet_job_rec *next;
    struct naominet_job_rec *prev;
    int in_use;
    int state;
    int player;
    int er_flag;
    int di_flag;
    int kind;
    int sock_error;
    uint start_ticks;
};

typedef struct naominet_job_queue_rec naominet_job_queue_rec, *Pnaominet_job_queue_rec;

struct naominet_job_queue_rec {
    struct naominet_job_rec *head;
    struct naominet_job_rec *tail;
};

typedef struct naominet_settings_rec naominet_settings_rec, *Pnaominet_settings_rec;

struct naominet_settings_rec {
    int tag;
    struct naominet_settings_rec *next;
    int auto_reauth;
    int player_count;
    int field_0x10;
    int ip_mode;
    int shift_jis;
    uint ip;
    int field_0x20[3];
    char *game_id;
    char *version;
    char *serial;
    char field_0x38[80];
};

typedef struct naominet_id_rec naominet_id_rec, *Pnaominet_id_rec;

struct naominet_id_rec {
    int tag;
    struct naominet_id_rec *next;
    int hi_number;
    int si_number;
};

typedef struct naominet_score_args_rec naominet_score_args_rec, *Pnaominet_score_args_rec;

struct naominet_score_args_rec {
    struct naominet_machine_rec *machine;
    int player;
    int kind;
    int er_flag;
    int di_flag;
    int play_count[16];
    int field_0x54[16];
    int play_seconds[16];
    uint start_ticks[16];
    uint end_ticks[16];
};

typedef struct naominet_score_option_rec naominet_score_option_rec, *Pnaominet_score_option_rec;

struct naominet_score_option_rec {
    int hi_number;
    int si_number;
    char kc_text[25];
    char sc_text[5];
    char kk_text[2];
    char ss_text[5];
    char er_text[2];
    char di_text[5];
};

typedef struct http_client_url_rec http_client_url_rec, *Phttp_client_url_rec;

struct http_client_url_rec {
    char url[256];
    char proto[256];
    char host[256];
    char path[256];
    short port;
};

typedef struct http_client_request_rec http_client_request_rec, *Phttp_client_request_rec;

struct http_client_request_rec {
    int header_count;
    char version[10];
    char method[10];
    char path[256];
    char names[15][256];
    char values[15][256];
};

typedef struct http_client_response_rec http_client_response_rec, *Phttp_client_response_rec;

struct http_client_response_rec {
    int version;
    int status;
    char reason[256];
    char names[15][256];
    char values[15][256];
    char field_0x1f08[4];
};

typedef struct http_client_body_rec http_client_body_rec, *Phttp_client_body_rec;

struct http_client_body_rec {
    uint content_length;
    uint chunk_size;
    int conn_close;
    int length;
    char data[4096];
};

typedef struct http_client_recv_rec http_client_recv_rec, *Phttp_client_recv_rec;

struct http_client_recv_rec {
    int state;
    int last_count;
    int received;
    int room;
    char *buf;
    char *buf_end;
    char *status_line;
    char *cursor;
};

typedef struct http_client_conn_rec http_client_conn_rec, *Phttp_client_conn_rec;

struct http_client_conn_rec {
    int slot;
    int state;
    int sock;
    int connected;
    int sock_error;
    uint start_ticks;
    struct ipv4_sock_addr addr;
    uint limit1;
    uint limit2;
    uint limit3;
    int send_state;
    int sent;
    char *send_block;
    char *send_text;
    struct http_client_request_rec *request;
    char *request_body;
    struct http_client_recv_rec recv;
    struct http_client_response_rec *response;
    struct http_client_body_rec *response_body;
};

/* readability round, agent A3 */
typedef struct sprite_record_rec {
    uint header[4];
    uint base_color;
    short has_alpha;
    short pal_group;
    float width_texels;
    float height_texels;
} sprite_record_rec;

typedef struct sprite_quad_args {
    int index;
    float x;
    float y;
    float z;
    float zoom_x;
    float zoom_y;
    float u0;
    float v0;
    float u1;
    float v1;
    int angle;
    float alpha;
    int list;
    int attr;
    int base_color;
    int offset_color;
} sprite_quad_args;

typedef struct dtpk_job_rec {
    uint job;
    int words_per_step;
    int whole_size;
    int bytes_left;
    int move_bytes;
    uint src;
    uint dst;
    uint bank_start;
} dtpk_job_rec;

/* readability round, agent Z1 */
typedef struct scan_stream_rec scan_stream_rec, *Pscan_stream_rec;

struct scan_stream_rec {
    uchar *pos;
    uchar *base;
    int left;
    int size;
    ushort flags;
    char file;
    char field_0x13[1];
    uchar *saved_pos;
    int saved_left;
    int pushback_size;
    uchar *pushback;
    uchar pushback_buf[4];
};


typedef struct math_error_rec math_error_rec, *Pmath_error_rec;

struct math_error_rec {
    int kind;
    char *func_name;
    float arg1;
    float arg2;
    float result;
    int error_no;
};

/* readability round, agent T1 */
typedef struct card_test_rec card_test_rec, *Pcard_test_rec;

struct card_test_rec {
    int usable;
    int kind;
    int step;
    int sub_step;
    int message;
    int message_timer;
    uint flags;
    int result;
    ushort value;
    char field_0x22[2];
    int word_0x24;
    int word_0x28;
};

typedef struct card_menu_rec card_menu_rec, *Pcard_menu_rec;

struct card_menu_rec {
    int frame;
    int cursor;
    int item_enabled[5];
    struct card_test_rec unit[2];
};

typedef struct menu_item_rec menu_item_rec, *Pmenu_item_rec;

struct menu_item_rec {
    int x;
    int y;
    char *text;
};

typedef struct input_test_row_rec input_test_row_rec, *Pinput_test_row_rec;

struct input_test_row_rec {
    char *format;
    int bit;
};

typedef struct card_test_message_rec card_test_message_rec, *Pcard_test_message_rec;

struct card_test_message_rec {
    char *text;
    int kind;
};

/* readability round, agent V1 */
typedef struct list_bank_list_rec list_bank_list_rec, *Plist_bank_list_rec;

struct list_bank_list_rec {
    int field_0x0;
    int opened;
    int dma_busy;
    int seg_count;
    struct list_block_ref first_seg;
    struct list_block_ref end_seg;
    struct list_block_ref last_seg;
    struct xfer_job_rec *job_head;
    struct xfer_job_rec *job_tail;
    int job_count;
};

typedef struct list_bank_rec list_bank_rec, *Plist_bank_rec;

struct list_bank_rec {
    int state;
    int last_list;
    int job_count;
    struct list_bank_list_rec lists[5];
};

/* readability round, agent V2 */
typedef struct leaf_cfg_rec leaf_cfg_rec, *Pleaf_cfg_rec;

struct leaf_cfg_rec {
    uint ground_model_id;
    uint fly_model_id;
    uint shadow_model_id;
    int leaf_count;
    int ground_count;
    int fly_max;
    int spawn_interval;
    float ring_radius;
    float gravity;
    float drag;
    float spin;
    float kick_limit_len;
    float kick_limit_y;
    float kick_push;
    float kick_limit_len_alt;
    float kick_limit_y_alt;
    float kick_push_alt;
    float half_width;
    float floor_y;
    float drop_half_width;
    float drop_floor_y;
    int field_0x54;
    float fall_floor_y;
    int grid_cells;
    int spawn_timer;
    int in_use_count;
    float grid_half_width;
    float cells_per_unit;
};

/* readability round, agent V4 */
typedef struct coli_grid_rec coli_grid_rec, *Pcoli_grid_rec;

struct coli_grid_rec {
    float x0;
    float z0;
    float scale;
    ushort width;
    ushort rows;
    ushort cell_count;
    ushort field_0x12;
};

typedef struct coli_wall_rec coli_wall_rec, *Pcoli_wall_rec;

struct coli_wall_rec {
    char field_0x0;
    char wall_no;
    short field_0x2;
    coli_poly_rec **polys;
    float field_0x8[3];
};

typedef struct field_coli_rec field_coli_rec, *Pfield_coli_rec;

struct field_coli_rec {
    int field_0x0;
    int version;
    float *verts;
    coli_poly_rec *polys;
    struct coli_grid_rec *grid;
    coli_poly_rec ***cells;
    int *wall_root;
    int *groups;
    int field_0x20;
    int *blocks;
    int block;
    int vert_count;
    int poly_count;
    int cell_count;
    int wall_count;
    int field_0x3c;
    int entry_count;
    int field_0x44;
    int field_0x48;
};

/* readability round, agent V5 */
typedef struct rank_usage_entry rank_usage_entry, *Prank_usage_entry;

struct rank_usage_entry {
    char chara;
    char field_0x1[3];
    int count;
};

typedef struct level_entry level_entry, *Plevel_entry;

struct level_entry {
    char *name;
    short sprites[3];
    short field_0xa;
};

typedef struct shine_style shine_style, *Pshine_style;

struct shine_style {
    int sprite;
    int flags;
    int flags2;
    short rot;
    short field_0xe;
};

/* readability round, agent V6 */
typedef struct msnm_try_group_rec msnm_try_group_rec, *Pmsnm_try_group_rec;
struct msnm_try_group_rec {
    struct msnm_try_rec tries[3];
    int weights[3];
};

typedef void (*enemy_state_fn)(void); /* an enemy AI state's frame routine */
typedef void (*enemy_mot_type_fn)(void); /* an enemy motion type's routine */

/* readability round, agent V0b */
/* V0b types */
typedef struct rob_action_handler_rec rob_action_handler_rec, *Prob_action_handler_rec;

struct rob_action_handler_rec {
    void (*start)(struct rob_rec *rob);
    void (*update)(struct rob_rec *rob);
};

typedef struct demo_cmd_rec demo_cmd_rec, *Pdemo_cmd_rec;

struct demo_cmd_rec {
    short frame;
    char cmd;
    char field_0x3;
    void *arg;
};

typedef struct demo_script_rec demo_script_rec, *Pdemo_script_rec;

struct demo_script_rec {
    short frame;
    short field_0x2;
    struct demo_cmd_rec *cmds;
};

typedef struct demo_aet_layer_rec demo_aet_layer_rec, *Pdemo_aet_layer_rec;

struct demo_aet_layer_rec {
    char *name;
    int play_flags;                          /* given to aet_param_flags_set: 4 = hold at the end */
    uint handle;
};

typedef struct appose_pose_motion_rec appose_pose_motion_rec, *Pappose_pose_motion_rec;

struct appose_pose_motion_rec {
    ushort motion;
    short camera;
};

typedef struct appose_pair_data_rec appose_pair_data_rec, *Pappose_pair_data_rec;

struct appose_pair_data_rec {
    char script;
    char field_0x1;
    short camera;
    ushort rob_motion;
    ushort opp_motion;
};

typedef struct sound_id_rec sound_id_rec, *Psound_id_rec;

struct sound_id_rec {
    int request;
    int field_0x4;
    char *name;
};

typedef struct sound_bank_entry_rec sound_bank_entry_rec, *Psound_bank_entry_rec;

struct sound_bank_entry_rec {
    short bank;
    short field_0x2;
    void *data;
    char *name;
};

typedef struct sound_music_entry_rec sound_music_entry_rec, *Psound_music_entry_rec;

struct sound_music_entry_rec {
    short bank;
    short sound_id;
    char *name;
};

typedef void (*camera_mode_fn)(void); /* a camera mode's frame routine */
typedef void (*demo_cmd_fn)(void); /* an attract demo script command */
typedef void (*demo_scene_fn)(void); /* an attract demo scene's init or draw routine */
typedef void (*appose_script_fn)(void); /* one step of an appose script */

/* readability round 5b, agent X2 */
typedef int (*dimm_cmd_handler_fn)(uint, uint, uint, uint);

/* readability round 5b */
typedef struct control_context_entry control_context_entry, *Pcontrol_context_entry;

struct control_context_entry {
    void (*handler)(struct control_rec *ctrl);
    int flag;
};

typedef struct elan_proj_cmd_rec elan_proj_cmd_rec, *Pelan_proj_cmd_rec;

struct elan_proj_cmd_rec {
    uint cmd;
    uint field_0x4;
    float scale_x;
    float offset_x;
    float scale_y;
    float offset_y;
    uint field_0x18;
    uint field_0x1c;
};

typedef struct elan_tile_clip_cmd_rec elan_tile_clip_cmd_rec, *Pelan_tile_clip_cmd_rec;

struct elan_tile_clip_cmd_rec {
    uint cmd;
    uint field_0x4;
    uint field_0x8;
    uint field_0xc;
    uint x0;
    uint y0;
    uint x1;
    uint y1;
};

typedef struct sprite_euc_font sprite_euc_font, *Psprite_euc_font;

struct sprite_euc_font {
    short page_sprites[32];
    float cell_width;
    float line_height;
    char page_count;
    char field_0x49[3];
};

/* readability round 5b */
typedef struct sel_chara_info_rec sel_chara_info_rec, *Psel_chara_info_rec;

struct sel_chara_info_rec {
    int chara_scene;
    int name_scene;
    int icon_scene;
    float icon_pos[2][2];
    char *small_layers[2];
    char *large_layers[2];
    char *decided_layers[2];
    char *profile_layer;
    char *name_layers[2];
    char *icon_layers[2];
    float versus_offsets[2][2];
};

/* ---- the ELAN renderers (hand-written assembly, source/asm/0c104260.s on) ---- */

/* One group of an object, as the ELAN renderers walk them from object +0x18
 * (a group whose first word is 0 ends the object). Its runs follow at +0x50:
 * a flags word (bits 0-1 culling, bit 3 independent triangles, bit 4 strip,
 * tested only for a group crossing the near plane, bit 6 Gouraud, bit 7 on a
 * positive word after a run = another run under the same header; a word of 0
 * or below ends the group), a count, then that many vertex entries. */
typedef struct elan_group_rec {
    uint control_word;        /* +0x00: parameter control word (negative; list in bits 24-26, color type in bits 4-5: with bit 4 set the group keeps the colors of the group before) */
    uint isp_word;            /* +0x04 */
    uint tsp_word;            /* +0x08 */
    uint tex_word;            /* +0x0c */
    float sphere[4];          /* +0x10: bounding sphere: center x, y, z, radius */
    undefined4 field_0x20;    /* not read by the renderers */
    char material;            /* +0x24: above 0 a specular row (the lit renderers: 0 and up lit), 0 diffuse, -1 unlit, -2 bump mapped, -3 vertex colors */
    char field_0x25[3];
    float ambient_scale;      /* +0x28: factor on the ambient color (the intensity renderers' group intensity) */
    float base_color[4];      /* +0x2c: alpha, red, green, blue */
    float offset_color[4];    /* +0x3c: alpha, red, green, blue; for a bump group (material -2) the first three are k, K1 and the light scale s */
    int run_bytes;            /* +0x4c: bytes from +0x50 to the next group */
} elan_group_rec;

/* A vertex entry of a run: 32 bytes in place, with bit 0 set in its first
 * word and in its last (the clippers step backwards by the last word), or
 * an 8-byte reference (elan_vertex_ref). */
typedef struct elan_vertex_rec {
    float x;
    float y;
    float z;
    float normal[3];
    float u;
    float v;
} elan_vertex_rec;

/* A reference to a vertex stored earlier: a word with bit 0 clear, then the
 * byte offset (bit 0 clear) from the end of the reference to the vertex. */
typedef struct elan_vertex_ref {
    int tag;
    int offset;
} elan_vertex_ref;

/* The vertex of a vertex-color group's runs (material -3). */
typedef struct elan_vertex_colour_rec {
    float x;
    float y;
    float z;
    uint normal_bytes;        /* +0x0c: the normal as signed bytes (nx << 16 | ny << 8 | nz); the top byte positive = lit */
    uint color;               /* +0x10: packed alpha, red, green, blue */
    undefined4 field_0x14;    /* not read */
    float u;
    float v;
} elan_vertex_colour_rec;

/* The vertex of a bump group's runs (material -2): 56 bytes, always in
 * place. The senders take L.N for the lighting bytes and the angle of L in
 * the T, B plane for the direction byte. */
typedef struct elan_bump_vertex_rec {
    float x;
    float y;
    float z;
    float n[3];
    float t[3];
    float b[3];
    float u;
    float v;
} elan_bump_vertex_rec;

/* A kept vertex of the near-plane clippers that send packed colors (the
 * vertex-color hooks' rings argb_clip_ring / argb_offset_clip_ring and
 * bump_clip_ring): four 32-byte slots a ring, filled from the end down. */
typedef struct argb_clip_slot {
    float near_value;         /* +0x00: the transform's fourth output (minus the depth), the value compared with the near limit */
    float x;                  /* +0x04: screen x before the divide by depth */
    float y;                  /* +0x08 */
    float inv_w;              /* +0x0c: 1 / depth */
    float u;                  /* +0x10 */
    float v;                  /* +0x14 */
    uint base_color;          /* +0x18: packed ARGB (the bump ring: the group's base color word) */
    uint offset_color;        /* +0x1c: packed ARGB; 0 in argb_clip_ring (the bump ring: the vertex's offset word) */
} argb_clip_slot;

/* A kept vertex of the intensity near-plane clippers (32 bytes). */
typedef struct intensity_clip_slot {
    float near_value;         /* +0x00: minus the depth */
    float x;                  /* +0x04: x before the divide by depth */
    float y;
    float inv_depth;
    float u;
    float v;
    float base;               /* +0x18: base intensity */
    float offset;             /* +0x1c: offset intensity */
} intensity_clip_slot;

/* A kept vertex of the near-plane clippers that send float colors (the lit
 * and float-color rings): 64 bytes. */
typedef struct float_colour_clip_slot {
    float near_value;         /* +0x00: minus the depth */
    float x;                  /* +0x04: x before the divide by depth */
    float y;
    float inv_depth;          /* +0x0c */
    float u;                  /* +0x10 (stored as raw words) */
    float v;
    undefined4 unused[2];     /* +0x18: never written; the kept-vertex senders copy the two words out as they stand */
    float base[4];            /* +0x20: base color: alpha, red, green, blue */
    float offset[4];          /* +0x30: offset color (zeros where the path has none) */
} float_colour_clip_slot;

/* A kept vertex of the vertex-color near-plane clippers: 64 bytes, of which
 * +0x28 on is not used. */
typedef struct vertex_colour_clip_slot {
    float near_value;         /* +0x00: minus the depth */
    float x;
    float y;
    float inv_depth;
    float u;
    float v;
    float color[4];           /* +0x18: alpha, red, green, blue */
    undefined4 unused[6];
} vertex_colour_clip_slot;

/* The 64-byte modifier volume triangle modvol_strips_send writes through the
 * store queues: each corner is X, Y, 1/w. Its slot helpers put the two
 * vertices before the newest alternately in a and c and the newest in b. */
typedef struct modvol_triangle_rec {
    uint control;             /* +0x00: vertex parameter control word (end of strip) */
    float a[3];               /* +0x04 */
    float b[3];               /* +0x10 */
    float c[3];               /* +0x1c */
    undefined4 unused[6];     /* +0x28: not written */
} modvol_triangle_rec;
