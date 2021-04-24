# s3fs-fuse-rc4
Forked from s3fs-fuse, s3fs-fuse-rc4 uses rc4 encryption/decryption uploaded and downloaded from AWS S3 buckets. Developed on Arch Linux.
## Features

* large subset of POSIX including reading/writing files, directories, symlinks, mode, uid/gid, and extended attributes
* compatible with Amazon S3, and other [S3-based object stores](https://github.com/s3fs-fuse/s3fs-fuse/wiki/Non-Amazon-S3)
* allows random writes and appends
* large files via multi-part upload
* renames via server-side copy
* optional server-side encryption
* data integrity via MD5 hashes
* in-memory metadata caching
* local disk data caching
* user-specified regions, including Amazon GovCloud
* authenticate via v2 or v4 signatures

## Installation

* Arch Linux:

  ```
  git clone https://github.com/delinquent313/s3fs-fuse-rc4
  cd s3fs-fuse-rc4 
  makepkg -i
  ```
Be sure to set the key file named .rc4Key in the home directory or it will use a default password for encryption/decryption. 

  ```
  echo "RC4KEY" > ~/.rc4Key
  ```


## Examples

s3fs supports the standard
[AWS credentials file](https://docs.aws.amazon.com/cli/latest/userguide/cli-config-files.html)
stored in `${HOME}/.aws/credentials`.  Alternatively, s3fs supports a custom passwd file.

The default location for the s3fs password file can be created:

* using a `.passwd-s3fs` file in the users home directory (i.e. `${HOME}/.passwd-s3fs`)
* using the system-wide `/etc/passwd-s3fs` file

Enter your credentials in a file `${HOME}/.passwd-s3fs` and set
owner-only permissions:

```
echo ACCESS_KEY_ID:SECRET_ACCESS_KEY > ${HOME}/.passwd-s3fs
chmod 600 ${HOME}/.passwd-s3fs
```

Run s3fs with an existing bucket `mybucket` and directory `/path/to/mountpoint`:

```
s3fs mybucket /path/to/mountpoint -o passwd_file=${HOME}/.passwd-s3fs
```

If you encounter any errors, enable debug output:

```
s3fs mybucket /path/to/mountpoint -o passwd_file=${HOME}/.passwd-s3fs -o dbglevel=info -f -o curldbg
```

You can also mount on boot by entering the following line to `/etc/fstab`:

```
mybucket /path/to/mountpoint fuse.s3fs _netdev,allow_other 0 0
```

or

```
mybucket /path/to/mountpoint fuse.s3fs _netdev,allow_other 0 0
```

If you use s3fs with a non-Amazon S3 implementation, specify the URL and path-style requests:

```
s3fs mybucket /path/to/mountpoint -o passwd_file=${HOME}/.passwd-s3fs -o url=https://url.to.s3/ -o use_path_request_style
```

or(fstab)

```
mybucket /path/to/mountpoint fuse.s3fs _netdev,allow_other,use_path_request_style,url=https://url.to.s3/ 0 0
```

Note: You may also want to create the global credential file first

```
echo ACCESS_KEY_ID:SECRET_ACCESS_KEY > /etc/passwd-s3fs
chmod 600 /etc/passwd-s3fs
```

Note2: You may also need to make sure `netfs` service is start on boot

## Limitations

Generally S3 cannot offer the same performance or semantics as a local file system.  More specifically:

* random writes or appends to files require rewriting the entire object, optimized with multi-part upload copy
* metadata operations such as listing directories have poor performance due to network latency
* non-AWS providers may have [eventual consistency](https://en.wikipedia.org/wiki/Eventual_consistency) so reads can temporarily yield stale data (AWS offers read-after-write consistency [since Dec 2020](https://aws.amazon.com/about-aws/whats-new/2020/12/amazon-s3-now-delivers-strong-read-after-write-consistency-automatically-for-all-applications/))
* no atomic renames of files or directories
* no coordination between multiple clients mounting the same bucket
* no hard links
* inotify detects only local modifications, not external ones by other clients or tools

## References

* [goofys](https://github.com/kahing/goofys) - similar to s3fs but has better performance and less POSIX compatibility
* [s3backer](https://github.com/archiecobbs/s3backer) - mount an S3 bucket as a single file
* [S3Proxy](https://github.com/gaul/s3proxy) - combine with s3fs to mount Backblaze B2, EMC Atmos, Microsoft Azure, and OpenStack Swift buckets
* [s3ql](https://github.com/s3ql/s3ql/) - similar to s3fs but uses its own object format
* [YAS3FS](https://github.com/danilop/yas3fs) - similar to s3fs but uses SNS to allow multiple clients to mount a bucket

## Frequently Asked Questions

* [FAQ wiki page](https://github.com/s3fs-fuse/s3fs-fuse/wiki/FAQ)
* [s3fs on Stack Overflow](https://stackoverflow.com/questions/tagged/s3fs)
* [s3fs on Server Fault](https://serverfault.com/questions/tagged/s3fs)

## License

Copyright (C) 2010 Randy Rizun <rrizun@gmail.com>

Licensed under the GNU GPL version 2

