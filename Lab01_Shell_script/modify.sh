#! /bin/bash


#determine if recursive choice, non-recursive one or help


case $1 in    	
"-r")


	#if recursive, check whether uppercase or lowercase
	case $2 in
	"-l"|"-L") # lowercase conversion

	# if next argument is valid
	if [ -n "$3" ]; then
		if [ -d "$3" ];then	
				for f in $3/*;do
					if [ -f $f ];then		
						#compute new name
						new_name="$(dirname "$f")/$(basename "$f" | tr '[A-Z]' '[a-z]')"
						#rename file with new_name
						mv -T "$f" "${new_name}"
						echo "Changed name: $new_name"
					fi

				done	
			#elif the argument item is a file
		else
			if [ -f "$3" ];then		
					#compute new name
					new_name="$(dirname "$3")/$(basename "$3" | tr '[A-Z]' '[a-z]')"
					#rename file with new_name
					mv -T "$3" "${new_name}"
					echo "Changed name: $new_name"
			else
					echo "Error, no file "$3" found"
			fi
				

		fi	
	./$0 -r -l ${@:4}
	fi
	
	
	;;

	"-u"|"-U") # uppercase conversion
	# if next argument is valid
	if [ -n "$3" ]; then

		if [ -d "$3" ];then	
				for f in $3/*;do
					if [ -f $f ];then		
						#compute new name
						new_name="$(dirname "$f")/$(basename "$f" | tr '[a-z]' '[A-Z]')"
						#rename file with new_name
						mv -T "$f" "${new_name}"
						echo "Changed name: $new_name"
					fi

				done	
			#elif the argument item is a file
		else
			if [ -f "$3" ];then		
					#compute new name
					new_name="$(dirname "$3")/$(basename "$3" | tr '[a-z]' '[A-Z]')"
					#rename file with new_name
					mv -T "$3" "${new_name}"
					echo "Changed name: $new_name"
			else
					echo "Error, no file "$3" found"
			fi
				

		fi	
	./$0 -r -u ${@:4}
	fi
	;;

	*)
	# I assume sed pattern was given
	

	# if next argument is valid
	if [ -n "$3" ]; then

		if [ -d "$3" ];then	
				for f in $3/*;do
					if [ -f $f ];then		
						#convert filename using sed pattern			
						filename=$(basename "$f" | sed "$2" )
						#compute new name, by changing basename to all lowercase
						new_name="$(dirname "$f")/$filename"
						#replace the name of the file
						mv -T "$f" "${new_name}"
						echo "Changed name: $new_name"
						#call modify recursively but erasing the first argument that has been already converted (which would be $3)
					fi

				done	
			#elif the argument item is a file
		else
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
				

		fi	
	./$0 -r $2 ${@:4}
	fi

	;;
	

	esac

;;

"-l"|"-L") # lowercase non-recursive conversion

	# while there are still files to convert
	while [ -n "$2" ]; do
		if [ -d "$2" ];then	
				for f in $2/*;do
					if [ -f $f ];then		
						#compute new name
						new_name="$(dirname "$f")/$(basename "$f" | tr '[A-Z]' '[a-z]')"
						#rename file with new_name
						mv -T "$f" "${new_name}"
						echo "Changed name: $new_name"
					fi

				done	
			#elif the argument item is a file
		else
			if [ -f "$2" ];then		
					#compute new name
					new_name="$(dirname "$2")/$(basename "$2" | tr '[A-Z]' '[a-z]')"
					#rename file with new_name
					mv -T "$2" "${new_name}"
					echo "Changed name: $new_name"
			else
					echo "Error, no file "$2" found"
			fi
				

		fi	
	shift
	done
;;

"-u"|"-U") # uppercase non-recursive conversion

	# while there are still files to convert
	while [ -n "$2" ]; do
		if [ -d "$2" ];then	
				for f in $2/*;do
					if [ -f $f ];then		
						#compute new name
						new_name="$(dirname "$f")/$(basename "$f" | tr '[a-z]' '[A-Z]')"
						#rename file with new_name
						mv -T "$f" "${new_name}"
						echo "Changed name: $new_name"
					fi

				done	
			#elif the argument item is a file
		else
			if [ -f "$2" ];then		
					#compute new name
					new_name="$(dirname "$2")/$(basename "$2" | tr '[a-z]' '[A-Z]')"
					#rename file with new_name
					mv -T "$2" "${new_name}"
					echo "Changed name: $new_name"
			else
					echo "Error, no file "$2" found"
			fi
				

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


# if argument 3 exists
#	if [ -n "$3" ];then	
		# if argument 3 is a valid file
#		if [ -f "$3" ];then
				#convert filename using sed pattern			
#				filename=$(basename "$3" | sed "$2" )
				#compute new name, by changing basename to all lowercase
#				new_name="$(dirname "$3")/$filename"
				#replace the name of the file
#				mv -T "$3" "${new_name}"
#				echo "Changed name: $new_name"
				#call modify recursively but erasing the first argument that has been already converted (which would be $3)
#			else
#			echo "Error, no file "$3" found"
#		fi
#		./$0 -r $2 ${@:4}
#	fi
	#---------------------------------------
