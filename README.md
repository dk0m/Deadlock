
# Deadlock

A PoC For Bypassing Browser File Locks Without Process Termination.

## How It Works

Deadlock enumerates all instances of a process, Enumerates the private handle table of each respective instance, Filters only for file handles that are of disk type after duplication, Searches for a specific file with a tag, If the file is found, Deadlock unlocks it by remotely calling NtClose on it and optionally dumps the file as well. 

## Showcase

[Showcase](https://streamable.com/e/i8ojzt)
