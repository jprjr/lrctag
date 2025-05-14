# lrctag

This is a commandline application for storing song lyrics into
music, via both tagging and files. It supports both plain-text
song lyrics, as well as synchronized lyrics.

It currently supports embedding lyrics for:

* FLAC
* Ogg Opus
* Ogg Vorbis
* Ogg Speex
* Ogg FLAC

By default, it accepts a list of filename. For each file, it
will:

* Determine the audio file type (FLAC, Ogg, etc).
* Check for missing unsychronized and synchronized lyrics,
both in the audio file's tags, as well as in text and LRC
files.
* Try to load lyrics from txt and LRC files.
* Save found lyrics to tags and files - without overwriting
any existing tags or files.

It does this in a multi-threaded manner, to speed up processing
large file collections.

It also has options to:

* Disable handling sychronized or unsynchornized lyrics completely.
* Enable and disable different lyrics sources ([LRCLIB](https://lrclib.net/), tags and files).
* Enable and disable different lyrics destinations (tags and files).
* Enable overwriting files.
* Recursively scan folders for audio files.
* Specify what file extensions are audio files.
* Specify which tags to use for lyrics.

This allows you to do things like - say you have an existing
collection of lyrics files you'd like to embed into audio files:

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

## Options and flags

The full list of options is:

* **`--no-synchronized-lyrics`** - disable all processing of synchronized lyrics.
* **`--no-unsynchronized-lyrics`** - disable all processing of unsynchronized lyrics.
* **`--recursive`** - iterate through subfolders to find audio files.
* **`--extension <ext>`** - classify a file extension as an audio file, can be
specified multiple times for multiple extensions.
* **`--source <type>`** - enable a lyrics source, can be specified multiple times
for multiple sources. Defaults to **`file`**. Available sources:
    * **`lrclib`** - fetch lyrics from [LRCLIB](https://lrclib.net/)
    * **`tag`** - read from embedded tags.
    * **`file`** - read from txt and lrc files.
* **`--destination <type>`** - enable a lyrics destination, can be specified multiple times
for multiple destinations. Defaults to **`file`** and **`tag`**. Available destinations:
    * **`tag`** - write to embedded tags.
    * **`file`** - write txt and lrc files.
* **`--overwrite-tags`** - enable overwriting existing lyrics tags.
* **`--overwrite-files`** - enable overwriting existing lyrics files.
* **`--threads <number>`** - create `<number>` threads for processing. Defaults to auto-detecting cores, or a single thread if that fails.
* **`--vorbiscomment-synchronized-lyrics-tag <tagname>`** - specify the tag to use
when saving synchronized lyrics to files that use vorbis comments (FLAC, Ogg (Vorbis, Opus, Speex, FLAC)).
* **`--no-vorbiscomment-synchronized-lyrics`** - disable saving synchronized lyrics
to files that use vorbis comments.
* **`--vorbiscomment-unsynchronized-lyrics-tag <tagname>`** - specify the tag to use
when saving unsynchronized lyrics to files that use vorbis comments.
* **`--no-vorbiscomment-unsynchronized-lyrics`** - disable saving unsynchronized lyrics
to files that use vorbis comments.

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
as long as you have cmake you should be good-to-go.


## License

GPLv3 (see `COPYING`).

