#!/usr/bin/env tclsh

package require xwiimote

set monitor	[xwii::monitor]
proc read_wiimotes {} {
	global monitor
	foreach name [$monitor read] {
		puts "New wiimote: $name"
	}
}

chan event [$monitor get_chan] readable read_wiimotes
read_wiimotes

vwait forever
