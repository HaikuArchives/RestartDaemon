/*!
\file
\brief The main function for the micro restarter.

This file contains the main function for the MicroRestarterApp, which
in turn can restart the restart_daemon.
*/
#include <stdio.h>
#include "mrapp.h"
int main()
{
	(new MicroRestarterApp())->Run();
	delete be_app;
	
	return 0;
		
}
/*
$Id: mrmain.cpp,v 1.1 2004/07/13 15:45:05 bls Exp $

$Log: mrmain.cpp,v $
Revision 1.1  2004/07/13 15:45:05  bls
source files added

Revision 1.3  2004/06/29 04:21:22  bls
More documenting; added more #defines for future functionality. Cleaned
up a bit more code. Still much more documenting to do.

Revision 1.2  2004/06/28 05:12:25  bls
Added CVS keywords to most files, added Doxygen file as well for documentation.

*/
