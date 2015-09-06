# fbfs

- cmd that creates filesystem and mount it. 
- umount(close) the file. 

- what data we would put in filesystem. 
	In header: 
	- name 
	- block size 
	- filesystem size 
	- root directory 

	In Node Type: 
	- directory node
	- file node

	In each directory node: 
	- list of files. 
	- list of directories 
	- max num of files and directories. 

	In each sub dir, 
	- data of sub directory should be created in separated block. 
	- need to define numbering mechanism for directories and files. 


