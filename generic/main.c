#include "tclstuff.h"
#include <tcl.h>
#include <inttypes.h>
#include <xwiimote.h>
#include <errno.h>
#include <stdint.h>

#define NS	"xwii::"

static int g_testmode = 0;
static Tcl_ThreadDataKey	thread_static_keys;
typedef int monitor_method_handler(struct xwii_monitor* handle, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);
typedef int iface_method_handler(struct xwii_iface* dev, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]);

// Static string Tcl_Objs <<<
struct static_keys {
	int			initialized;
	Tcl_Obj*	CORE;
	Tcl_Obj*	ACCEL;
	Tcl_Obj*	IR;
	Tcl_Obj*	MOTION_PLUS;
	Tcl_Obj*	NUNCHUK;
	Tcl_Obj*	CLASSIC_CONTROLLER;
	Tcl_Obj*	BALANCE_BOARD;
	Tcl_Obj*	PRO_CONTROLLER;
	Tcl_Obj*	ALL;
	Tcl_Obj*	WRITABLE;
	Tcl_Obj*	sec;
	Tcl_Obj*	usec;
	Tcl_Obj*	type;
	Tcl_Obj*	key;
	Tcl_Obj*	state;
	Tcl_Obj*	up;
	Tcl_Obj*	down;
	Tcl_Obj*	repeat;
	Tcl_Obj*	unknown;
	Tcl_Obj*	code;
	Tcl_Obj*	left;
	Tcl_Obj*	right;
	Tcl_Obj*	a;
	Tcl_Obj*	b;
	Tcl_Obj*	plus;
	Tcl_Obj*	minus;
	Tcl_Obj*	home;
	Tcl_Obj*	one;
	Tcl_Obj*	two;
	Tcl_Obj*	x;
	Tcl_Obj*	y;
	Tcl_Obj*	z;
	Tcl_Obj*	tl;
	Tcl_Obj*	tr;
	Tcl_Obj*	zl;
	Tcl_Obj*	zr;
	Tcl_Obj*	thumbl;
	Tcl_Obj*	thumbr;
	Tcl_Obj*	accel;
	Tcl_Obj*	ir;
	Tcl_Obj*	balance_board;
	Tcl_Obj*	motion_plus;
	Tcl_Obj*	pro_controller_key;
	Tcl_Obj*	pro_controller_move;
	Tcl_Obj*	classic_controller_key;
	Tcl_Obj*	classic_controller_move;
	Tcl_Obj*	nunchuk_key;
	Tcl_Obj*	nunchuk_move;
	Tcl_Obj*	watch;
	Tcl_Obj*	drums_key;
	Tcl_Obj*	drums_move;
	Tcl_Obj*	guitar_key;
	Tcl_Obj*	guitar_move;
	Tcl_Obj*	gone;
	Tcl_Obj*	weights;
	Tcl_Obj*	stick;
	Tcl_Obj*	whammy;
	Tcl_Obj*	fretbar;
	Tcl_Obj*	sources;
	Tcl_Obj*	factor;
};
//>>>
static void init_static_keys(struct static_keys* keys) //<<<
{
#define MAKE_KEY(name, str) \
	Tcl_IncrRefCount((keys->name = Tcl_NewStringObj(str, -1)));

	MAKE_KEY(CORE, "CORE");
	MAKE_KEY(ACCEL, "ACCEL");
	MAKE_KEY(IR, "IR");
	MAKE_KEY(MOTION_PLUS, "MOTION_PLUS");
	MAKE_KEY(NUNCHUK, "NUNCHUK");
	MAKE_KEY(CLASSIC_CONTROLLER, "CLASSIC_CONTROLLER");
	MAKE_KEY(BALANCE_BOARD, "BALANCE_BOARD");
	MAKE_KEY(PRO_CONTROLLER, "PRO_CONTROLLER");
	MAKE_KEY(ALL, "ALL");
	MAKE_KEY(WRITABLE, "WRITABLE");
	MAKE_KEY(sec, "sec");
	MAKE_KEY(usec, "usec");
	MAKE_KEY(type, "type");
	MAKE_KEY(key, "key");
	MAKE_KEY(state, "state");
	MAKE_KEY(repeat, "repeat");
	MAKE_KEY(unknown, "unknown");
	MAKE_KEY(code, "code");
	MAKE_KEY(left, "left");
	MAKE_KEY(right, "right");
	MAKE_KEY(up, "up");
	MAKE_KEY(down, "down");
	MAKE_KEY(a, "a");
	MAKE_KEY(b, "b");
	MAKE_KEY(plus, "plus");
	MAKE_KEY(minus, "minus");
	MAKE_KEY(home, "home");
	MAKE_KEY(one, "one");
	MAKE_KEY(two, "two");
	MAKE_KEY(x, "x");
	MAKE_KEY(y, "y");
	MAKE_KEY(z, "z");
	MAKE_KEY(tl, "tl");
	MAKE_KEY(tr, "tr");
	MAKE_KEY(zl, "zl");
	MAKE_KEY(zr, "zr");
	MAKE_KEY(thumbl, "thumbl");
	MAKE_KEY(thumbr, "thumbr");
	MAKE_KEY(accel, "accel");
	MAKE_KEY(ir, "ir");
	MAKE_KEY(balance_board, "balance_board");
	MAKE_KEY(motion_plus, "motion_plus");
	MAKE_KEY(pro_controller_key, "pro_controller_key");
	MAKE_KEY(pro_controller_move, "pro_controller_move");
	MAKE_KEY(classic_controller_key, "classic_controller_key");
	MAKE_KEY(classic_controller_move, "classic_controller_move");
	MAKE_KEY(watch, "watch");
	MAKE_KEY(nunchuk_key, "nunchuk_key");
	MAKE_KEY(nunchuk_move, "nunchuk_move");
	MAKE_KEY(drums_key, "drums_key");
	MAKE_KEY(drums_move, "drums_move");
	MAKE_KEY(guitar_key, "guitar_key");
	MAKE_KEY(guitar_move, "guitar_move");
	MAKE_KEY(gone, "gone");
	MAKE_KEY(weights, "weights");
	MAKE_KEY(stick, "stick");
	MAKE_KEY(whammy, "whammy");
	MAKE_KEY(fretbar, "fretbar");
	MAKE_KEY(sources, "sources");
	MAKE_KEY(factor, "factor");
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
	int fd = xwii_monitor_get_fd(monitor, 0);
	Tcl_Channel	channel;

	CHECK_ARGS(0, "");

	channel = Tcl_MakeFileChannel((ClientData)fd, TCL_READABLE);
	Tcl_RegisterChannel(interp, channel);

	Tcl_SetObjResult(interp, Tcl_NewStringObj(Tcl_GetChannelName(channel), -1));
	return TCL_OK;
}

