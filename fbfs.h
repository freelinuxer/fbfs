typedef struct fs_meta {
   int meta_type; /* meta data type: 0 - root dir */
   char name[100];
   char rootdir[10];
   int fs_size;
   int block_size;
} FS_META;

