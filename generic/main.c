#include "tclstuff.h"
#include <tcl.h>
#include <inttypes.h>
#include <xwiimote.h>

#define NS	"xwii::"

static int g_testmode = 0;
static Tcl_ThreadDataKey	thread_mesg_callbacks;
static Tcl_ThreadDataKey	thread_static_keys;
typedef int montior_method_handler(struct xwii_monitor* handle, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);

// Static string Tcl_Objs <<<
struct static_keys {
	int			initialized;
	Tcl_Obj*	type;
	Tcl_Obj*	status;
	Tcl_Obj*	ext_type;
	Tcl_Obj*	none;
	Tcl_Obj*	nunchuk;
	Tcl_Obj*	classic;
	Tcl_Obj*	balance;
	Tcl_Obj*	motionplus;
	Tcl_Obj*	unknown;
	Tcl_Obj*	bdaddr;
	Tcl_Obj*	btclass;
	Tcl_Obj*	name;
	Tcl_Obj*	sec;
	Tcl_Obj*	nsec;
	Tcl_Obj*	mesgs;
	Tcl_Obj*	battery;
	Tcl_Obj*	btn;
	Tcl_Obj*	buttons;
	Tcl_Obj*	acc;
	Tcl_Obj*	stick;
	Tcl_Obj*	l_stick;
	Tcl_Obj*	r_stick;
	Tcl_Obj*	l;
	Tcl_Obj*	r;
	Tcl_Obj*	right_top;
	Tcl_Obj*	right_bottom;
	Tcl_Obj*	left_top;
	Tcl_Obj*	left_bottom;
	Tcl_Obj*	x;
	Tcl_Obj*	y;
	Tcl_Obj*	z;
	Tcl_Obj*	phi;
	Tcl_Obj*	theta;
	Tcl_Obj*	psi;
	Tcl_Obj*	angle_rate;
	Tcl_Obj*	low_speed;
	Tcl_Obj*	error;
	Tcl_Obj*	disconnect;
	Tcl_Obj*	comm;
	Tcl_Obj*	ir;
	Tcl_Obj*	sources;
};
//>>>
static void init_static_keys(struct static_keys* keys) //<<<
{
#define MAKE_KEY(name, str) \
	Tcl_IncrRefCount((keys->name = Tcl_NewStringObj(str, -1)));

	MAKE_KEY(type, "type");
	MAKE_KEY(status, "status");
	MAKE_KEY(ext_type, "ext_type");
	MAKE_KEY(none, "none");
	MAKE_KEY(nunchuk, "nunchuk");
	MAKE_KEY(classic, "classic");
	MAKE_KEY(balance, "balance");
	MAKE_KEY(motionplus, "motionplus");
	MAKE_KEY(unknown, "unknown");
	MAKE_KEY(bdaddr, "bdaddr");
	MAKE_KEY(btclass, "btclass");
	MAKE_KEY(name, "name");
	MAKE_KEY(sec, "sec");
	MAKE_KEY(nsec, "nsec");
	MAKE_KEY(mesgs, "mesgs");
	MAKE_KEY(battery, "battery");
	MAKE_KEY(btn, "btn");
	MAKE_KEY(buttons, "buttons");
	MAKE_KEY(acc, "acc");
	MAKE_KEY(stick, "stick");
	MAKE_KEY(l_stick, "l_stick");
	MAKE_KEY(r_stick, "r_stick");
	MAKE_KEY(l, "l");
	MAKE_KEY(r, "r");
	MAKE_KEY(right_top, "right_top");
	MAKE_KEY(right_bottom, "right_bottom");
	MAKE_KEY(left_top, "left_top");
	MAKE_KEY(left_bottom, "left_bottom");
	MAKE_KEY(x, "x");
	MAKE_KEY(y, "y");
	MAKE_KEY(z, "z");
	MAKE_KEY(phi, "phi");
	MAKE_KEY(theta, "theta");
	MAKE_KEY(psi, "psi");
	MAKE_KEY(angle_rate, "angle_rate");
	MAKE_KEY(low_speed, "low_speed");
	MAKE_KEY(error, "error");
	MAKE_KEY(disconnect, "disconnect");
	MAKE_KEY(comm, "comm");
	MAKE_KEY(ir, "ir");
	MAKE_KEY(sources, "sources");
	keys->initialized = 1;
}

//>>>

