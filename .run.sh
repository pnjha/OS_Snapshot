#!/bin/bash

CC=g++ 
EXEC=classfileName
SRC=sourcefileName
# prints the input
function print_my_input() {
  make run
}

function checkC() {  
	#make run
	echo "aaa"
	if [ "a" == "a" ]
	then
		echo "aaa1"
	fi
		
}

function startScheduler(){
	echo "startScheduler"
	isError=`echo 0`

	if [ "$isError" == "0" ]
	then
		EXEC=`echo customScheduler`
		SRC=`echo scheduler.cpp -pthread -lcrypto`
		#if [ "$SRC" -nt "$EXEC" ]
		#then
			echo "Rebuliding Scheduler Class File"
			$CC -o $EXEC $SRC
		#fi
		RESULT=`./$EXEC`
		#do process in background 
		#RESULT=`./$EXEC & disown`
	fi			
}

function createSnapShot() {  
	echo "createSnapShot"
	isError=`echo 0`

	if [ "$#" -lt "2" ]
	then
		echo "Improper Command. Please verify input Parameters"
		echo "createSnapShot sourceDirectoryPath destinationDirectoryPath"
		isError=`echo -1`
	fi	
	#echo $isError
	if [ "$isError" == "0" ]
	then
		EXEC=`echo Main`
		SRC=`echo Main.cpp -lcrypto`
		echo "$SRC"
		#if [ "$SRC" -nt "$EXEC" ]
		#then
			echo "Rebuliding Class File"
			$CC -o $EXEC $SRC 
		#fi
		RESULT=`./$EXEC createSnapShot $1 $2`
		#echo $RESULT
		if [ "$RESULT" = 110 ]
		then
			echo "Successfully Instered into SnapShot File"
		elif [ "$RESULT" = 130 ]
		then
			echo "Already snapshot of this directory is getting Created"
		fi
		
	fi		
}

function restoreSnapShot() {  
	echo "restoreSnapShot"	
	isError=`echo 0`
	echo "restoreSnapShot"	
	if [ "$#" -lt "1" ]
	then
		echo "Improper Command. Please verify input Parameters"
		echo "sample : restoreSnapShot sourceDirectoryPath"
		isError=`echo -1`
	fi	
	#echo $isError
	# if [ "$isError" == "0" ]
	# then
		EXEC=`echo restoreSnapShot`
		SRC=`echo restoreSnapShot.cpp -lcrypto`
		#if [ "$SRC" -nt "$EXEC" ]
		#then
			echo "Rebuliding Restore Snapshot file Class File Need to Comment Out"
			$CC -o $EXEC $SRC
		#fi
		RESULT=`./$EXEC $1`
		if [ "$?" == "110" ]
		then
			echo "Successfully Instered into SnapShot File"
		fi
		
	# sfi
	
		
}

function removeSnapShot() {  
	echo "removeSnapShot"
	isError=`echo 0`

	if [ "$#" -lt "1" ]
	then
		echo "Improper Command. Please verify input Parameters"
		echo "sample : removeSnapShot sourceDirectoryPath"
		isError=`echo -1`
	fi	
	#echo $isError
	if [ "$isError" == "0" ]
	then
		EXEC=`echo Main`
		SRC=`echo Main.cpp`
		if [ "$SRC" -nt "$EXEC" ]
		then
			echo "Rebuliding Class File Again"
			$CC -o $EXEC $SRC
		fi
		RESULT=`./$EXEC stopSnapshotScheduler $1`
		if [ "$RESULT" = 140 ]
		then
			echo "User is trying to remove non-existed Snapshot"
		elif [ "$RESULT" = 120 ]
		then
			echo "Successfully removed the SnapShot"
		fi
		
	fi	
}

function stopScheduler() {  
	echo "stopScheduler"
	isError=`echo 0`
	if [ "$isError" == "0" ]
	then
		PIDOFSCHEDULER=`pgrep customScheduler`
		#PIDOFSCHEDULER=`pgrep firefox`
		if [ "$PIDOFSCHEDULER" = "" ]
		then
			echo "Scheduler is not running"
		fi
		if [ "$PIDOFSCHEDULER" > 0 ]
		then
			RESULT=`kill $PIDOFSCHEDULER`
			echo "Process terminated"
		fi
		
	fi	
}

#runOnStart
