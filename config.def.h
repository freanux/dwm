/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 3;        /* border pixel of windows */
// static const unsigned int gappx     = 11;        /* gaps between windows */
static const unsigned int gappx     = 13;        /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 0;    /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 6;   /* systray spacing */
static const unsigned int bh_margin = 7;        /* upper and lower statusbar margins */
static const unsigned int txt_margin = 4;       /* upper and lower text margins */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
//static const char *fonts[]          = { "Monofoki Nerd Font:size=14.5" };
static const char *fonts[]          = { "JetBrainsMonoNLSlashed Nerd Font:size=13" };
static const char dmenufont[]       = "monospace:size=10";
static const char col_gray1[]       = "#1e1e2e";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_title[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char col_menu[]        = "#121221";
static const char col_active[]      = "#63aeed";
static const char col_inactive[]    = "#434c5e";
static const char *colors[][3]      = {
	/*                fg         bg         border   */
	[SchemeNorm]  = { col_gray3, col_gray1, col_inactive },
	[SchemeSel]   = { col_gray4, col_menu,  col_active   },
	[SchemeTitle] = { col_title, col_gray1, col_active   },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class                instance    title       tags mask   iscentered      isfloating      monitor */
	{ "Firefox",            NULL,       NULL,       1 << 8,     0,              0,              -1 },
    { "firefox",            "Alert",    NULL,       0,          1,              1,              -1 },
	{ "fnb_release",        NULL,       NULL,       0,          0,              1,              -1 },
    { "gnome-calculator",   NULL,       NULL,       0,          1,              1,              -1 },
    { "Galculator",         NULL,       NULL,       0,          1,              1,              -1 },
    { "Vinagre",            NULL,       NULL,       0,          1,              1,              -1 },
    { "Mumble",             "mumble",   NULL,       0,          1,              1,              -1 },
    { "Eww",                "eww",      NULL,       0,          1,              1,              -1 },
    { "Osmo",               "osmo",     NULL,       0,          1,              1,              -1 },
    { "codelite-terminal",  NULL,       NULL,       0,          1,              1,              -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define ALTKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "alacritty", NULL };
//static const char *termcmd[]  = { "kitty", "--title", "Terminal", NULL };
static const char *rofi[] = { "rofi", "-show", "drun", "-show-emojis", NULL }; 
static const char *mute[] = { "/home/flynn/scripts/dwm/togglemute.sh", NULL };
static const char *voldn[] = { "/home/flynn/scripts/dwm/volume_down.sh", NULL };
static const char *volup[] = { "/home/flynn/scripts/dwm/volume_up.sh", NULL };
static const char *togglesink[] = { "/home/flynn/scripts/dwm/togglesink.sh", NULL };
static const char *screenshot[] = { "/home/flynn/scripts/dwm/screenshot.sh", NULL };
static const char *quicklaunch[] = { "/home/flynn/scripts/dwm/quick_launcher.sh", NULL };
static const char *slaap[] = { "systemctl", "suspend", NULL };
static const char *colorpicker[] = { "xcolor", "-s", "clipboard", NULL };
static const char *firefox[] = { "firefox", NULL };
static const char *firefox_private[] = { "firefox", "--private-window", NULL };
static const char *togglemicro[] = { "/home/flynn/scripts/dwm/togglemicro.sh", NULL };
static const char *filebrowser[] = { "/home/flynn/scripts/dwm/filebrowser.sh", NULL };
static const char *calendar[] = { "/home/flynn/scripts/dwm/calendar.sh", NULL };
static const char *tools1[] = { "/home/flynn/scripts/dwm/tools1.sh", NULL };
static const char *tools2[] = { "/home/flynn/scripts/dwm/tools2.sh", NULL };
static const char *tools3[] = { "/home/flynn/scripts/dwm/tools3.sh", NULL };
static const char *status_tool[] = { "/home/flynn/scripts/dwm/status_tool.sh", NULL };
static const char *lockscreen[] = { "/home/flynn/scripts/dwm/lockscreen.sh", NULL };

static const Key keys[] = {
	/* modifier                     key                         function        argument */
    { 0,                            XF86XK_AudioMute,           spawn,          { .v = mute } },
    { MODKEY,                       XF86XK_AudioMute,           spawn,          { .v = togglesink } },
	{ 0,                            XF86XK_AudioLowerVolume,    spawn,          { .v = voldn } },
	{ 0,                            XF86XK_AudioRaiseVolume,    spawn,          { .v = volup } },
    { 0,                            XK_Print,                   spawn,          { .v = screenshot } },
    { 0,                            XF86XK_Tools,               spawn,          { .v = tools1 } },
    { MODKEY,                       XF86XK_Tools,               spawn,          { .v = tools2 } },
    { ControlMask,                  XF86XK_Tools,               spawn,          { .v = tools3 } },
    { 0,                            XF86XK_Launch5,             spawn,          { .v = filebrowser } },
    { MODKEY,                       XF86XK_Launch5,             spawn,          { .v = calendar } },
    { MODKEY,                       XF86XK_Launch6,             spawn,          { .v = colorpicker } },
    { 0,                            XF86XK_Launch6,             spawn,          { .v = firefox } },
    { ControlMask,                  XF86XK_Launch6,             spawn,          { .v = firefox_private } },
    { 0,                            XF86XK_Launch7,             spawn,          { .v = quicklaunch } },
    { MODKEY,                       XF86XK_Launch7,             spawn,          { .v = slaap } },
    { 0,                            XF86XK_Launch8,             spawn,          { .v = togglemicro } },
	{ MODKEY,                       XK_p,                       spawn,          { .v = rofi } },
	{ MODKEY,                       XK_Escape,                  spawn,          { .v = rofi } },
	{ MODKEY|ShiftMask,             XK_Return,                  spawn,          { .v = termcmd } },
    { ALTKEY,                       XK_Escape,                  spawn,          { .v = termcmd } },
    { MODKEY,                       XK_l,                       spawn,          { .v = lockscreen } },
	{ MODKEY,                       XK_b,                       togglebar,      { 0 } },
	{ MODKEY,                       XK_j,                       focusstack,     { .i = +1 } },
	{ MODKEY,                       XK_k,                       focusstack,     { .i = -1 } },
	{ MODKEY,                       XK_i,                       incnmaster,     { .i = +1 } },
	{ MODKEY,                       XK_d,                       incnmaster,     { .i = -1 } },
	{ MODKEY|ALTKEY,                XK_Left,                    setmfact,       { .f = -0.05 } },
	{ MODKEY|ALTKEY,                XK_Right,                   setmfact,       { .f = +0.05 } },
	{ MODKEY,                       XK_Return,                  zoom,           { 0 } },
	{ MODKEY|ShiftMask,             XK_c,                       killclient,     { 0 } },
    { MODKEY|ALTKEY,                XK_Escape,                  killclient,     { 0 } },
    { ALTKEY,                       XK_F4,                      killclient,     { 0 } },
	{ MODKEY,                       XK_t,                       setlayout,      { .v = &layouts[0] } },
	{ MODKEY,                       XK_f,                       setlayout,      { .v = &layouts[1] } },
	{ MODKEY,                       XK_m,                       setlayout,      { .v = &layouts[2] } },
	{ MODKEY,                       XK_z,                       setlayout,      { .v = &layouts[2] } },
	{ MODKEY,                       XK_space,                   setlayout,      { 0 } },
	{ MODKEY|ShiftMask,             XK_space,                   togglefloating, { 0 } },
	{ MODKEY,                       XK_0,                       view,           { .ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,                       tag,            { .ui = ~0 } },
	{ MODKEY,                       XK_comma,                   focusmon,       { .i = -1 } },
	{ MODKEY,                       XK_period,                  focusmon,       { .i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,                   tagmon,         { .i = -1 } },
	{ MODKEY|ShiftMask,             XK_period,                  tagmon,         { .i = +1 } },
	{ MODKEY,                       XK_KP_Subtract,             setgaps,        {.i = -1 } },
	{ MODKEY,                       XK_KP_Add,                  setgaps,        {.i = +1 } },
	{ MODKEY,                       XK_KP_Multiply,             setgaps,        {.i = 0 } },
	{ ALTKEY,                       XK_Tab,                     focusstack,     { .i = +1 } },
	{ ALTKEY|ShiftMask,             XK_Tab,                     focusstack,     { .i = -1 } },
	{ MODKEY,                       XK_Right,                   viewnext,       {0} },
	{ MODKEY,                       XK_Left,                    viewprev,       {0} },
    { MODKEY,                       XK_section,                 view,           { 0 } },
	{ MODKEY,                       XK_Tab,                     viewnext,       {0} },
	{ MODKEY|ShiftMask,             XK_Tab,                     viewprev,       {0} },
	{ MODKEY|ALTKEY,                XK_Tab,                     viewprev,       {0} },
    { MODKEY|ShiftMask,             XK_Right,                   tagtonext,      {0} },
	{ MODKEY|ShiftMask,             XK_Left,                    tagtoprev,      {0} },
	TAGKEYS(                        XK_1,                                       0)
	TAGKEYS(                        XK_2,                                       1)
	TAGKEYS(                        XK_3,                                       2)
	TAGKEYS(                        XK_4,                                       3)
	TAGKEYS(                        XK_5,                                       4)
	TAGKEYS(                        XK_6,                                       5)
	TAGKEYS(                        XK_7,                                       6)
	TAGKEYS(                        XK_8,                                       7)
	TAGKEYS(                        XK_9,                                       8)
	{ MODKEY|ShiftMask,             XK_q,                       quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button1,        spawn,          {.v = status_tool } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
