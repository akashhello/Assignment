[root@nitesh-rmm02 ~]# zpool status
  pool: rwzpool
 state: ONLINE
status: Some supported features are not enabled on the pool. The pool can
        still be used, but some features are unavailable.
action: Enable all features using 'zpool upgrade'. Once this is done,
        the pool may no longer be accessible by software that does not support
        the features. See zpool-features(5) for details.
  scan: none requested
config:

        NAME        STATE     READ WRITE CKSUM
        rwzpool     ONLINE       0     0     0
          sdb       ONLINE       0     0     0

errors: No known data errors

[sparse using DD]
/mnt/sparse-file on /mnt/MyDisk type ext4 (rw,loop=/dev/loop2)

[root@nitesh-rmm02 mnt]# dd of=sparse-file bs=1k seek=102400 count=0

[root@nitesh-rmm02 mnt]# mkfs.ext4 sparse-file

[root@nitesh-rmm02 mnt]# mount -o loop  sparse-file /mnt/MyDisk/

/mnt/sparse-file on /mnt/MyDisk type ext4 (rw,loop=/dev/loop2)

[Act as device]
Disk /dev/loop2: 104 MB, 104857600 bytes
255 heads, 63 sectors/track, 12 cylinders
Units = cylinders of 16065 * 512 = 8225280 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disk identifier: 0x00000000

[steps to install ZFS]
https://michaelwiki.geekgalaxy.com/w/index.php/ZFS_on_Linux