//>>>
static int monitor_method_read(struct xwii_monitor* monitor, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //<<<
{
	Tcl_Obj*	res = Tcl_NewObj();
	char*		devname;

	CHECK_ARGS(0, "");

	while ((devname = xwii_monitor_poll(monitor)) != NULL) {
		TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewStringObj(devname, -1)));
	}

	Tcl_SetObjResult(interp, res);
	return TCL_OK;
}

//>>>
static int monitor_cmd(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //<<<
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

	return (methodtable[method])(monitor, interp, objc-1, objv+1);
}

//>>>
static void monitor_release(ClientData cdata) //<<<
{
	// TODO: close open handles?
	struct xwii_monitor*	monitor = (struct xwii_monitor*)cdata;
	xwii_monitor_unref(monitor);
	monitor = NULL;
}

//>>>
static int glue_monitor(ClientData c, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //<<<
{
	Tcl_Obj*				cmd;
	static int				cmdseq=1;
	int						poll=1, direct=0;
	struct xwii_monitor*	monitor;

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
static int iface_method_get_chan(struct xwii_iface* dev, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //<<<
{
	int fd = xwii_iface_get_fd(dev);
	Tcl_Channel	channel;

	CHECK_ARGS(0, "");

	channel = Tcl_MakeFileChannel((ClientData)fd, TCL_READABLE);
	Tcl_RegisterChannel(interp, channel);

	Tcl_SetObjResult(interp, Tcl_NewStringObj(Tcl_GetChannelName(channel), -1));
	return TCL_OK;
}

//>>>
static int iface_method_watch(struct xwii_iface* dev, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //<<<
{
	int		watch, res;

	CHECK_ARGS(1, "report_hotplug");

	TEST_OK(Tcl_GetBooleanFromObj(interp, objv[1], &watch));

	res = xwii_iface_watch(dev, watch);
	if (res != 0)
		THROW_ERROR("Error setting watch state");

	return TCL_OK;
}

//>>>
static int get_iface_bitmask_from_obj(Tcl_Interp* interp, Tcl_Obj* obj, unsigned int* bitmask) //<<<
{
	int				iface, i, oc;
	Tcl_Obj**		ov;
	const char* iface_names[] = {
		"CORE",
		"ACCEL",
		"IR",
		"MOTION_PLUS",
		"NUNCHUK",
		"CLASSIC_CONTROLLER",
		"BALANCE_BOARD",
		"PRO_CONTROLLER",
		"ALL",
		"WRITABLE",
		NULL
	};
	int ifaces[] = {
		XWII_IFACE_CORE,
		XWII_IFACE_ACCEL,
		XWII_IFACE_IR,
		XWII_IFACE_MOTION_PLUS,
		XWII_IFACE_NUNCHUK,
		XWII_IFACE_CLASSIC_CONTROLLER,
		XWII_IFACE_BALANCE_BOARD,
		XWII_IFACE_PRO_CONTROLLER,
		XWII_IFACE_ALL,
		XWII_IFACE_WRITABLE
	};

	TEST_OK(Tcl_ListObjGetElements(interp, obj, &oc, &ov));

	*bitmask = 0;
	for (i=1; i<oc; i++) {
		TEST_OK(Tcl_GetIndexFromObj(interp, ov[i], iface_names, "iface", TCL_EXACT, &iface));
		*bitmask |= ifaces[iface];
	}

	return TCL_OK;
}

//>>>
static int iface_method_open(struct xwii_iface* dev, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //<<<
{
	unsigned int	iface_bitmask;
	int				res;

	CHECK_ARGS(1, "ifaces");

	TEST_OK(get_iface_bitmask_from_obj(interp, objv[1], &iface_bitmask));

	res = xwii_iface_open(dev, iface_bitmask);
	if (res != 0)
		THROW_ERROR("Error opening (some) interfaces");

	return TCL_OK;
}

//>>>
static int iface_method_close(struct xwii_iface* dev, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //<<<
{
	unsigned int	iface_bitmask;

	CHECK_ARGS(1, "ifaces");

	TEST_OK(get_iface_bitmask_from_obj(interp, objv[1], &iface_bitmask));

	xwii_iface_open(dev, iface_bitmask);

	return TCL_OK;
}

//>>>
static int iface_bitmask_to_obj(Tcl_Interp* interp, unsigned int bitmask, Tcl_Obj** res) //<<<
{
	struct static_keys* keys = Tcl_GetThreadData(&thread_static_keys, sizeof(*keys));
	*res = Tcl_NewObj();

	if (bitmask & XWII_IFACE_CORE)
		TEST_OK(Tcl_ListObjAppendElement(interp, *res, keys->CORE));
	if (bitmask & XWII_IFACE_ACCEL)
		TEST_OK(Tcl_ListObjAppendElement(interp, *res, keys->ACCEL));
	if (bitmask & XWII_IFACE_IR)
		TEST_OK(Tcl_ListObjAppendElement(interp, *res, keys->IR));
	if (bitmask & XWII_IFACE_MOTION_PLUS)
		TEST_OK(Tcl_ListObjAppendElement(interp, *res, keys->MOTION_PLUS));
	if (bitmask & XWII_IFACE_NUNCHUK)
		TEST_OK(Tcl_ListObjAppendElement(interp, *res, keys->NUNCHUK));
	if (bitmask & XWII_IFACE_CLASSIC_CONTROLLER)
		TEST_OK(Tcl_ListObjAppendElement(interp, *res, keys->CLASSIC_CONTROLLER));
	if (bitmask & XWII_IFACE_BALANCE_BOARD)
		TEST_OK(Tcl_ListObjAppendElement(interp, *res, keys->BALANCE_BOARD));
	if (bitmask & XWII_IFACE_PRO_CONTROLLER)
		TEST_OK(Tcl_ListObjAppendElement(interp, *res, keys->PRO_CONTROLLER));
	if (bitmask & XWII_IFACE_WRITABLE)
		TEST_OK(Tcl_ListObjAppendElement(interp, *res, keys->WRITABLE));

	return TCL_OK;
}

//>>>
static int iface_method_opened(struct xwii_iface* dev, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //<<<
{
	Tcl_Obj*	res;

	CHECK_ARGS(0, "");

	TEST_OK(iface_bitmask_to_obj(interp, xwii_iface_opened(dev), &res));
	Tcl_SetObjResult(interp, res);
	return TCL_OK;
}

//>>>
static int iface_method_available(struct xwii_iface* dev, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //<<<
{
	Tcl_Obj*	res;

	CHECK_ARGS(0, "");

	TEST_OK(iface_bitmask_to_obj(interp, xwii_iface_available(dev), &res));
	Tcl_SetObjResult(interp, res);
	return TCL_OK;
}

//>>>
Tcl_Obj* keycode_to_obj(unsigned int code) //<<<
{
	struct static_keys* keys = Tcl_GetThreadData(&thread_static_keys, sizeof(*keys));

	switch (code) {
		case XWII_KEY_LEFT:		return keys->left;
		case XWII_KEY_RIGHT:	return keys->right;
		case XWII_KEY_UP:		return keys->up;
		case XWII_KEY_DOWN:		return keys->down;
		case XWII_KEY_A:		return keys->a;
		case XWII_KEY_B:		return keys->b;
		case XWII_KEY_PLUS:		return keys->plus;
		case XWII_KEY_MINUS:	return keys->minus;
		case XWII_KEY_HOME:		return keys->home;
		case XWII_KEY_ONE:		return keys->one;
		case XWII_KEY_TWO:		return keys->two;
		case XWII_KEY_X:		return keys->x;
		case XWII_KEY_Y:		return keys->y;
		case XWII_KEY_TL:		return keys->tl;
		case XWII_KEY_TR:		return keys->tr;
		case XWII_KEY_ZL:		return keys->zl;
		case XWII_KEY_ZR:		return keys->zr;
		case XWII_KEY_THUMBL:	return keys->thumbl;
		case XWII_KEY_THUMBR:	return keys->thumbr;
	}
	return keys->unknown;
}

//>>>
static int ev_key(Tcl_Interp* interp, Tcl_Obj** ev_dict, struct xwii_event* ev) //<<<
{
	struct static_keys* keys = Tcl_GetThreadData(&thread_static_keys, sizeof(*keys));

	switch (ev->v.key.state) {
		case 0:
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->state, keys->up));
			break;
		case 1:
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->state, keys->down));
			break;
		case 2:
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->state, keys->repeat));
			break;
		default:
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->state, keys->unknown));
			break;
	}
	TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->code, keycode_to_obj(ev->v.key.code)));

	return TCL_OK;
}

