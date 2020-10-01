/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx = 3; /* border pixel of windows */
static const unsigned int snap = 32;    /* snap pixel */
static const unsigned int gappih = 20;  /* horiz inner gap between windows */
static const unsigned int gappiv = 10;  /* vert inner gap between windows */
static const unsigned int gappoh = 10;  /* horiz outer gap between windows and screen edge */
static const unsigned int gappov = 30;  /* vert outer gap between windows and screen edge */
static const int swallowfloating = 0;   /* 1 means swallow floating windows by default */
static const int smartgaps = 0;         /* 1 means no outer gap when there is only one window */
static const int showbar = 1;           /* 0 means no bar */
static const int topbar = 1;            /* 0 means bottom bar */
static const char* fonts[] = {"Fira Code:size=11", "JoyPixels:pixelsize=11:antialias=true:autohint=true"};
static char dmenufont[] = "Fira Code:size=11";
static char normbgcolor[] = "#222222";
static char normbordercolor[] = "#444444";
static char normfgcolor[] = "#bbbbbb";
static char selfgcolor[] = "#eeeeee";
static char selbordercolor[] = "#770000";
static char selbgcolor[] = "#005577";
static char* colors[][3] = {
    /*               fg           bg           border   */
    [SchemeNorm] = {normfgcolor, normbgcolor, normbordercolor},
    [SchemeSel] = {selfgcolor, selbgcolor, selbordercolor},
};

typedef struct
{
    const char* name;
    const void* cmd;
} Sp;
const char* spcmd1[] = {"st", "-n", "spterm", "-g", "120x34", NULL};
const char* spcmd2[] = {"st", "-n", "spcalc", "-f", "monospace:size=16", "-g", "50x20", "-e", "bc", "-lq", NULL};
static Sp scratchpads[] = {
    /* name          cmd  */
    {"spterm", spcmd1},
    {"spranger", spcmd2},
};

