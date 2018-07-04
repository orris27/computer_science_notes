#! /bin/sh

src=/backup
cmd=/usr/local/bin/inotifywait
passwordfile=/etc/rsync.password
user=rsync_backup
host1=172.16.55.136
dst=shared

if [ -z "$src" ]
	then
	 echo "error"
	 exit 9
fi


"$cmd" -mrq -e create,close_write,delete,attrib --format "%w%f" "$src" | \
while read line
do 
	cd "$src" && /bin/rsync -aruz -R --delete ./ --timeout=100 "$user"@"$host1"::"$dst" --password-file="$passwordfile" > /dev/null 2>&1
done

