#!/bin/bash

# Automatic driver initialization

create_driver()
{
    # check if driver with same name already exists
     if [[ -f "./src/drivers/$1.cpp" || -f  "./include/drivers/$1.h" ]];then
        echo "Driver with name: \"$1\" already exists."
    else    
        # TODO: add string manipulation => add code snippets in line_protocol.proto, main.pp: action_handler/ registration_handler    
    
        # add <new_driver>.cpp to src/drivers
        sed "s/template_driver/$1/" ./tools/driver_init/templates/template_driver.cpp > ./src/drivers/$1.cpp
        # add <new_driver>.h to src/drivers
        sed "s/template_driver/$1/" ./tools/driver_init/templates/template_driver.h > ./src/drivers/$1.h

        #DRIVER_NAME= echo ${driver_name}| tr '[a-z]' '[A-Z]' 
        echo "Created driver: \"$1\""
    fi
}


if [ -z "$1" ];then
    echo "Error: no argument set. Please set the name of the new driver"
else 
    # confirmation 
    while true; do
        read -p "Are you sure you want to create the template driver: \"$1\"? [y/n]" yn
        case $yn in
            [Yy]* ) echo;create_driver $1; break;;
            [Nn]* ) exit;;
            * ) echo "Please answer yes or no.";;
        esac
    done
fi
