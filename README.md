# lrctag

This is a commandline application for storing song lyrics into
music, via both tagging and files. It supports both plain-text
song lyrics, as well as synchronized lyrics.

It currently supports embedding lyrics for:

* VorbisComment tags (FLAC, Ogg, etc)
* ID3v2 tags (MP3)
* APE tags (APE, WavPack, TrueAudio, others)
* MP4 tags

## Operation

By default, it accepts a list of filename. For each file, it
will:

* Determine the audio file type (FLAC, Ogg, etc).
* Check for missing unsynchronized and synchronized lyrics,
both in the audio file's tags, as well as in text and LRC
files.
* Try to load missing lyrics from txt and LRC files first,
then reach out to [LRCLIB](https://lrclib.net).
* Save found lyrics to tags and files - without overwriting
any existing tags or files.

It does this in a multi-threaded manner, to speed up processing
large file collections.

It also has options to:

* Disable handling synchronized or unsynchornized lyrics completely.
* Enable and disable different lyrics sources and ordering.
* Enable and disable different lyrics destinations (tags and files).
* Enable overwriting files.
* Recursively scan folders for audio files.
* Specify what file extensions are audio files.
* Specify which field names to use for lyrics.
* Generate a CSV to stdout, listing audio files and their lyrics tags/files status.

This allows you to do things like - say you have an existing
collection of lyrics files you'd like to embed into audio files,
and you specifically just want to use your lyrics files (and not
use LRCLIB).

```
lrctag \
  --recursive \
  --source file \
  --destination tag \
  /path/to/folder
```

Or - say you have the opposite, you have a collection of audio
files with embedded lyrics that you'd like to put into files,
and overwrite any existing files:

```
lrctag \
  --recursive \
  --source tag \
  --destination file \
  --overwrite-files \
  /path/to/folder
```

Maybe your music player expects FLAC files to store synchronized lyrics in
a field named `SYNCEDLYRICS` - you can specify that's where you want those
lyrics saved:

```
lrctag
  --vorbiscomment-synchronized SYNCEDLYRICS
  /path/to/file.flac
```

## File-specific details

### FLAC

FLAC files will store lyrics in Vorbis Comments. If any ID3v1 or ID3v2 tags are
found in a FLAC file, they will be stripped.

If a FLAC file does not have existing Vorbis Comments, they will be created,
and populated using existing ID3v2 or ID3v1 tags if available.

You can disable stripping ID3v1 and ID3v2 tags from FLAC files.

There are no options for storing lyrics in ID3v2 tags.

### MP3

MP3 files will store lyrics in ID3v2 tags. If any ID3v1 or APE tags are found
in an MP3 file, they will be stripped.

If an MP3 file does not have an existing ID3v2 tag, it will be created, and populated
with data from the existing APE or ID3v1 tag.

You can disable stripping ID3v1 and APE tags.

### APE

APE files will store lyrics in APE tags.

If no APE tags are found, they will be added. If ID3v1 tags are found
instead of APE tags, they will be converted into APE tags and stripped.

You can disable stripping ID3v1 tags from APE files.

### WavPack

WavPack files will store lyrics in APE tags.

If no APE tags are found, they will be added. If ID3v1 tags are found
instead of APE tags, they will be converted into APE tags and stripped.

You can disable stripping ID3v1 tags from WavPack files.

### Musepack

Musepack files will store lyrics in APE tags.

If no APE tags are found, they will be added. If ID3v1 tags are found
instead of APE tags, they will be converted into APE tags and stripped.
Additionally, any ID3v2 tags will be stripped.

### TrueAudio

TrueAudio allows either ID3v2 or APE tags.

By default, TrueAudio files will use APE tags. If an APE tag is not found,
it will be created, and populated with data from any existing ID3v2 or ID3v1 tags.
ID3v2 and ID3v1 tags will be stripped.

Using the `--trueaudio-id3v2` flag will change the behavior to use ID3v2 tags.
If an ID3v2 tag does not exist, it will be created, and populated with data
from any existing APE or ID3v1 tag. APE and ID3v1 tags will be stripped.

### Ogg Vorbis, Opus, Speex, FLAC

Anything in an Ogg file will have lyrics stored in Vorbis Comments.

## Tag-specific details

### ID3v2

ID3v2 is likely to be the most compatible tagging format -
there are well-defined ID3v2 frames for storing unsynchronized
and synchronized lyrics (`USLT` and `SYLT` frames, specifically).

### VorbisComment and APE

VorbisComment and APE tags both rely on storing LRC-formatted
synchronized lyrics in a field. You can customize which field
name is used.

By default, VorbisComment will store synchronized lyrics
in field labeld `LYRICS`, and unsynchronized lyrics in a
field labeled `UNSYNCEDLYRICS`.

APE tags will store synchronized lyrics in a field labeled
`SyncedLyrics`, and unsynchronized lyrics in a field labeled
`Lyrics`.

### MP4

MP4 only has a single field for storing lyrics, by default
this is used for unsynchronized lyrics. This can be changed
to store synchronized lyrics instead.

## Options and flags

The full list of options is:

### General Options

* **`--no-synchronized-lyrics`** - disable all processing of synchronized lyrics.
* **`--no-unsynchronized-lyrics`** - disable all processing of unsynchronized lyrics.
* **`--recursive`** - iterate through subfolders to find audio files.
* **`--extension <ext>`** - classify a file extension as an audio file, can be
specified multiple times for multiple extensions.
* **`--source <type>`** - enable a lyrics source, can be specified multiple times
for multiple sources. Sources will be tried in the order specified. Defaults to **`file`**, then **`lrclib`**. Available sources:
    * **`file`** - read from txt and lrc files.
    * **`lrclib`** - fetch lyrics from [LRCLIB](https://lrclib.net/)
    * **`tag`** - read from embedded tags.
* **`--destination <type>`** - enable a lyrics destination, can be specified multiple times
for multiple destinations. Defaults to **`file`** and **`tag`**. Available destinations:
    * **`tag`** - write to embedded tags.
    * **`file`** - write txt and lrc files.
* **`--overwrite-tags`** - enable overwriting existing lyrics tags.
* **`--overwrite-files`** - enable overwriting existing lyrics files.
* **`--threads <number>`** - create `<number>` threads for processing. Defaults to auto-detecting cores, or a single thread if that fails.
* **`--report`** - enable the reporting mode.
    * This will generate a CSV to stdout that lists:
        * File path
        * Synchronized lyrics tag status
        * Unsynchronized lyrics tag status
        * Synchronized lyrics file status
        * Unsynchronized lyrics file status
    * The status fields will be one of:
        * "Yes" - the lyrics tag/file exists and is valid.
        * "No" - the lyrics tag/file does not exist, or is not valid.
        * "NA" - only appears in the tag fields if the audio format doesn't support the tag.
        * "Unknown" - the audio file is unsupported so, no way to know.
    * This will cause the following options to be ignored, because they're not applicable to report mode:
        * `--no-synchronized-lyrics`
        * `--no-unsynchronized-lyrics`
        * `--source`
        * `--destination`
        * `--overwrite-tags`
        * `--overwrite-files`
    * All files will be open read-only.

### FLAC file options

* **`--flac-strip-id3v1`** - strip any ID3v1 tags found in FLAC files (default).
* **`--no-flac-strip-id3v1`** - do not strip any ID3v1 tags found in FLAC files.
* **`--flac-strip-id3v2`** - strip any ID3v2 tags found in FLAC files (default).
* **`--no-flac-strip-id3v2`** - do not strip any ID3v2 tags found in FLAC files.

### MP3 file options

* **`--mp3-strip-id3v1`** - strip any ID3v1 tags found in MP3 files (default).
* **`--no-mp3-strip-id3v1`** - do not strip any ID3v1 tags found in MP3 files.
* **`--mp3-strip-ape`** - strip any APE tags found in MP3 files (default).
* **`--no-mp3-strip-ape`** - do not strip any APE tags found in MP3 files.

### TrueAudio file options

* **`--trueaudio-ape`** - use APE tags instead of ID3v2 tags, strips ID3v2 tags.
* **`--trueaudio-id3v2`** - use ID3v2 tags instead of APE tags, strips APE tags.
* **`--trueaudio-strip-id3v1`** - strip any ID3v1 tags found in TrueAudio Files (default).
* **`--no-trueaudio-strip-id3v1`** - do not strip any ID3v1 tags found in TrueAudio Files.

### APE file options

* **`--ape-strip-id3v1`** - strip any ID3v1 tags found in APE files (default).
* **`--no-ape-strip-id3v1`** - do not strip any ID3v1 tags found in APE files.

### WavPack file options

* **`--ape-strip-id3v1`** - strip any ID3v1 tags found in WavPack files (default).
* **`--no-ape-strip-id3v1`** - do not strip any ID3v1 tags found in WavPack files.

### Vorbis Comment tag options

* **`--vorbiscomment-synchronized <name>`** - specify the field to use
when saving synchronized lyrics to Vorbis Comments (default: **`LYRICS`**).
* **`--no-vorbiscomment-synchronized`** - disable saving synchronized lyrics
to Vorbis Comments.
* **`--vorbiscomment-unsynchronized <name>`** - specify the field to use
when saving unsynchronized lyrics to Vorbis Comments (default: **`UNSYNCEDLYRICS`**).
* **`--no-vorbiscomment-unsynchronized`** - disable saving unsynchronized lyrics
to Vorbis Comments.

### APE tag options

* **`--ape-synchronized <field>`** - specify the field to use
when saving synchronized lyrics to APE tags (default: **`SyncedLyrics`**).
* **`--no-ape-synchronized`** - disable saving synchronized lyrics
to APE tags.
* **`--ape-unsynchronized <field>`** - specify the field to use
when saving unsynchronized lyrics to APE tags (default: **`Lyrics`**).
* **`--no-ape-unsynchronized`** - disable saving unsynchronized lyrics
to APE tags.

### MP4 tag options

* **`--mp4-unsynchronized`** - store plain-text, unsynchronized lyrics in the `©lyr` tag (default).
* **`--mp4-synchronized`** - store LRC-formatted, synchronized lyrics in the `©lyr` tag.

### ID3v2 tag options

* **`--id3v2.4`** - save ID3v2.4 tags (default).
* **`--id3v2.3`** - save ID3v2.3 tags.

## Dependencies

* [TagLib](https://taglib.org/) version 1.12 or greater.
* [argparse](https://github.com/p-ranav/argparse)
* [spdlog](https://github.com/gabime/spdlog)
* [ICU4C](https://unicode-org.github.io/icu/userguide/icu4c/)
* [libcurl](https://curl.se/libcurl/)
* [JSON for Modern C++](https://json.nlohmann.me/) (aka "nlohmann JSON")
* [CMake](https://cmake.org/) for building.

I do not provide distro-specific instructions for installing packaged
dependencies.

## Building

Use cmake.

I do not provide step-by-step instructions for building the program,
flags for specifying how to find libraries, etc.

## License

GPLv3 (see `COPYING`).