//>>>
static int construct_event(Tcl_Interp* interp, struct xwii_event* ev, Tcl_Obj** ev_dict) //<<<
{
	struct static_keys* keys = Tcl_GetThreadData(&thread_static_keys, sizeof(*keys));

	TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->sec, Tcl_NewIntObj(ev->time.tv_sec)));
	TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->usec, Tcl_NewIntObj(ev->time.tv_usec)));

	switch (ev->type) {
		case XWII_EVENT_KEY: //<<<
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->type, keys->key));
			TEST_OK(ev_key(interp, ev_dict, ev));
			break;
			//>>>
		case XWII_EVENT_ACCEL: //<<<
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->type, keys->accel));
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->x, Tcl_NewIntObj(ev->v.abs[0].x)));
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->y, Tcl_NewIntObj(ev->v.abs[1].y)));
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->z, Tcl_NewIntObj(ev->v.abs[2].z)));
			break;
			//>>>
		case XWII_EVENT_IR: //<<<
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->type, keys->ir));
			{
				int	i;
				Tcl_Obj*	sources = Tcl_NewObj();
				Tcl_Obj*	source;
				for (i=0; i<4; i++) {
					source = Tcl_NewObj();
					if (xwii_event_ir_is_valid(&ev->v.abs[i])) {
						TEST_OK(Tcl_ListObjAppendElement(interp, source, Tcl_NewIntObj(ev->v.abs[i].x)));
						TEST_OK(Tcl_ListObjAppendElement(interp, source, Tcl_NewIntObj(ev->v.abs[i].y)));
					}
					TEST_OK(Tcl_ListObjAppendElement(interp, sources, source));
				}
				TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->sources, sources));
			}
			break;
			//>>>
		case XWII_EVENT_BALANCE_BOARD: //<<<
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->type, keys->balance_board));
			{
				Tcl_Obj*	weights = Tcl_NewObj();
				TEST_OK(Tcl_ListObjAppendElement(interp, weights, Tcl_NewIntObj(ev->v.abs[0].x)));
				TEST_OK(Tcl_ListObjAppendElement(interp, weights, Tcl_NewIntObj(ev->v.abs[1].x)));
				TEST_OK(Tcl_ListObjAppendElement(interp, weights, Tcl_NewIntObj(ev->v.abs[2].x)));
				TEST_OK(Tcl_ListObjAppendElement(interp, weights, Tcl_NewIntObj(ev->v.abs[3].x)));
				TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->weights, weights));
			}
			break;
			//>>>
		case XWII_EVENT_MOTION_PLUS: //<<<
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->type, keys->motion_plus));
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->x, Tcl_NewIntObj(ev->v.abs[0].x)));
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->y, Tcl_NewIntObj(ev->v.abs[1].y)));
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->z, Tcl_NewIntObj(ev->v.abs[2].z)));
			break;
			//>>>
		case XWII_EVENT_PRO_CONTROLLER_KEY: //<<<
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->type, keys->pro_controller_key));
			TEST_OK(ev_key(interp, ev_dict, ev));
			break;
			//>>>
		case XWII_EVENT_PRO_CONTROLLER_MOVE: //<<<
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->type, keys->pro_controller_move));
			{
				Tcl_Obj*	pos = Tcl_NewObj();
				TEST_OK(Tcl_DictObjPut(interp, pos, keys->x, Tcl_NewIntObj(ev->v.abs[0].x)));
				TEST_OK(Tcl_DictObjPut(interp, pos, keys->y, Tcl_NewIntObj(ev->v.abs[0].y)));
				TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->left, pos));

				pos = Tcl_NewObj();
				TEST_OK(Tcl_DictObjPut(interp, pos, keys->x, Tcl_NewIntObj(ev->v.abs[1].x)));
				TEST_OK(Tcl_DictObjPut(interp, pos, keys->y, Tcl_NewIntObj(ev->v.abs[1].y)));
				TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->right, pos));
			}
			break;
			//>>>
		case XWII_EVENT_WATCH: //<<<
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->type, keys->watch));
			break;
			//>>>
		case XWII_EVENT_CLASSIC_CONTROLLER_KEY: //<<<
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->type, keys->classic_controller_key));
			TEST_OK(ev_key(interp, ev_dict, ev));
			break;
			//>>>
		case XWII_EVENT_CLASSIC_CONTROLLER_MOVE: //<<<
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->type, keys->classic_controller_move));
			{
				Tcl_Obj*	pos = Tcl_NewObj();
				TEST_OK(Tcl_DictObjPut(interp, pos, keys->x, Tcl_NewIntObj(ev->v.abs[0].x)));
				TEST_OK(Tcl_DictObjPut(interp, pos, keys->y, Tcl_NewIntObj(ev->v.abs[0].y)));
				TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->left, pos));

				pos = Tcl_NewObj();
				TEST_OK(Tcl_DictObjPut(interp, pos, keys->x, Tcl_NewIntObj(ev->v.abs[1].x)));
				TEST_OK(Tcl_DictObjPut(interp, pos, keys->y, Tcl_NewIntObj(ev->v.abs[1].y)));
				TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->right, pos));

				TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->tl, Tcl_NewIntObj(ev->v.abs[2].x)));
				TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->tr, Tcl_NewIntObj(ev->v.abs[2].y)));
			}
			break;
			//>>>
		case XWII_EVENT_NUNCHUK_KEY: //<<<
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->type, keys->nunchuk_key));
			TEST_OK(ev_key(interp, ev_dict, ev));
			break;
			//>>>
		case XWII_EVENT_NUNCHUK_MOVE: //<<<
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->type, keys->nunchuk_move));
			{
				Tcl_Obj* pos = Tcl_NewObj();
				TEST_OK(Tcl_DictObjPut(interp, pos, keys->x, Tcl_NewIntObj(ev->v.abs[0].x)));
				TEST_OK(Tcl_DictObjPut(interp, pos, keys->y, Tcl_NewIntObj(ev->v.abs[0].y)));
				TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->stick, pos));

				pos = Tcl_NewObj();
				TEST_OK(Tcl_DictObjPut(interp, pos, keys->x, Tcl_NewIntObj(ev->v.abs[1].x)));
				TEST_OK(Tcl_DictObjPut(interp, pos, keys->y, Tcl_NewIntObj(ev->v.abs[1].y)));
				TEST_OK(Tcl_DictObjPut(interp, pos, keys->z, Tcl_NewIntObj(ev->v.abs[1].z)));
				TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->accel, pos));
			}
			break;
			//>>>
		case XWII_EVENT_DRUMS_KEY: //<<<
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->type, keys->drums_key));
			TEST_OK(ev_key(interp, ev_dict, ev));
			break;
			//>>>
		case XWII_EVENT_DRUMS_MOVE:
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->type, keys->drums_move));
			// TODO
			break;

		case XWII_EVENT_GUITAR_KEY: //<<<
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->type, keys->guitar_key));
			TEST_OK(ev_key(interp, ev_dict, ev));
			break;
			//>>>
		case XWII_EVENT_GUITAR_MOVE: //<<<
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->type, keys->guitar_move));
			{
				Tcl_Obj*	t = Tcl_NewObj();

				TEST_OK(Tcl_DictObjPut(interp, t, keys->x, Tcl_NewIntObj(ev->v.abs[0].x)));
				TEST_OK(Tcl_DictObjPut(interp, t, keys->y, Tcl_NewIntObj(ev->v.abs[0].y)));
				TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->stick, t));

				TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->whammy, Tcl_NewIntObj(ev->v.abs[1].x)));
				TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->fretbar, Tcl_NewIntObj(ev->v.abs[1].x)));
			}
			break;
			//>>>
		case XWII_EVENT_GONE: //<<<
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->type, keys->gone));
			break;
			//>>>
		default:
			TEST_OK(Tcl_DictObjPut(interp, *ev_dict, keys->type, keys->unknown));
			break;
	}
	return TCL_OK;
}

