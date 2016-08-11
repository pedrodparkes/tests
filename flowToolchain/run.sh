#/bin/sh

# check if video file is given
if [ "$#" -lt 1 ]; then
	echo no input file given, exiting
	exit
fi

# check if video file exists
if [ ! -f $1 ]; then
	echo $1 doesn\'t exist
	exit
fi

###
### 	Variables
### 

BLACK=$(tput setaf 0)
RED=$(tput setaf 1)
GREEN=$(tput setaf 2)
YELLOW=$(tput setaf 3)
LIME_YELLOW=$(tput setaf 190)
POWDER_BLUE=$(tput setaf 153)
BLUE=$(tput setaf 4)
MAGENTA=$(tput setaf 5)
CYAN=$(tput setaf 6)
WHITE=$(tput setaf 7)
BRIGHT=$(tput bold)
NORMAL=$(tput sgr0)
BLINK=$(tput blink)
REVERSE=$(tput smso)
UNDERLINE=$(tput smul)

function bright()
{
	echo ${BRIGHT}$1${NORMAL}
}
function dir()
{
	echo ${BRIGHT}$1${NORMAL}
}
function file()
{
	echo ${WHITE}$1${NORMAL}
}

file=$1
filefull=$(basename "$file")
extension=$([[ "$filefull" = *.* ]] && echo "${filefull##*.}" || echo '')
filename="${filefull%.*}"

# check if directory name given
if [ "$#" -lt 2 ]; then
	echo no directory name given, using $(dir $filename)
	dirname=$filename
else
	dirname=$2
fi

# flow directory
flowdir="$dirname""_flow"
echo flow directory: $(dir $flowdir)

###
###	Frames
###

# file name format
frameformat=$dirname/frame-%06d.png
#framemask=$dirname/frame-00000[12].png
framemask=$dirname/frame-*.png

count=-1
# check if directory exists
if [ ! -d "$dirname" ]; then 
	# directory doesn't exist, create one
	echo creating $(dir $dirname) directory for frames
	mkdir "$dirname"
else
	echo directory $(dir $dirname) already exists
	count=`ls 2>/dev/null -Ub1 -- $framemask | wc -l`
fi

# getting separate images
if [ $count -le 0 ]; then
	# no content to interfere with, proceed
	echo input type is $(fiel $extension)
	echo extracting $(file $filefull) frames into $(dir $dirname) directory
	printf "\n\n-----------------------------------------------------------------\n"
#	ffmpeg -i $file -f image2 -s 512x288 $frameformat
	ffmpeg -i $file -f image2 -s 256x144 $frameformat
	printf "\n-----------------------------------------------------------------\n\n"
	count=`ls 2>/dev/null -Ub1 -- $framemask | wc -l`
else
	# some files already exist, skip this step
	echo there are files satisfiing $(file $framemask), skiping frames extraction
fi

################
#count=30
################

###
###	Flow files
###

# flow file name format
#flowformat=$flowdir/flow-%06d.flo		# for binary flow file
#flowmask=$flowdir/flow-*.flo
flowformat=$flowdir/flow-%06d.png		# for image representation
flowmask=$flowdir/flow-*.png

flowcount=-1
# check if flow directory exists
if [ ! -d "$flowdir" ]; then
	# flow directory doesnt exit
	echo creating $(dir $flowdir) directory for flow files
	mkdir "$flowdir"
else
	echo flow directory $(dir $flowdir) already exists
	flowcount=`ls 2>/dev/null -Ub1 -- $flowmask | wc -l`
#	`ls $flowmask | wc -l`
fi

if [ $flowcount -le 0 ]; then
	# no flow files, generate
	echo generating flow files

	echo $count
	# iterate through frame files
	prev=`printf $frameformat 1`
	for ((i=1;i<count;i++)); do
		next=`printf $frameformat $((i+1))`
		flowfile=`printf $flowformat $i`
		dif=$(diff -q $prev $next)
#		if [ "$dif" != "" ]; then
			echo -ne generate flow from $(file $prev) to $(file $next) "->" $(file $flowfile) \($i/$count\)\\r 
	#		sleep 0.05
			./tvl1/tvl1flow $prev $next $flowfile
#		fi
		prev=$next
	done
	flowcount=`ls 2>/dev/null -Ub1 -- $flowmask | wc -l`
	echo 
else
	# flow files already exist, skip
	echo there are files satisfiing $(file $flowmask), skiping flow computation
fi

###
###	Flow images
###


