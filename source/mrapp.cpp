/*!
\file
\brief The definition file for the MicroRestarterApp class.
*/
#include "mrapp.h"
#include <String.h>
#include <Roster.h>
#include <Alert.h>
#include <Entry.h>
#define STATUS_CHECK 'schk'

MicroRestarterApp::MicroRestarterApp()
	:BApplication(APP2SIG)
{
/*
This code is simple enough: it tells the roster server/daemon that we want to receive notifications
when applications start or quit.
*/
	be_roster->StartWatching(be_app_messenger,B_REQUEST_LAUNCHED|B_REQUEST_QUIT);
}

MicroRestarterApp::~MicroRestarterApp()
{
	
}

bool MicroRestarterApp::QuitRequested()
{
/*
Before we quit, we want to make sure that we stop receiving notifications and potentially free up 
resources that are involved in notifying us of the application launches and terminations.
*/
	be_roster->StopWatching(be_app_messenger);
	return true;
}
/*!
\internal
Messages received from the BeOS roster concerning application startup and termination are similar to the
one below, which was taken from the launch of the restart_daemon.

BMessage: what = BRAS (0x42524153, or 1112686931)
    entry   be:signature, type='CSTR', c=1, size=37, data[0]: "application/x-vnd.bls-restart_daemon"
    entry        be:team, type='LONG', c=1, size= 4, data[0]: 0xea4 (3748, '')
    entry      be:thread, type='LONG', c=1, size= 4, data[0]: 0x22a9 (8873, '')
    entry       be:flags, type='LONG', c=1, size= 4, data[0]: 0x10000009 (268435465, '')
    entry         be:ref, type='RREF', c=1, size=27, data[0]: device=7, directory=7653630, name="restart_daemon", path="/zanos/programming/restart_daemon/output/restart_daemon"                       
*/
void MicroRestarterApp::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		/*
			BMessage from the roster that some program has quit. We parse the BMessage to see if the
			message contains the application signature of the restart_daemon.
		*/
		case B_SOME_APP_QUIT:
		{
			BString sig;
			msg->FindString("be:signature",&sig);
			if (sig.ICompare(APP1SIG)==0)
			{
				//The restart daemon has quit; launch it again.
				entry_ref ref;
				msg->FindRef("be:ref",&ref);
				team_id team=-1;
				status_t status=B_ERROR;
				status=be_roster->Launch(&ref,(BMessage*)NULL,&team);
				if (team==-1)
				{
					//some error checking and handling should go here... obviously...
					(new BAlert("mr","Couldn't restart restart_daemon...","WTF?"))->Go(NULL);
					
				}
			}
		}break;
		/*
			BMessage from the roster that some program has started. Again we parse the BMessage to see if
			the message contains the application signature of the restart_daemon.
		*/
		case B_SOME_APP_LAUNCHED:
		{
			BString sig;
			msg->FindString("be:signature",&sig);
			if (sig.ICompare(APP1SIG)==0)
			{//our partner (the restart_daemon application) has started; we are no longer needed. commit seppuku.
				be_app_messenger.SendMessage(B_QUIT_REQUESTED);
			}
			
		}break;
		default:
			BApplication::MessageReceived(msg);
	}
}
/*
$Id: mrapp.cpp,v 1.1 2004/07/13 15:45:05 bls Exp $

$Log: mrapp.cpp,v $
Revision 1.1  2004/07/13 15:45:05  bls
source files added

Revision 1.6  2004/07/03 18:29:32  bls
* Implemented the update and cancel features.
* Fixed the Deskbar icon by removing the bitmap that I was trying to draw there; now I use DrawString to create a viewable icon.
* Fixed an undiscovered problem with the micro_restarter: namely that because I wasn't doing a proper string comparison, the micro_restarter stopped restarting the restart_daemon itself when requested. The question is: Why did it work previously?
* Tweaks here and there.

Revision 1.5  2004/06/29 20:26:43  bls
Even more documentation added.

Revision 1.4  2004/06/29 04:21:22  bls
More documenting; added more #defines for future functionality. Cleaned
up a bit more code. Still much more documenting to do.

Revision 1.3  2004/06/29 00:02:58  bls
Added more documentation, cleaned up some code a bit.

Revision 1.2  2004/06/28 05:12:25  bls
Added CVS keywords to most files, added Doxygen file as well for documentation.

*/

