#include <dir.h>
#include <strstrea.h>
#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>

enum { sz = 300 };

void removetitle(char*& buf, strstream& str) {
        extern int inTitle;
        char tmp[sz] = {0};
        strcpy(tmp, buf);
        strupr(tmp);
        char* p1 = strstr(tmp, "<TITLE>");
        char* p2 = strstr(tmp, "</TITLE>");
        if(p1 && p2 && p1 < p2) {
                if(p1 == tmp && p2 == tmp + strlen(tmp) - 8 ) {
                        //cout << "titleline" << endl;
                        return;
                }
                if(inTitle) {
                        inTitle = 0;
                        p1 = buf + (p2-tmp) + 8;
                        removetitle(p1, str);
                        return;
                }
                else {
                        *(buf + (p1-tmp)) = 0;
                        str << buf;
                        p1 = buf + (p2-tmp) + 8;
                        removetitle(p1, str);
                        return;
                }
        }

        if(p1 && p2 && p1 > p2 ) {
                if(inTitle) {
                        inTitle = 0;
                        p1 = buf + (p2-tmp) + 8;
                        removetitle(p1, str);
                        return;
                }
                else {
                        *(buf + (p2-tmp)) = 0;
                        str << buf;
                        p1 = buf + (p2-tmp) + 8;
                        removetitle(p1, str);
                        return;
                 }
        }


        if(p1 && !p2) {
                if(inTitle) {
                        p1 = buf + (p1-tmp) + 7;
                        removetitle(p1, str);
                        return;
                }
                else {
                        inTitle = 1;
                        *(buf + (p1-tmp)) = 0;
                        str << buf;
                        p1 = buf + (p1-tmp) + 7;
                        removetitle(p1, str);
                        return;
                }
        }

        if(!p1 && p2) {
                if(inTitle) {
                        inTitle = 0;
                        p1 = buf + (p2-tmp) + 8;
                        removetitle(p1, str);
                        return;
                }
                else {
                        *(buf + (p2-tmp)) = 0;
                        str << buf;
                        p1 = buf + (p2-tmp) + 8;
                        removetitle(p1, str);
                        return;
                }
        }

        if(!p1 && !p2) {
                if(!inTitle) str << buf << endl;
                return;
        }
}

void uni_title(char* filename) {
        char buf[sz] = {0};
        ifstream in(filename);
        strstream newfilestr;
        int rowCount = 1;

        if ( in.getline(buf, sz) ) {
                newfilestr << buf << "\n";
        }

        while ( in.getline(buf, sz) ) {
                rowCount++;
                removetitle(buf, newfilestr);
        }

        ofstream out(filename);
        out << newfilestr.rdbuf();
        out.close();
}

void gt(char* filename, char* title0, strstream& str) {

        extern int fileCount, modify;
        //cout << filename << endl;

        enum { sz = 300 };
        char buf[sz];
        char* cp1;
        char* cp2;
        char* cp3;
        char* cp4;
        char newtitle[50] = {0};
        strstream newfilestr;

        if(strcmp(filename, "index.htm")) {
		ifstream in(filename);
		if(!in) {
	                cout << "Open file failed: " << filename << endl;
	                return;
	        }
		while ( in.getline(buf, sz) ) {
	                //common
	                /**/
	                if(!(cp1 = strstr(buf, "<title>"))) cp1 = strstr(buf, "<TITLE>");
			if(!(cp4 = strstr(buf, "</title>"))) cp4 = strstr(buf, "</TITLE>");
	                /**/
	                //linuxeden
			/**
	                if(!(cp1 = strstr(buf, "<td><center>"))) cp1 = strstr(buf, "<TD><CENTER>");
			if(!(cp4 = strstr(buf, "</center></td>"))) cp4 = strstr(buf, "</CENTER></TD>");
	                /**/
			if (cp1 && cp4) {
				cp1+=7;       //common
				//cp1+=12;        //linuxeden
				while( *cp1 == 32 ) cp1++;
				if( strlen(title0) ) {
					cp2 = strstr(cp1, title0);
					cp3 = cp2 + strlen(title0);
					if ( cp2 && cp3 <= cp4 ) {
						strncpy(newtitle, cp1, cp2-cp1);
						strncat(newtitle, cp3, cp4-cp3);
					}
					else strncpy(newtitle, cp1, cp4-cp1);
				}
				else strncpy(newtitle, cp1, cp4-cp1);
				newtitle[cp4-cp1] = 0;
				for(int i = strlen(newtitle)-1; i>0; i--) {
					//cout << newtitle;
					if(newtitle[i]!=32) break;
					else newtitle[i]=0;
				}

				if(!modify) break;      //break while, exit file scan
			}
	                if(modify) newfilestr << buf << "\n";
		}
        }
        if( !strlen(newtitle) ) strcpy(newtitle, "unTitled");

        if(modify) {

                /**    //backup old files
                char bakfilename[50] = {0};
                strcat(bakfilename, filename);
                strcat(bakfilename, ".bak");
                unlink(bakfilename);
                rename(filename, bakfilename);
                /**/

                //add title on top of the file
                ofstream newfile(filename);
                newfile << "<title>" << newtitle << "</title>" << endl;
                newfile << newfilestr.rdbuf();
                newfile.close();

                //remove other title tags in the file
                uni_title(filename);
        }

        cout << filename << ":\t" << newtitle << endl;
        str << "<tr onmouseover=this.className='tr_over' onmouseout=this.className=''> \n";
        str << "  <td width=30>" << setfill('0') << setw(3) << fileCount << "</td> \n";
        str << "  <td width=*><a href=" << filename << " target=_blank>" << newtitle << "</a></td> \n";
        str << "  <td width=*>" << " <a href=" << filename << " target=_blank>" << filename << "</a></td> \n";
        str << "</td></tr> \n";

}

