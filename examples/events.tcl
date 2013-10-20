#!/usr/bin/env tclsh

package require xwiimote

set monitor	[xwii::monitor]

proc iface_readable name {
	global ifaces
	set iface	[dict get $ifaces $name]

	foreach event [$iface dispatch] {
		puts "$name ev: ($event)"
		switch -- [dict get $event type] {
			watch {
				puts "Available: [$iface available]"
			}
		}
	}
}

set ifaces	{}
proc read_wiimotes {} {
	global monitor ifaces
	foreach name [$monitor read] {
		puts "New wiimote: $name"
		set iface	[xwii::iface $name]
		dict set ifaces $name $iface
		$iface watch 1

		set available	[$iface available]
		puts "Available interfaces: $available"

		for {set i 1} {$i < 5} {incr i} {
			puts "led $i: [$iface get_led $i]"
		}

		$iface set_led 4 [expr {[$iface get_led 4] ? "off" : "on"}]

		puts "Battery level: [$iface get_battery] %"

		#puts "devtype: \"[$iface get_devtype]\""
		puts "extension: \"[$iface get_extension]\""

		#$iface rumble 1
		#after 1000 [list $iface rumble 0]

		try {
			#[dict get $ifaces $name] open [lmap e {ALL} {
			#	expr {$e in $available ? $e : [continue]}
			#}]
			$iface open {ALL}
		} on error {errmsg options} {
			puts $errmsg
		}
		puts "Opened: [$iface opened]"
		chan event [$iface get_chan] readable \
			[list iface_readable $name]
	}
}

chan event [$monitor get_chan] readable read_wiimotes
read_wiimotes

vwait forever
