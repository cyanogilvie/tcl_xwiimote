#!/usr/bin/env tclsh

package require xwiimote
namespace path {::tcl::mathop}

set monitor	[xwii::monitor]

proc iface_readable name {
	global ifaces
	set iface	[dict get $ifaces $name]

	foreach event [$iface dispatch] {
		switch -- [dict get $event type] {
			watch {
				puts "Available: [$iface available]"
			}
			accel {
				set x	[* [dict get $event x] 0.0981]
				set y	[* [dict get $event y] 0.0981]
				set z	[* [dict get $event z] 0.0981]
				set x	[- $x 2.4]
				set y	[- $y 1.0]
				set z	[- $z 2.3]

				set mag	[expr {sqrt($x**2 + $y**2 + $z**2)}]
				puts [format {Accel: (x: %5.2f, y: %5.2f, z: %5.2f, mag: %5.2f) m/s} $x $y $z $mag]
			}
			ir {
				puts "IR: [join [lmap source [dict get $event sources] {
					if {[llength $source] > 0} {
						format "(%4d, %3d)" {*}$source
					} else {
						format ()
					}
				}]]"
			}
			default {
				puts "$name ev: ($event)"
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
			$iface open ALL
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
