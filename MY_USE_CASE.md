# WHAT I DO

I do not "subscribe" to music.  I buy CDs, and if CDs
aren't available, I buy DRM-Free digital copies in the
highest bitrate/quality that I can find.

When I buy the CD, I immediately import it into
iTunes (ALAC lossless format).  When I buy a digital,
I immediately import it into iTunes.

I started using iTunes back in 2008, when I bought
an iPod (these are long discontinued at this point,
with the hope that iPod users would "upgrade" to
an iPhone).

I no longer have a functional iPod, and an iPhone
with enough storage to hold all my music costs about
3 times what I paid for my Android that DOES have
enough storage.

# WHAT I NEED

I need something that can automatically copy my 
playlists out of iTunes to the various devices that I
actually play music from.  Two such devices are Android,
but I also use a Raspberry Pi running DLNA and an Emby
service on another Linux server.

For the Androids alone, I used to use some of the 
dedicated iTunes to Android apps (neither of which
actually function on my devices), but for my other
sources, I needed something with a little bit more
freedom.

To be sure, I have looked high and low for an
alternative to iTunes that can do playlists
with the same complexity as iTunes itself.  That is,
I use playlists that are formed of multiple
smart playlists with often complex rule-sets.

Ulimately, I couldn't find what I wanted, I wrote the
playlister tool.

# MY SETUP

At this point, despite having an up-to-date Apple, I
still run iTunes on a Windows machine for it's
ability to automatically export the `xml` file that
this program actually needs to run.  If that ever
goes away, I did happen to crack the file format
for Apple Music, but that's a LOT of work to
integrate a direct Apple Music database reader into
this tool, so I've not done it.

I have a scheduled Task on Windows that runs ROBOCOPY
of the entire iTunes folder structure to my NAS on
a nightly basis.  Then on my Linux server,
I run `playlister`
twice for each device I copy to (the extra for
playlists I want to randomize).
Finally, I use rsync to copy the `iTunes Media/Music`
folder and device specific playlists to my other
devices.

# FILES

## tablet.conf

Here, I'm pointing the configuration to my itunesxml,
and telling it to only copy songs into the playlist if
it can find them on the server (this prevents errors
on devices that like to error instead of just skip
missing media).  Finally, I'm telling playlister to
remove the windows destination from the song location
and then replace it with the destination where the 
music folder is copied to on the tablet.

```
$ cat tablet.conf 
itunesxml=/media/music/DS_Sync/iTunes/iTunes Music Library.xml
output_dir=/media/music/DS_Sync/Tablet
verify_dir=/media/music/DS_Sync/iTunes/iTunes Media
verify=y
randomize=no
format=extm3u
extension=m3u8
location_remove=C:\Users\gvollink\Music\iTunes\iTunes Media\
location_replace=/9898-C0ED/Music

[lists]
W4:Reception
```

This outputs `W4-Reception.m3u8`, which is an unsorted
playlist (exactly the songs we played at our wedding
reception, in the right order).

## Makefile

I use the UNIX development tool `make` to run playlister
(only when an input changes).

```makefile
ADD=""
ifdef QUIET
	ADD=-q -q
endif
ifdef VERBOSE
	ADD=-v
endif

all: tablet

force:
	@-rm -f tablet
	$(MAKE) all

tablet: /media/music/DS_Sync/iTunes/iTunes Music Library.xml tablet.conf Makefile
	@echo playlister $(ADD) -c tablet.conf
	@../bin/playlister $(ADD) -c tablet.conf
	@touch tablet

.PHONY: all force
```

## Cron...

First I wrote a very simple shell command

```shell
#!/bin/sh
cd /media/playlister/data
make QUIET=1
```

Then I created a crontab for it: /etc/cron.d/playlister

```cron
mple of job definition:
# .---------------- minute (0 - 59)
# |  .------------- hour (0 - 23)
# |  |  .---------- day of month (1 - 31)
# |  |  |  .------- month (1 - 12) OR jan,feb,mar,apr ...
# |  |  |  |  .---- day of week (0 - 6) (Sunday=0 or 7) OR sun,mon,tue,wed,thu,fri,sat
# |  |  |  |  |
# *  *  *  *  * user-name command to be executed
 15  *  *  *  * gvollink  /media/playlister/cronjob
```

So the cron runs hourly, and only updates if the XML
was updated by iTunes since the last time it ran.

# END

Honestly, the above is mostly documentation for myself.
