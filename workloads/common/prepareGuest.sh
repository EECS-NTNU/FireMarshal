#!/bin/bash

export PATH="$PATH:/usr/local/bin/"

INVOKE_DEFARGS="--prepare --compile"

if [ $# -lt 2 ]; then
	echo "$0 [target dir] [arguments to invoke.py...]" >&2
	exit 1
fi

if [ ! -d "$1" ]; then
	echo "Could not find $1" >&2
	exit 1
fi

targetDir=$(realpath "$1")
shift

target=$(basename "${targetDir}")
prepareDir="$(dirname "${targetDir}")/_prepare"

cd "${targetDir}" || exit 1
mkdir -p "${prepareDir}"/output

cp -lf "${targetDir}"/invoke.spec.json "${prepareDir}"/

echo "Prepare benchmarks and create runscript ${targetDir}/run.sh"
echo "invoke.py ${INVOKE_DEFARGS} $@ > ${prepareDir}/run.sh"
invoke.py ${INVOKE_DEFARGS} "$@" > "${prepareDir}"/run.sh
chmod +x "${prepareDir}"/run.sh

# Convert all symbolic links to hard links so we can safely delete stuff that is referenced
# echo "Convert symbolic links into hardlinks"
# find . -type l -xtype f -exec bash -c 'ln -fv "$(readlink -m "$0")" "$0"' {} \;

# Get every path that is in the runscript and copy it to a new location
# This way we only keep the data in the image that is required for this run
for p in $(grep -E '^  cd ".*"$' "${prepareDir}"/run.sh | sed 's/^  cd "//g' | sed 's/"$//g' | sort -n | uniq); do
	np="${prepareDir}/${p}"
	p="${targetDir}/${p}"	
	echo "Copy benchmark $(basename "$(dirname "$p")")/$(basename "$p") into image..."
	# Copy using dereferencing to resolve symbolic links and hardlink only to solve the space problem
	mkdir -p "$(dirname "${np}")" && cp -lRL "$p" "$np"
done

echo "Cleanup..."
# Remove the old benchmark suite and put our new one in place
rm -fR "${targetDir}"
mv "${prepareDir}" "${targetDir}"

# Report how big the filesystem is now
echo -n "Minimum filesystem size: " 
df -h | grep -E '/$' | awk '{print $3}'

poweroff