int get_arg_index(int argc, char* argv[], char* s) {
        for(int i = 0; i < argc; i++)
                if( !strcmp(argv[i], s) ) return i;
        return 0;
}

int check_option(int argc, char* argv[]) {
        for(int i = 0; i < argc; i++)
                if( *(argv[i])=='-' &&
                    strcmp(argv[i], "-n") &&
                    strcmp(argv[i], "-t") &&
                    strcmp(argv[i], "-p") &&
                    strcmp(argv[i], "-m") )
                        return i;
        return 0;
}


int fileCount = 0;
int modify = 0;
int inTitle = 0;

main(int argc, char* argv[]) {


        char* usage = "USAGE: gettitle.exe [-n filename] [-t title] [-p path] [-m]\n"
                        "  filename:   name or pattern of the file(s) you want to scan, default: *.htm\n"
                        "  title:      the special string to be ignored in the title, default: null\n"
                        "  path:       work path, default: current path\n"
                        "  -m:         modify the title and update that file\n";

        int paraidx = 0;
        char filenamepattern[50] = "*.htm";;
        char title0[50] = {0};

        if( get_arg_index(argc, argv, "-help") ||
        get_arg_index(argc, argv, "--help") ||
        get_arg_index(argc, argv, "/help") ||
        get_arg_index(argc, argv, "?") ||
        get_arg_index(argc, argv, "/?") ) {
                cout << usage; exit(0);
        }

        if( int tmp = check_option(argc, argv) ) {
                cout << "Bad option(s): " << argv[tmp] << endl;
                cout << usage;
                exit(0);
        }

        if( paraidx = get_arg_index(argc, argv, "-n") ) {
                if(argc > paraidx+1) {
                        strcpy(filenamepattern, argv[paraidx+1]);
                        filenamepattern[strlen(argv[paraidx+1])] = 0;
                }
                else { cout << usage; exit(0); }
        }

        if( paraidx = get_arg_index(argc, argv, "-t") ) {
                if(argc > paraidx+1) strcpy(title0, argv[paraidx+1]);
                else { cout << usage; exit(0); }
        }

        if( paraidx = get_arg_index(argc, argv, "-p") ) {
                if(argc > paraidx+1) {
                        if(chdir(argv[paraidx+1])) {
                                cout << "chdir error: " << argv[paraidx+1] << endl;
                                cout << usage;
                                exit(1);
                        }
                }
                else { cout << usage; exit(0); }
        }

        if( paraidx = get_arg_index(argc, argv, "-m") ) {
                modify = 1;
        }

        unlink("index.htm.bak");
	rename("index.htm", "index.htm.bak");
        strstream str;
        str << "<style>\n";
        str << "a {color:#666699} \n";
        str << ".tr_over {background:#666699; color:#ffffff} \n";
        str << ".tr_over a {color:#ffffff} \n";
        str << ".tr_out {} \n";
        str << "</style>\n";
        str << "<table width='' cellspacing=0 cellpadding=2 style=\"font-size:80%; margin:5;\">\n";

	struct ffblk ffblk;
	int found;
	found = findfirst(filenamepattern, &ffblk, 0);
	while(!found) {
                fileCount++;
		gt(ffblk.ff_name, title0, str);
		found = findnext(&ffblk);
	}

        cout << "total: " << fileCount << " files" << endl;
        str << "</table>\n";

        ofstream out("index.htm");
        out << str.rdbuf();
}
