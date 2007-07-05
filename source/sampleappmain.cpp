/*!
\file
\brief The sample application's main function.
*/
#include "sampleapp.h"

/*!
\brief The sample application's main function.
*/
int main()
{
	(new SampleApp())->Run();
	delete be_app;
	return 0;
}
/*
$Id: sampleappmain.cpp,v 1.1 2004/07/13 15:45:05 bls Exp $

$Log: sampleappmain.cpp,v $
Revision 1.1  2004/07/13 15:45:05  bls
source files added

Revision 1.3  2004/06/29 20:26:43  bls
Even more documentation added.

Revision 1.2  2004/06/28 05:12:25  bls
Added CVS keywords to most files, added Doxygen file as well for documentation.

*/
