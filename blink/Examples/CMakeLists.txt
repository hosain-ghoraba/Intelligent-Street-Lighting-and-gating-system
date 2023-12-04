add_executable(sweep SweepExample.c)

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(sweep PRIVATE pico_stdlib hardware_pwm pico_servo hardware_adc)

# create map/bin/hex file etc.
pico_add_extra_outputs(sweep)

# add url via pico_set_program_url
example_auto_set_url(sweep)



add_executable(TwoServoSweep TwoServoSweepExample.c)

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(TwoServoSweep PRIVATE pico_stdlib hardware_pwm pico_servo hardware_adc)

 
# create map/bin/hex file etc.
pico_add_extra_outputs(TwoServoSweep)

# add url via pico_set_program_url
example_auto_set_url(TwoServoSweep)



add_executable(ServoObject ServoObjectExample.cpp)

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(ServoObject PRIVATE pico_stdlib hardware_pwm pico_servo_object hardware_adc)
# Enable usb output, disable uart output
pico_enable_stdio_usb(ServoObject 1)
pico_enable_stdio_uart(ServoObject 0)
 

# create map/bin/hex file etc.
pico_add_extra_outputs(ServoObject)

# add url via pico_set_program_url
example_auto_set_url(ServoObject)

