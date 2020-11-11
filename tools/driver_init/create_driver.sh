#!/bin/bash

# Automatic driver initialization

create_driver()
{
    # check if driver with same name already exists
     if [[ -f "./src/drivers/$1.cpp" || -f  "./include/drivers/$1.h" ]];then
        echo "Driver with name: \"$1\" already exists."
    else    
        # string manipulation
        Driver_Name=$(echo $1 | sed 's/\(^[a-z]\|\_[a-z]\)/\U&/g')
        DRIVER_NAME=$(echo $1 | sed 's/[a-z]/\U&/g')
        DriverName=$(echo ${Driver_Name} | sed 's/\_//g')

        # add <new_driver>.cpp to src/drivers
        sed "s/template_driver/$1/g; s/Template_Driver/${Driver_Name}/g; s/TEMPLATE_DRIVER/${DRIVER_NAME}/g" ./tools/driver_init/templates/template_driver.cpp > ./src/drivers/$1.cpp
        # add <new_driver>.h to src/drivers
        sed "s/template_driver/$1/g; s/Template_Driver/${Driver_Name}/g; s/TEMPLATE_DRIVER/${DRIVER_NAME}/g" ./tools/driver_init/templates/template_driver.h > ./src/drivers/$1.h

        # Add code snippets in:

        # line_protocol.proto: Action message
        sed "s/Template_Driver/${Driver_Name}/g" ./tools/driver_init/templates/proto_action.txt > ./tools/driver_init/templates/memory.txt
        sed -n -i -e "/ADI-PROTO-Action/r ./tools/driver_init/templates/memory.txt" -e 1x -e '2,${x;p}' -e '${x;p}' ./protobuf/line_protocol.proto
        # line_protocol.proto: Registration message
        sed "s/Template_Driver/${Driver_Name}/g" ./tools/driver_init/templates/proto_reg.txt > ./tools/driver_init/templates/memory.txt
        sed -n -i -e "/ADI-PROTO-Reg/r ./tools/driver_init/templates/memory.txt" -e 1x -e '2,${x;p}' -e '${x;p}' ./protobuf/line_protocol.proto
        
        # line_protocol.proto: oneof declerations => only working if .h files inside driver dir are only for drivers
        num_of_files=$(ls ./src/drivers | egrep '\.h$' |wc -l)
        new_message_index=$(((num_of_files) + 1))
        # oneof decleration: Action
        sed "s/Template_Driver/${Driver_Name}/g; s/template_driver/$1/g; s/INDEX/${new_message_index}/g" ./tools/driver_init/templates/proto_oneof_action.txt > ./tools/driver_init/templates/memory.txt
        sed -n -i -e "/ADI-PROTO-Oneof-Action/r ./tools/driver_init/templates/memory.txt" -e 1x -e '2,${x;p}' -e '${x;p}' ./protobuf/line_protocol.proto
        # oneof decleration: Registration
        sed "s/Template_Driver/${Driver_Name}/g; s/template_driver/$1/g; s/INDEX/${new_message_index}/g" ./tools/driver_init/templates/proto_oneof_reg.txt > ./tools/driver_init/templates/memory.txt
        sed -n -i -e "/ADI-PROTO-Oneof-Reg/r ./tools/driver_init/templates/memory.txt" -e 1x -e '2,${x;p}' -e '${x;p}' ./protobuf/line_protocol.proto

        # main.cpp: action_handler
        sed "s/template_driver/$1/g" ./tools/driver_init/templates/main_action.txt > ./tools/driver_init/templates/memory.txt
        sed -n -i -e "/ADI-MAIN-Action/r ./tools/driver_init/templates/memory.txt" -e 1x -e '2,${x;p}' -e '${x;p}' ./src/main.cpp
        # main.cpp: registration_handler
        sed "s/template_driver/$1/g" ./tools/driver_init/templates/main_reg.txt > ./tools/driver_init/templates/memory.txt
        sed -n -i -e "/ADI-MAIN-Reg/r ./tools/driver_init/templates/memory.txt" -e 1x -e '2,${x;p}' -e '${x;p}' ./src/main.cpp

        # main.h: include driver
        sed "s/template_driver/$1/g" ./tools/driver_init/templates/main_include.txt > ./tools/driver_init/templates/memory.txt
        sed -n -i -e "/ADI-MAIN-Include/r ./tools/driver_init/templates/memory.txt" -e 1x -e '2,${x;p}' -e '${x;p}' ./src/main.h

        # simple_gateway.py: profile class for driver
        sed "s/template_driver/$1/g; s/TemplateDriver/${DriverName}/g" ./tools/driver_init/templates/py_profile.txt > ./tools/driver_init/templates/memory.txt
        sed -n -i -e "/ADI-PY-Profile/r ./tools/driver_init/templates/memory.txt" -e 1x -e '2,${x;p}' -e '${x;p}' ./examples/simple_gateway.py

        #

        echo "Created driver: \"$1\""
    fi
}


if [ -z "$1" ];then
    echo "Error: no argument set. Please set the name of the new driver"
else 
    # confirmation 
    while true; do
        read -p "Please save all open files first and answer with [y/n] to continue/cancel the initialization of: \"$1\"" yn
        case $yn in
            [Yy]* ) echo;create_driver $1; break;;
            [Nn]* ) exit;;
            * ) echo "Please answer yes or no.";;
        esac
    done
fi