//>>>
static int iface_method_dispatch(struct xwii_iface* dev, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //<<<
{
	Tcl_Obj*			res;
	Tcl_Obj*			ev_dict;
	struct xwii_event	ev;
	int					r;

	CHECK_ARGS(0, "");

	res = Tcl_NewObj();
	while ((r = xwii_iface_dispatch(dev, &ev, sizeof(ev))) == 0) {
		ev_dict = Tcl_NewObj();
		TEST_OK(construct_event(interp, &ev, &ev_dict));
		TEST_OK(Tcl_ListObjAppendElement(interp, res, ev_dict));
	}
	if (r != -EAGAIN)
		THROW_ERROR("Error while reading events");

	Tcl_SetObjResult(interp, res);
	return TCL_OK;
}

//>>>
static int iface_method_rumble(struct xwii_iface* dev, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //<<<
{
	int		on, res;

	CHECK_ARGS(1, "on");

	TEST_OK(Tcl_GetBooleanFromObj(interp, objv[1], &on));

	res = xwii_iface_rumble(dev, on);
	if (res != 0)
		THROW_ERROR("Error setting rumble");

	return TCL_ERROR;
}

//>>>
static int iface_method_get_led(struct xwii_iface* dev, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //<<<
{
	int		res, led;
	bool	state;

	CHECK_ARGS(1, "led");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &led));
	if (led < 1 || led > 4)
		THROW_ERROR("led must be in the range [1, 4]");

	res = xwii_iface_get_led(dev, led, &state);
	if (res != 0)
		THROW_ERROR("Error getting led state");

	Tcl_SetObjResult(interp, Tcl_NewBooleanObj(state));
	return TCL_OK;
}

//>>>
static int iface_method_set_led(struct xwii_iface* dev, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //<<<
{
	int		res, led, state;

	CHECK_ARGS(2, "led state");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &led));
	TEST_OK(Tcl_GetBooleanFromObj(interp, objv[2], &state));

	if (led < 1 || led > 4)
		THROW_ERROR("led must be in the range [1, 4]");

	res = xwii_iface_set_led(dev, led, state);
	if (res != 0)
		THROW_ERROR("Error setting led state");

	return TCL_OK;
}

