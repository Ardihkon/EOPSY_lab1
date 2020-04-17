#! /bin/bash


#determine if recursive choice, help, or modify directory name


case $1 in    	
"-r")

	case $2 in
	"-l"|"-L")

	
	if [ -f "$3" ];then		
				new_name="$(dirname "$3")/$(basename "$3" | tr '[A-Z]' '[a-z]')"
				mv -T "$3" "${new_name}"
				echo "Changed name: $new_name"
				./$0 -r -l ${@:4}
	fi
	
	
	;;

	"-u"|"-U")

		if [ -f "$3" ];then		
				new_name="$(dirname "$3")/$(basename "$3" | tr '[a-z]' '[A-Z]')"
				mv -T "$3" "${new_name}"
				echo "Changed name: $new_name"
				./$0 -r -u ${@:4}
		fi	
	;;

	*)
	
		echo "Improper call to recursive modify!"
	;;
	

	esac

;;

"-l"|"-L")
	while [ -n "$2" ]; do
		if [ -f "$2" ];then		
			new_name="$(dirname "$2")/$(basename "$2" | tr '[A-Z]' '[a-z]')"
			mv -T "$2" "${new_name}"
			echo "Changed name: $new_name"
		else
			echo "Error, no file found"
		fi
	shift
	done
;;

"-u"|"-U")
	while [ -n "$2" ]; do
		if [ -f "$2" ];then		
			new_name="$(dirname "$2")/$(basename "$2" | tr '[a-z]' '[A-Z]')"
			mv -T "$2" "${new_name}"
			echo "Changed name: $new_name"
		else
			echo "Error, no file found"
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
;;

'')
#empty input - error
	echo "No arguments given to modify script!"
;; 


*)	
	echo "Improper arguments!!"
	
	
;;

esac



