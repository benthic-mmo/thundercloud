# thundercloud
a fork of the firestorm project, stripped down to be opensimulator specific, and with a UI rewrite 

# Getting Started
## Building On Debian 

In order to keep the project and its dependencies all in one place, you should use buildah to create an OCI container for it (this project is big!) 

Install buildah 

``` sudo apt install buildah ```

Create an empty debian container 

``` container=$(buildah from debian)``` 

Enter the container 

``` buildah run $container bash ``` 

create a source directory and clone thundercloud inside the container

``` mkdir ~/src ```

``` git clone https://github.com/unnamed-mmo/thundercloud.git```

follow these instructions (bring this to this repo as we change things)
https://wiki.firestormviewer.org/fs_compiling_firestorm_linux 

Mount the container's filesystem 


``` buildah unshare (this will make you root! ```

``` buildah mount $container```

go into where it was mounted (usually /var/lib/containers/storage/vfs/dir/...)

find the built viewer in thundercloud/build-linux-x86_64/newview
copy the packaged dir to your /home 

run

``` ./firestorm.sh ``` 

to verify build
