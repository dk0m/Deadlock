
# Deadlock

A PoC For Bypassing Browser File Locks Without Process Termination.

## How It Works

Deadlock enumerates all instances of a process, Enumerates the private handle table of each respective instance, Filters only for file handles that are of disk type after duplication, Searches for a specific file with a tag, If the file is found, Deadlock unlocks it by remotely calling NtClose on it and optionally dumps the file as well.

## Usage
```
$ deadlock.exe <TARGET_PROCESS> <LOCKED_FILE_TAG> <OUTPUT_FILE> <CLOSE_REMOTE?>
```

## Examples
Dumping Chrome cookies:
```
deadlock.exe chrome.exe Cookies ./cookies.dmp 0
```

Dumping Chrome cookies & removeing the file lock:
```
deadlock.exe chrome.exe Cookies ./cookies.dmp 1
```

## Using This In Malware

This could be utilized by malware authors (specifically stealer developers) to remove locks on files they are interested in (Cookies, Web Data, etc) without making too much noise, While this is possible, I do NOT encourage you to use this for malicious purposes.

