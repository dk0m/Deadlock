
# Deadlock

A PoC For Bypassing Browser File Locks Without Process Termination.

## How It Works

Deadlock enumerates all instances of a process, Enumerates the private handle table of each respective instance, Filters only for file handles that are of disk type after duplication, Searches for a specific file with a tag, If the file is found, Deadlock unlocks it by remotely calling NtClose on it and optionally dumps the file as well. 

## Showcase

<div style="position:relative; width:100%; height:0px; padding-bottom:56.250%"><iframe allow="fullscreen" allowfullscreen height="100%" src="https://streamable.com/e/i8ojzt?" width="100%" style="border:none; width:100%; height:100%; position:absolute; left:0px; top:0px; overflow:hidden;"></iframe></div>