//>>>
static int iface_method_get_battery(struct xwii_iface* dev, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //<<<
{
	int		res;
	uint8_t	capacity;

	CHECK_ARGS(0, "");


	res = xwii_iface_get_battery(dev, &capacity);
	if (res != 0)
		THROW_ERROR("Error getting battery state");

	Tcl_SetObjResult(interp, Tcl_NewIntObj(capacity));
	return TCL_OK;
}

//>>>
static int iface_method_get_devtype(struct xwii_iface* dev, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //<<<
{
	char*		devtype;
	int			res;

	CHECK_ARGS(0, "");

	res = xwii_iface_get_devtype(dev, &devtype);
	if (res != 0)
		THROW_ERROR("Error getting devtype");

	Tcl_SetObjResult(interp, Tcl_NewStringObj(devtype, -1));
	free(devtype);

	return TCL_OK;
}

//>>>
static int iface_method_get_extension(struct xwii_iface* dev, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //<<<
{
	char*		extension;
	int			res;

	CHECK_ARGS(0, "");

	res = xwii_iface_get_extension(dev, &extension);
	if (res != 0)
		THROW_ERROR("Error getting extension");

	Tcl_SetObjResult(interp, Tcl_NewStringObj(extension, -1));
	free(extension);

	return TCL_OK;
}

//>>>
static int iface_method_set_mp_normalization(struct xwii_iface* dev, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //<<<
{
	int32_t	x, y, z, factor;

	CHECK_ARGS(4, "x y z factor");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &x));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &y));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &z));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[4], &factor));

	xwii_iface_set_mp_normalization(dev, x, y, z, factor);

	return TCL_OK;
}

