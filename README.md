# Source lines of code counter for Windows and Linux
Counts lines of code recursively in a directory, only works on windows and linux, it cannot be configured to count a file, only directories. Also it will not count lines that are empty or only have spaces. You can configure it to show the lines of code in individual files with the -s option, you can also configure it to not count hidden files with -h option, and you can configure it to only count files with certain extensions with -ext option and listing the extension names after it, without the dot. Note that all args after -ext are considered to be file extensions so it -s and the directory name shall go before it.

On windows? You're in luck! Prebuilt executable should be in releases, if I remember, but that is a big if.
On ubuntu? You're in luck! Prebuilt executable is in releases.
