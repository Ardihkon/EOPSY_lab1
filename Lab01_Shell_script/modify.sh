#! /bin/bash


#determine if recursive choice, non-recursive one or help


case $1 in    	
"-r")


	#if recursive, check whether uppercase or lowercase
	case $2 in
	"-l"|"-L") # lowercase conversion

	if [ -n "$3" ];then	
		#if such file exists...
		if [ -f "$3" ];then		
				#compute new name, by changing basename to all lowercase
				new_name="$(dirname "$3")/$(basename "$3" | tr '[A-Z]' '[a-z]')"
				#replace the name of the file
				mv -T "$3" "${new_name}"
				echo "Changed to name: $new_name"
				#call modify again shifting the arguments to left (but replacing -r and -l), so that only the list of files to change reduces by 1
			else
			echo "Error, no file "$3" found"	
		fi
	./$0 -r -l ${@:4}
	fi
	
	
	;;

	"-u"|"-U") # uppercase conversion
	#if there are still arguments
	if [ -n "$3" ];then	
		#if such file exists
		if [ -f "$3" ];then		
				#compute new name, by changing basename to all lowercase
				new_name="$(dirname "$3")/$(basename "$3" | tr '[a-z]' '[A-Z]')"
				#replace the name of the file
				mv -T "$3" "${new_name}"
				echo "Changed name: $new_name"
				#call modify again but erasing the first argument that has been already converted (which would be $3)
			else
			echo "Error, no file "$3" found"	
		fi	
		./$0 -r -u ${@:4}
	fi
	;;

	*)
	# I assume sed pattern was given
	# if argument 3 exists
	if [ -n "$3" ];then	
		# if argument 3 is a valid file
		if [ -f "$3" ];then
				#convert filename using sed pattern			
				filename=$(basename "$3" | sed "$2" )
				#compute new name, by changing basename to all lowercase
				new_name="$(dirname "$3")/$filename"
				#replace the name of the file
				mv -T "$3" "${new_name}"
				echo "Changed name: $new_name"
				#call modify recursively but erasing the first argument that has been already converted (which would be $3)
			else
			echo "Error, no file "$3" found"
		fi
		./$0 -r $2 ${@:4}
	fi


		
	;;
	

	esac

;;

"-l"|"-L") # lowercase non-recursive conversion

	# while there are still files to convert
	while [ -n "$2" ]; do
		# lif the argument item is a file
		if [ -f "$2" ];then		
			#compute new name
			new_name="$(dirname "$2")/$(basename "$2" | tr '[A-Z]' '[a-z]')"
			#rename file with new_name
			mv -T "$2" "${new_name}"
			echo "Changed name: $new_name"
		else
			echo "Error, no file "$2" found"
		fi
	shift
	done
;;

"-u"|"-U") # uppercase non-recursive conversion

	# while there are still files to convert
	while [ -n "$2" ]; do
		# if the argument item is a file
		if [ -f "$2" ];then	
			# compute new name
			new_name="$(dirname "$2")/$(basename "$2" | tr '[a-z]' '[A-Z]')"
			#rename file with new_name
			mv -T "$2" "${new_name}"
			echo "Changed name: $new_name"
		else
			echo echo "Error, no file "$2" found"
		fi
	shift
	done	
;;

"-h")
	#help tab

	echo 
	echo "Help"
	echo 

	echo "To modify filenames by lowercase (l) (or uppercase (u)) non-recursively, type in: "
	echo "modify  [-l|-u] <dir/file names...>"
	echo

	echo "To modify filenames by lowercase (l) (or uppercase (u)) recursively, type in: "
	echo "modify -r [-l|-u] <dir/file names...>"
	echo

	echo "To modify using sed recursively, type in: "
	echo "modify -r <sed pattern> <dir/file names...>"
	echo
;;

'')
#empty input - error
	echo "No arguments given to modify script!"
;; 


*)	
#all other cases - error
	echo "Improper arguments!"
;;

esac



