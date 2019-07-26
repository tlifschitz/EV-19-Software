# List all available programming hardware, and select the USB-Blaster.
# (Note: this example assumes only one USB-Blaster is connected.)
#set n [llength [get_hardware_names]]
#puts [format "| %d |" $n]
#if {$n == 0} { puts "No board connected!";exit}

# Get hardware name
foreach hardware_name [get_hardware_names] {
puts $hardware_name
if { [string match "USB-Blaster*" $hardware_name] } {
set usbblaster_name $hardware_name
}
}

# Get device name
foreach device_name [get_device_names -hardware_name $usbblaster_name] {
puts $device_name
if { [string match "@1*" $device_name] } {
set test_device $device_name
}
}

# Get number of editable memories
set memoryInstances [get_editable_mem_instances -hardware_name $usbblaster_name  -device_name $test_device]
set n [llength $memoryInstances]
puts [format "Number of editable memories: %d" $n]

#0: index, 1: depth, 2: width, 3: mode, 4: type, 5: name"
if { $n==2 } {
# Initiate a editing sequence
begin_memory_edit -hardware_name $usbblaster_name -device_name $test_device

foreach instance $memoryInstances {
puts "Updating [lindex $instance 5]"
update_content_to_memory_from_file -instance_index [lindex $instance 0] -mem_file_path "./[lindex $instance 5].mif" -mem_file_type mif
}

end_memory_edit
} else {
	puts "Number of editable memories differs from 2 (instrucion ROM, and data RAM). Check your design."
}


#${COMMAND} ${FLAGS} ${OUTPUT_FLAG} ${OUTPUT_PREFIX}${OUTPUT} && ${iHex2rviHex} 4096 ${OUTPUT_PREFIX}${OUTPUT} && ${COMMAND} -O ihex -j .data --reverse-bytes=4 "Mandelbrot.elf" "RAM.hex" && ${iHex2rviHex} 1024 "RAM.hex" && quartus_stp -t Programmer.tcl