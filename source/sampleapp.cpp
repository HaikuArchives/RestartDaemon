/*!
\file
\brief The sample application's BApplication class.
*/
#include "sampleapp.h"
#include <Roster.h>
#include <Alert.h>
#include <Messenger.h>


#define RD_RESTART_APP 'rdra'
#define RD_APP_RESTARTED 'rdar'

/*!
\brief The what value for a BMessage that triggers a BAlert prompt.
*/
#define PROMPT_MESSAGE 'prmp'

/*!
\brief What value for a start-up message.
*/
#define START_UP_MESSAGE 'sums'

SampleApp::SampleApp()
	:BApplication(SAMPLESIG)
{
	show_prompt=true;
	i_launched_rd=false;
}

SampleApp::~SampleApp()
{
}

void SampleApp::ReadyToRun()
{
	be_app_messenger.SendMessage(PROMPT_MESSAGE);
	if (!be_roster->IsRunning(APP1SIG))
	{	
		be_roster->Launch(APP1SIG);
		i_launched_rd=true;
	}
}

void SampleApp::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case PROMPT_MESSAGE:
		{
			if (show_prompt) {
				BMessenger msgr("application/x-vnd.bls-restart_daemon");
				BMessage qmsg(RD_RESTART_APP);
				/*
				You may use either be:signature or rd:app_sig to send the mime type informatin
				to the restart_daemon; it doesn't matter which. Both are optional if rd:ref
				is supplied instead.
				*/
				qmsg.AddString("be:signature",SAMPLESIG);
				qmsg.AddString("rd:app_sig",SAMPLESIG);
				qmsg.AddString("rd:app_name","rd_sample_app");
				BMessage start_msg(START_UP_MESSAGE);
				start_msg.AddBool("i_launched_rd",i_launched_rd);
				qmsg.AddMessage("rd:start_message",&start_msg);
				/*
				Note: There is an additional fields that are optional at this point:
				rd:restart_time
				
				rd:start_message should be of BMessage type, and should contain BMessages
				that the application will receive upon restarting. There appears to be a
				bug with the BRoster::Launch() function that only lets BMessages with a 
				what value of B_REFS_RECEIVED to be transmitted to the launching application
				through that function. However, it is still possible to send one or more
				messages shortly after startup to achieve a similar effect. Hopefully the
				OSBOSes will fix that bug.
				
				rd:restart_time will not restart the application until after this time value
				as generated via a bit of math and real_time_clock(). For instance, if
				you want to have your application restart in 5 minutes (assuming it's in
				the process of shutting down now), you could send the restart request
				BMessage with the rd:restart_time field set to (real_time_clock()+300).
				The default value if not specified is zero (0), which will cause the
				application to be launched immediately. In fact, any value less than
				the current value returned by real_time_clock() in the daemon will cause
				the application to be launched immediately after it has shutdown.
				*/
				app_info ai;
				GetAppInfo(&ai);
				/*
				rd:ref is optional if you supply the mime type signature of your application.
				rd:ref is useful if you want a specific version of your application to be
				restarted; otherwise whichever one that BeOS finds first with the matching
				application signature will be started.
				*/
				qmsg.AddRef("rd:ref",&ai.ref);
				switch((new BAlert("rd_sample_app","Restart rd_sample_app?","No","Yes"))->Go())
				{
					case 0:
					{
						(new BAlert("rd_sample_app","Ok, just quitting then.","Ok"))->Go();
					}break;
					case 1:
					{
						
						msgr.SendMessage(&qmsg);
					}break;
				}
				//Whatever the response to the prompt; quit the application.
				be_app_messenger.SendMessage(B_QUIT_REQUESTED);
			}
		}break;
		case START_UP_MESSAGE:
		{
			(new BAlert("rd_sample_app","Start-up message received.","Ok"))->Go();
			msg->FindBool("i_launched_rd",&i_launched_rd);
		}break;
		case RD_APP_RESTARTED:
		{
			show_prompt=false;
			(new BAlert("sample app","App launched by restart daemon.\nSampleApp will now quit normally.","Ok"))->Go();
			be_app_messenger.SendMessage(B_QUIT_REQUESTED);
		}break;
		default:
			BApplication::MessageReceived(msg);
		
	}
	
}

bool SampleApp::QuitRequested()
{
	if (i_launched_rd && !show_prompt)
	{
		BMessenger msgr(APP1SIG);
		msgr.SendMessage(B_QUIT_REQUESTED);
	}
	return true;
}

/*
$Id: sampleapp.cpp,v 1.1 2004/07/13 15:45:05 bls Exp $

$Log: sampleapp.cpp,v $
Revision 1.1  2004/07/13 15:45:05  bls
source files added

Revision 1.5  2004/07/03 18:29:32  bls
* Implemented the update and cancel features.
* Fixed the Deskbar icon by removing the bitmap that I was trying to draw there; now I use DrawString to create a viewable icon.
* Fixed an undiscovered problem with the micro_restarter: namely that because I wasn't doing a proper string comparison, the micro_restarter stopped restarting the restart_daemon itself when requested. The question is: Why did it work previously?
* Tweaks here and there.

Revision 1.4  2004/06/30 05:40:41  bls
I have begun the process of adding the Deskbar icon, and I also added some
more code to the restart_daemon application to flesh it out a bit more.
Strangely enough, I haven't broken anything, although the deskbar icon
is appearing as a blank white square. This appears to be caused by the
archiving process as the image is added to the Deskbar... It should be
loading the icon image from the resource file, and showing the letters
"RD" in black letters on a white background, but it doesn't seem to be
doing that... Strange. Any how, added more documentation as well.

Revision 1.3  2004/06/29 20:26:43  bls
Even more documentation added.

Revision 1.2  2004/06/28 05:12:25  bls
Added CVS keywords to most files, added Doxygen file as well for documentation.

*/