/* tagging */
static const char* tags[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9"};

static const Rule rules[] = {
    /* xprop(1):
     *	WM_CLASS(STRING) = instance, class
     *	WM_NAME(STRING) = title
     */
    /* class      instance    title       tags mask     isfloating   monitor */
    {"Steam", "Steam", NULL, 1 << 4, 0, 0, 0, 0},
    {"dota2", "dota2", NULL, 1 << 5, 0, 0, 0, 0},
    {"discord", NULL, "Discord", 1 << 2, 0, 0, 0, -1},
    {"ProtonMail Bridge", "protonmail-bridge", "ProtonMail Bridge", 0, 1, 0, 0, -1},
    {"St", NULL, NULL, 0, 0, 1, 0, -1},
    {NULL, NULL, "Microsoft Teams Notification", 0, 1, 0, 0, -1},
    {NULL, NULL, "go-twitch-cli", 0, 0, 0, 1, -1},
    {NULL, NULL, "newsboat", 0, 0, 0, 1, -1},
    {NULL, "spterm", NULL, SPTAG(0), 1, 1, 0, -1},
    {NULL, "spcalc", NULL, SPTAG(1), 1, 1, 0, -1},
};


/* layout(s) */
static const float mfact = 0.55;  /* factor of master area size [0.05..0.95] */
static const int nmaster = 1;     /* number of clients in master area */
static const int resizehints = 1; /* 1 means respect size hints in tiled resizals */
#define FORCE_VSPLIT 1            /* nrowgrid layout: force two clients to always split vertically */

#include "vanitygaps.c"
static const Layout layouts[] = {
    /* symbol     arrange function */
    {"[]=", tile},   /* Default: Master on left, slaves on right */
    {"TTT", bstack}, /* Master on top, slaves on bottom */

    {"[@]", spiral},   /* Fibonacci spiral */
    {"[\\]", dwindle}, /* Decreasing in size right and leftward */

    {"H[]", deck},    /* Master on left, slaves in monocle-like mode on right */
    {"[M]", monocle}, /* All windows on top of eachother */

    {"|M|", centeredmaster},         /* Master in middle, slaves on sides */
    {">M>", centeredfloatingmaster}, /* Same but master floats */

    {"><>", NULL}, /* no layout function means floating behavior */
    {NULL, NULL},
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY, TAG)                                                                             \
    {MODKEY, KEY, view, {.ui = 1 << TAG}}, {MODKEY | ControlMask, KEY, toggleview, {.ui = 1 << TAG}}, \
        {MODKEY | ShiftMask, KEY, tag, {.ui = 1 << TAG}},                                             \
        {MODKEY | ControlMask | ShiftMask, KEY, toggletag, {.ui = 1 << TAG}},
#define STACKKEYS(MOD, ACTION)                                                              \
    {MOD, XK_j, ACTION##stack, {.i = INC(+1)}}, {MOD, XK_k, ACTION##stack, {.i = INC(-1)}}, \
        {MOD, XK_v, ACTION##stack, {.i = 0}}, /* { MOD, XK_grave, ACTION##stack, {.i = PREVSEL } }, \ */
/* { MOD, XK_a,     ACTION##stack, {.i = 1 } }, \ */
/* { MOD, XK_z,     ACTION##stack, {.i = 2 } }, \ */
/* { MOD, XK_x,     ACTION##stack, {.i = -1 } }, */

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd)                                          \
    {                                                       \
        .v = (const char*[]) { "/bin/sh", "-c", cmd, NULL } \
    }

void resetCols()
{
    // normbgcolor           = "#222222";
    // normbordercolor       = "#444444";
    // normfgcolor           = "#bbbbbb";
    // selfgcolor            = "#eeeeee";
    // selbordercolor        = "#770000";
    // selbgcolor            = "#005577";
    // for (i = 0; i < LENGTH(colors); i++)
    // scheme[i] = drw_scm_create(drw, colors[i], 3);
}

void exitdwm(const Arg* arg)
{
    // quit(arg);
    system("sudo killall -15 Xorg");
}

void changemasterfact(Arg* arg)
{
    setmfact(arg);
}

void changevolume(const Arg* arg)
{
    int w = arg->i;
    switch (w)
    {
        case -1:
            // system("pactl set-sink-volume 0 -5%");
            if (fork() == 0)
            {
                system("changevolume pa -5");
                exit(0);
            }
            break;
        case +1:
            if (fork() == 0)
            {
                system("changevolume pa +5");
                exit(0);
            }
            // system("pactl set-sink-volume 0 +5%");
            break;
        case -2:
            if (fork() == 0)
            {
                // system("changevolume mpc -5");
                system("airplay-remote VolumeDown");
                exit(0);
            }
            // system("mpc volume -5");
            break;
        case +2:
            if (fork() == 0)
            {
                // system("changevolume mpc +5");
                system("airplay-remote VolumeUp");
                exit(0);
            }
            // system("mpc volume +5");
            break;
    }
}

void focustomon(const Arg* arg)
{
    unsigned int i = 0;
    for (Monitor* m = mons; m; m = m->next, i++)
    {
        if (i == arg->ui)
        {
            if (selmon->sel)
                unfocus(selmon->sel, 0);
            selmon = m;
            focus(NULL);
            if (selmon->sel)
            {
                XWarpPointer(dpy, NULL, selmon->sel->win, 0, 0, 0, 0, selmon->sel->w / 2, selmon->sel->h / 2);
            }
            else
            {
                XWarpPointer(dpy, None, selmon->barwin, 0, 0, 0, 0, selmon->mw / 2, selmon->mh / 2);
            }
            break;
        }
    }
}

void movetomon(const Arg* arg)
{
    unsigned int i = 0;
    if (!selmon->sel)
        return;
    for (Monitor* m = mons; m; m = m->next, i++)
    {
        if (i == arg->ui)
        {
            sendmon(selmon->sel, m);
            unfocus(selmon->sel, 0);
            selmon = m;
            focus(NULL);
            XWarpPointer(dpy, NULL, selmon->sel->win, 0, 0, 0, 0, selmon->sel->w / 2, selmon->sel->h / 2);
            arrange(selmon);
            break;
        }
    }
}

void togglefullscreen(const Arg* arg)
{
    if (!selmon->sel)
        return;
    if (selmon->sel->isfullscreen)
        setfullscreen(selmon->sel, 0);
    else
        setfullscreen(selmon->sel, 1);
}

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char* dmenucmd[] = {"dmenu_run",
                                 "-m",
                                 dmenumon,
                                 "-fn",
                                 dmenufont,
                                 "-nb",
                                 normbgcolor,
                                 "-nf",
                                 normfgcolor,
                                 "-sb",
                                 selbgcolor,
                                 "-sf",
                                 selfgcolor,
                                 NULL};
static const char* dmenuunicode[] = {"echo /home/hintz3y/.config/emoji | dmenu",
                                     "-m",
                                     dmenumon,
                                     "-fn",
                                     dmenufont,
                                     "-nb",
                                     normbgcolor,
                                     "-nf",
                                     normfgcolor,
                                     "-sb",
                                     selbgcolor,
                                     "-sf",
                                     selfgcolor,
                                     NULL};
static const char* termcmd[] = {"st", NULL};

static const char* termcwd[] = {"samedir", NULL};


#include <X11/XF86keysym.h>

#include "movestack.c"
#include "shiftview.c"
static Key keys[] = {
    /* modifier                     key        function        argument */
    // First Row
    {MODKEY, XK_q, killclient, {0}},
    {MODKEY | ShiftMask, XK_q, killclient, {.i = 1}},

    {MODKEY, XK_w, spawn, SHCMD("$BROWSER")},
    {MODKEY | ShiftMask, XK_w, spawn, SHCMD("st -e rtv --enable-media")},

    {MODKEY, XK_e, spawn, SHCMD("st -e neomutt; pkill -RTMIN+12 dwmblocks")},
    //{ MODKEY|ShiftMask,			XK_e,		spawn,	{.v = NULL} },

    {MODKEY, XK_r, spawn, SHCMD("st -e lf")},
    //{ MODKEY|ShiftMask,			XK_r,		spawn,	{.v = LFcmd} },

    {MODKEY, XK_t, spawn, SHCMD("st -e go-twitch-cli 2>>~/.cache/go-streamlink/log")},
    {MODKEY | ShiftMask, XK_t, spawn, SHCMD("st -e watch tranmission-remote -l")},

    //{MODKEY, XK_y, spawn, SHCMD("st -e calcurse -D ~/.local/share/calcurse -C ~/.config/calcurse")},
    //{ MODKEY|ShiftMask,			XK_y,		spawn,	{.v = calcurse} },
    {MODKEY, XK_y, spawn, SHCMD("teams")},
    {MODKEY | ShiftMask, XK_y, spawn, SHCMD("pkill teams")},

    //{ MODKEY,						XK_u,		spawn,	{.v = NULL} },
    //{ MODKEY|ShiftMask,			XK_u,		spawn,	{.v = NULL} },

    {MODKEY, XK_i, spawn, SHCMD("st -e gotop")},
    //{ MODKEY|ShiftMask,			XK_i,		spawn,	{.v = htopcmd} },

    //{ MODKEY,						XK_o,		incnmaster,	{.i = -1 } },
    {MODKEY | ShiftMask, XK_o, incnmaster, {.i = -1}},

    //{ MODKEY,						XK_p,		spawn,	{.v = passmenu} },
    {MODKEY | ShiftMask, XK_p, spawn, SHCMD("passmenu")},

    // Second Row
    //{ MODKEY,						XK_a,	spawn,	{.v = pulsemixercmd} },
    {MODKEY | ShiftMask, XK_a, spawn, SHCMD("st -e pulsemixer")},

    {MODKEY, XK_s, togglescratch, {.ui = 0}},
    {MODKEY | ShiftMask, XK_s, togglescratch, {.ui = 1}},

    {MODKEY, XK_d, spawn, {.v = dmenucmd}},
    //{ MODKEY|ShiftMask,			XK_d,		spawn,	{.v = dmenucmd} },

    {MODKEY, XK_f, togglefullscreen, {0}},
    //{ MODKEY|ShiftMask,			XK_f,		togglefullscreen,	{0} },

    {MODKEY, XK_g, spawn, SHCMD("steam")},
    {MODKEY | ShiftMask, XK_g, spawn, SHCMD("sudo killall steam")},

    {MODKEY, XK_h, focustomon, {.ui = 0}},
    {MODKEY | ShiftMask, XK_h, movetomon, {.ui = 0}},

    {MODKEY, XK_j, focusstack, {.i = +1 + 2000}},
    {MODKEY | ShiftMask, XK_j, movestack, {.i = +1}},

    {MODKEY, XK_k, focusstack, {.i = -1 + 2000}},
    {MODKEY | ShiftMask, XK_k, movestack, {.i = -1}},

    {MODKEY, XK_l, focustomon, {.ui = 1}},
    {MODKEY | ShiftMask, XK_l, movetomon, {.ui = 1}},

    //{ MODKEY,						XK_i,		incnmaster,	{.i = +1 } },
    {MODKEY | ShiftMask, XK_i, incnmaster, {.i = +1}},


    // Third Row

    // Unused
    //{ MODKEY,						XK_z,		spawn,	{.v = NULL } },
    //{ MODKEY|ShiftMask,			XK_z,		spawn,	{.v = NULL } },

    {MODKEY, XK_x, spawn, SHCMD("sudo killall gpg-agent")},
    {MODKEY | ShiftMask, XK_x, spawn, SHCMD("sudo killall gpg-agent; slock; mw sync")},

    {MODKEY, XK_c, spawn, SHCMD("discord-canary")},
    {MODKEY | ShiftMask, XK_c, spawn, SHCMD("sudo killall DiscordCanary")},

    // Unused
    //{ MODKEY,						XK_v,		spawn,	{.v = NULL} },
    //{ MODKEY|ShiftMask,			XK_v,		spawn,	{.v = NULL} },

    //{ MODKEY,						XK_b,		spawn,	{.v = NULL} },
    {MODKEY | ShiftMask, XK_b, togglebar, {0}},

    {MODKEY, XK_n, spawn, SHCMD("st -e newsboat")},
    //{ MODKEY|ShiftMask,				XK_n,		nextlayout,	{.i = -1}},

    // Side mouse button
    {0,
     XK_KP_0,
     spawn,
     SHCMD(
         "pulsemixer --id $(pulsemixer --list-sources | grep Default | awk '{print $3}' | sed 's/,//g') --toggle-mute; "
         "pkill -RTMIN+11 $STATUSBAR")},
    {ShiftMask,
     XK_KP_0,
     spawn,
     SHCMD("pulsemixer --id $(pulsemixer --list-sinks | grep Default | awk '{print $3}' | sed 's/,//g') --toggle-mute; "
           "pkill -RTMIN+11 $STATUSBAR")},
    //{MODKEY,
    // XK_m,
    // spawn,
    // SHCMD("pulsemixer --id $(pulsemixer --list-sinks | grep Default | awk '{print $3}' | sed 's/,//g') --toggle-mute;
    // " "pkill -RTMIN+11 $STATUSBAR")},
    //{MODKEY | ShiftMask,
    // XK_m,
    // spawn,
    // SHCMD("pulsemixer --id $(pulsemixer --list-sources | grep Default | awk '{print $3}' | sed 's/,//g') "
    //"--toggle-mute; pkill -RTMIN+11 $STATUSBAR")},


    // Misc

    {MODKEY, XK_equal, changemasterfact, {.f = 0.05}},
    {MODKEY, XK_minus, changemasterfact, {.f = -0.05}},


    {MODKEY, XK_grave, spawn, SHCMD("dmenuunicode")},

    {MODKEY, XK_space, zoom, {0}},
    {MODKEY | ShiftMask, XK_space, togglefloating, {0}},

    {MODKEY, XK_Return, spawn, SHCMD("st")},
    {MODKEY | ShiftMask, XK_Return, spawn, SHCMD("samedir")},


    {MODKEY, XK_Scroll_Lock, spawn, SHCMD("screenkey")},
    {MODKEY, XK_Delete, spawn, SHCMD("showclip")},

    {MODKEY | ShiftMask, XK_Escape, exitdwm, {0}},

    {0, XK_Print, spawn, SHCMD("maimpick")},
    {MODKEY, XK_Print, spawn, SHCMD("dmenurecord")},
    {MODKEY | ShiftMask, XK_Print, spawn, SHCMD("dmenurecord kill")},


    // Media Keys
    {MODKEY | ShiftMask, XK_Page_Up, changevolume, {.i = +2}},
    {MODKEY | ShiftMask, XK_Page_Down, changevolume, {.i = -2}},
    {MODKEY, XK_Page_Up, changevolume, {.i = +1}},
    {MODKEY, XK_Page_Down, changevolume, {.i = -1}},

    {0, XF86XK_MonBrightnessUp, spawn, SHCMD("xbacklight +5")},
    {0, XF86XK_MonBrightnessDown, spawn, SHCMD("xbacklight -5")},

    //{ MODKEY,						XK_Insert,	spawn,	{.v = mpctoggle } },
    //{ MODKEY|ShiftMask,				XK_Insert,	spawn,	{.v = mpcstop} },
    //{ MODKEY,						XK_Home,	spawn,	{.v = mpcnext} },
    //{ MODKEY,						XK_End,		spawn,	{.v = mpcprev} },


    /**/
    TAGKEYS(XK_1, 0) TAGKEYS(XK_2, 1) TAGKEYS(XK_3, 2) TAGKEYS(XK_4, 3) TAGKEYS(XK_5, 4) TAGKEYS(XK_6, 5)
        TAGKEYS(XK_7, 6) TAGKEYS(XK_8, 7) TAGKEYS(XK_9, 8)

            {MODKEY, XK_0, view, {.ui = ~0}},
    {MODKEY | ShiftMask, XK_0, tag, {.ui = ~0}},
    {MODKEY, XK_F2, quit, {0}},

    {MODKEY, XK_F5, xrdb, {.v = NULL}},

    {MODKEY, XK_F7, spawn, SHCMD("vpn-toggle")},
    {MODKEY, XK_F8, spawn, SHCMD("mw sync")},
    {MODKEY, XK_F11, spawn, SHCMD("ducksearch")},
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
    /* click                event mask      button          function        argument */
    {ClkWinTitle, 0, Button2, zoom, {0}},
    {ClkStatusText, 0, Button1, sigdwmblocks, {.i = 1}},
    {ClkStatusText, 0, Button2, sigdwmblocks, {.i = 2}},
    {ClkStatusText, 0, Button3, sigdwmblocks, {.i = 3}},
    {ClkStatusText, 0, Button4, sigdwmblocks, {.i = 4}},
    {ClkStatusText, 0, Button5, sigdwmblocks, {.i = 5}},
    {ClkStatusText, ShiftMask, Button1, sigdwmblocks, {.i = 6}},
    {ClkStatusText, ShiftMask, Button3, spawn, SHCMD("st -e nvim /opt/dwmblocks/config.h")},
    {ClkClientWin, MODKEY, Button1, movemouse, {0}},
    {ClkClientWin, MODKEY, Button2, defaultgaps, {0}},
    {ClkClientWin, MODKEY, Button3, resizemouse, {0}},
    {ClkClientWin, MODKEY, Button4, incrgaps, {.i = +1}},
    {ClkClientWin, MODKEY, Button5, incrgaps, {.i = -1}},
    {ClkTagBar, 0, Button1, view, {0}},
    {ClkTagBar, 0, Button3, toggleview, {0}},
    {ClkTagBar, MODKEY, Button1, tag, {0}},
    {ClkTagBar, MODKEY, Button3, toggletag, {0}},
    {ClkTagBar, 0, Button4, shiftview, {.i = -1}},
    {ClkTagBar, 0, Button5, shiftview, {.i = 1}},
    {ClkRootWin, 0, Button2, togglebar, {0}},
};
