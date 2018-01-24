#!/bin/bash
#
filename="$1"
SCRIPT="uname -s; uname -r;"

while read -u10 line
do
    name=($line)
    count=$( ping -c 1 ${name[0]} | grep icmp* | wc -l )
    if [ $count -eq 0 ]  	
    then
         
    	echo "Host is not Alive! Try again later.."

   else

	ssh -l ${name[1]} ${name[0]} exit
        #sshpass -p '${name[2]}'	ssh -o ${name[1]}@${name[0]}  exit
	#sleep .5
	
	if [ $? -eq 0 ]
	then
		#sleep .5
		ssh -l  ${name[1]} ${name[0]}  "${SCRIPT}" > ${name[0]}.txt
		#sshpass -p '${name[2]}' ssh -o StrictHostKeyChecking=no ${name[1]}@${name[0]}  "${SCRIPT}" > ${name[0]}.txt
		#sleep 5

	else
		./winexe //${name[0]}  -U ${name[1]}%${name[2]} 'systeminfo'|grep -w "OS Name:">${name[0]}.txt
		./winexe //${name[0]}  -U ${name[1]}%${name[2]} 'systeminfo'|grep -w "OS Version:">>${name[0]}.txt

	fi
    fi	
done 10< "$filename"
