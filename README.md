
# Deadlock

A PoC For Bypassing Browser File Locks Without Process Termination.

## How It Works

Deadlock enumerates all instances of a process, Enumerates the private handle table of each respective instance, Filters only for file handles that are of disk type after duplication, Searches for a specific file with a tag, If the file is found, Deadlock unlocks it by remotely calling NtClose on it and optionally dumps the file as well.

In this example, We unlock the Cookies file of the Chrome browser, But you can make it work with any browser.

## Usage In Malware

> **Warning**
I'm not responsible for whatever you do with this software.
 
This could be utilized by malware authors (specifically stealer developers) to remove locks on files they are interested in (Cookies, Web Data, etc) without making too much noise.

