#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <dir.h>
#include <iostream.h>
int get_arg_index(int argc, char* argv[], char* s) {
        for(int i = 0; i < argc; i++)
                if( !strcmp(argv[i], s) ) return i;
        return 0;
}

main(int argc, char* argv[]) {

        //z.exe *.php *.htm <null>

        char* usage = "USAGE: gettitle.exe old_ext new_ext [path]\n"
                        "  old_ext:     old extension, example: php\n"
                        "  new_ext:     new extension, example: htm\n"
                        "  path:        work path, default: current path\n";

        int fileCount = 0;
        char pattern[50] = "*.";
        char newname[50] = {0};

        if( get_arg_index(argc, argv, "-help") ||
        get_arg_index(argc, argv, "--help") ||
        get_arg_index(argc, argv, "/help") ||
        get_arg_index(argc, argv, "?") ||
        get_arg_index(argc, argv, "/?") ) {
                cout << usage;
                exit(0);
        }

         if(argc < 3) {
                cout << usage;
                exit(0);
        }

        if(argc > 3) {
                if( chdir(argv[3]) ) {
                        cout << "CHDIR ERROR: " << argv[3] << endl;
                        cout << usage;
                        exit(0);
                }
        }

        strcat(pattern, argv[1]);

	struct ffblk ffblk;
	int done;
	done = findfirst(pattern, &ffblk, 0);
	while(!done) {
                fileCount++;
                memset(newname, 0, 50);
                for(int i=0; i<strlen(ffblk.ff_name)-strlen(argv[1]); i++) {
                        newname[i] = ffblk.ff_name[i];
                }
                strcat(newname, argv[2]);
                cout << ffblk.ff_name << "\t->\t" << newname << endl;
                rename(ffblk.ff_name, newname);
                done = findnext(&ffblk);
	}
        cout << "total: " << fileCount << " files" << endl;

}