/*
#define EVAL(args ...) _eval(args, NULL)
static int _eval(Tcl_Interp* interp, int flags, ...) //<<<
{
	Tcl_Obj*	cmd = Tcl_NewObj();
	Tcl_Obj*	obj;
	int			res;
	va_list		ap;

	va_start(ap, flags);
	obj = va_arg(ap, Tcl_Obj*);
	while (obj) {
		TEST_OK(Tcl_ListObjAppendElement(interp, cmd, obj));
		obj = va_arg(ap, Tcl_Obj*);
	}
	va_end(ap);

	Tcl_IncrRefCount(cmd);
	res = Tcl_EvalObjEx(interp, cmd, flags || TCL_EVAL_DIRECT);
	Tcl_DecrRefCount(cmd);
	return res;
}

//>>>
*/

static int glue_testmode(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //<<<
{
	// Internal: set the test mode (used by the unit tests)
	CHECK_ARGS(1, "enabled");
	TEST_OK(Tcl_GetBooleanFromObj(interp, objv[1], &g_testmode));
	return TCL_OK;
}

//>>>
static int monitor_method_get_chan(struct xwii_monitor* monitor, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //<<<
{
	int fd = xwii_monitor_get_fd(monitor);
	Tcl_Channel	channel;

	channel = Tcl_MakeFileChannel(fd, TCL_READABLE);
	Tcl_RegisterChannel(interp, channel);
	channel_name = Tcl_GetChannelName(channel);

	Tcl_SetObjResult(interp, Tcl_NewStringObj(channel_name, -1));
	return TCL_OK;
}

//>>>
static int monitor_method_read(struct xwii_monitor* monitor, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //<<<
{
	Tcl_Obj*	res = Tcl_NewObj();
	char*		devname;

	while ((devname = xwii_monitor_poll(monitor)) != NULL) {
		TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewStringObj(devname, -1)));
	}

	Tcl_SetObjResult(interp, res);
	return TCL_OK;
}

//>>>
static int monitor_cmd(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj const* objv[]) //<<<
{
	struct xwii_monitor*	monitor = (struct xwii_monitor*)cdata;

	int		method;
	const char* methods[] = {
		"get_chan",
		"read",
		NULL
	};
	monitor_method_handler* methodtable[] = {
		monitor_method_get_chan,
		monitor_method_read,
	};

	if (objc < 2)
		CHECK_ARGS(1, "method ?args ...?");

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[1], methods, "method", TCL_EXACT,
				&method));

	return (methodtable[method])(monitor, interp, objv, objv);
}

//>>>
static int monitor(ClientData c, Tcl_Interp* interp, int objc, Tcl_Obj const* objv[]) //<<<
{
	Tcl_Obj*				cmd;
	static int				cmdseq=1;
	Tcl_Channel				channel;
	int						fd;
	int						poll=1, direct=0;
	struct monitor_cb*		cb;

	if (objc > 3)
		CHECK_ARGS(2, "?poll? ?direct?");

	if (objc > 1) TEST_OK(Tcl_GetBooleanFromObj(interp, objv[1], &poll));
	if (objc > 2) TEST_OK(Tcl_GetBooleanFromObj(interp, objv[2], &direct));

	monitor = xwii_monitor_new(poll, direct);

	cmd = Tcl_ObjPrintf("xwii_monitor%d", cmdseq++);
	Tcl_CreateObjCommand(interp, Tcl_GetString(cmd),
			monitor_cmd, monitor, monitor_release);
	Tcl_SetObjResult(interp, cmd);

	return TCL_OK;
}

//>>>
int Xwiimote_Init(Tcl_Interp* interp) //<<<
{
	Tcl_HashTable*	mesg_callbacks;
	struct static_keys*	keys;

	if (Tcl_InitStubs(interp, "8.6", 0) == NULL) return TCL_ERROR;

	mesg_callbacks = Tcl_GetThreadData(&thread_mesg_callbacks, sizeof(*mesg_callbacks));
	keys = Tcl_GetThreadData(&thread_static_keys, sizeof(*keys));
	if (!keys->initialized) {
		init_static_keys(keys);
		Tcl_InitHashTable(mesg_callbacks, TCL_ONE_WORD_KEYS);
		fprintf(stderr, "Initialized mesg_callbacks: %s\n", Tcl_HashStats(mesg_callbacks));
	}

	NEW_CMD(NS "monitor", glue_monitor);

	NEW_CMD(NS "_testmode", glue_testmode);

	TEST_OK(Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION));

	return TCL_OK;
}

//>>>

// vim: foldmethod=marker foldmarker=<<<,>>> ts=4 shiftwidth=4