//>>>
static int iface_method_get_mp_normalization(struct xwii_iface* dev, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //<<<
{
	struct static_keys* keys = Tcl_GetThreadData(&thread_static_keys, sizeof(*keys));
	int32_t		x, y, z, factor;
	Tcl_Obj*	res;

	CHECK_ARGS(0, "");

	xwii_iface_get_mp_normalization(dev, &x, &y, &z, &factor);

	res = Tcl_NewObj();

	TEST_OK(Tcl_DictObjPut(interp, res, keys->x, Tcl_NewIntObj(x)));
	TEST_OK(Tcl_DictObjPut(interp, res, keys->y, Tcl_NewIntObj(y)));
	TEST_OK(Tcl_DictObjPut(interp, res, keys->z, Tcl_NewIntObj(z)));
	TEST_OK(Tcl_DictObjPut(interp, res, keys->factor, Tcl_NewIntObj(factor)));

	Tcl_SetObjResult(interp, res);
	return TCL_OK;
}

//>>>
static int iface_cmd(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //<<<
{
	struct xwii_iface*	dev = (struct xwii_iface*)cdata;

	int		method;
	const char* methods[] = {
		"get_chan",
		"watch",
		"open",
		"close",
		"opened",
		"available",
		"dispatch",
		"rumble",
		"get_led",
		"set_led",
		"get_battery",
		"get_devtype",
		"get_extension",
		"set_mp_normalization",
		"get_mp_normalization",
		NULL
	};
	iface_method_handler* methodtable[] = {
		iface_method_get_chan,
		iface_method_watch,
		iface_method_open,
		iface_method_close,
		iface_method_opened,
		iface_method_available,
		iface_method_dispatch,
		iface_method_rumble,
		iface_method_get_led,
		iface_method_set_led,
		iface_method_get_battery,
		iface_method_get_devtype,
		iface_method_get_extension,
		iface_method_set_mp_normalization,
		iface_method_get_mp_normalization
	};

	if (objc < 2)
		CHECK_ARGS(1, "method ?args ...?");

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[1], methods, "method", TCL_EXACT,
				&method));

	return (methodtable[method])(dev, interp, objc-1, objv+1);
}

//>>>
static void iface_release(ClientData cdata) //<<<
{
	struct xwii_iface*	dev = (struct xwii_iface*)cdata;
	xwii_iface_unref(dev);
	dev = NULL;
}

//>>>
static int glue_iface(ClientData c, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //<<<
{
	Tcl_Obj*			cmd;
	static int			cmdseq=1;
	int					res;
	struct xwii_iface*	dev;

	CHECK_ARGS(1, "syspath");

	res = xwii_iface_new(&dev, Tcl_GetString(objv[1]));
	if (res != 0)
		THROW_ERROR("Error creating interface");

	cmd = Tcl_ObjPrintf("xwii_iface%d", cmdseq++);
	Tcl_CreateObjCommand(interp, Tcl_GetString(cmd),
			iface_cmd, dev, iface_release);
	Tcl_SetObjResult(interp, cmd);

	return TCL_OK;
}

//>>>
int Xwiimote_Init(Tcl_Interp* interp) //<<<
{
	struct static_keys*	keys;

	if (Tcl_InitStubs(interp, "8.6", 0) == NULL) return TCL_ERROR;

	keys = Tcl_GetThreadData(&thread_static_keys, sizeof(*keys));
	if (!keys->initialized)
		init_static_keys(keys);

	NEW_CMD(NS "monitor", glue_monitor);
	NEW_CMD(NS "iface", glue_iface);

	NEW_CMD(NS "_testmode", glue_testmode);

	TEST_OK(Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION));

	return TCL_OK;
}

//>>>

// vim: foldmethod=marker foldmarker=<<<,>>> ts=4 shiftwidth=4
