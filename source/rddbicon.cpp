/*!
\file
\brief This file contains the definition of the RDDBIcon class.
*/
#include "rddbicon.h"
#include <Roster.h>
#include <Message.h>
#include <Messenger.h>
#include "rdapp.h"
#include <Alert.h>

#define QUIT_RD 'qtrd'
#define RESTART_RD 'r*rd'
/*!
\brief Creates the RDDBIcon view for Deskbar instantiation.

This function creates the BView (RDDBIcon) necessary for creating the Deskbar
icon replicant. The BDeskbar class's AddItem() function now requires this method
in order to create the Deskbar replicant rather than an older deprecated method
that was a bit simpler. Oh well, doesn't really matter as long as it works.
*/
extern "C" _EXPORT BView *instantiate_deskbar_item(void)
{
	return (new RDDBIcon(BRect(0,0,15,15),"restart_daemon_db_icon",0,B_WILL_DRAW));
}

RDDBIcon::RDDBIcon(BRect frame, const char *name, uint32 resize_mode,uint32 flags)
	:BView(frame,name,resize_mode,flags)
{
	Init();
}

RDDBIcon::RDDBIcon(BMessage *archive)
	:BView(archive)
{
	Init();
	replicant_mode=true;
}

RDDBIcon::~RDDBIcon()
{
	delete deskbar;
}

void RDDBIcon::Init(void)
{
	replicant_mode=false;
	deskbar=new BDeskbar();
	pop_up_menu=NULL;
	restart_rd_mi=NULL;
	quit_rd_mi=NULL;

	/*
	prepare menus
	
	We're adding a separator item in deference to the future, when we might build
	a list of applications that have requested to be restarted if/when they shutdown.
	We will want to keep them separated from the menu items that affect the daemon.
	*/
	pop_up_menu=new BPopUpMenu("restart_daemon_pop_up_menu",false,false,B_ITEMS_IN_COLUMN);
	pop_up_menu->AddSeparatorItem();
	restart_rd_mi=new BMenuItem("Restart",new BMessage(RESTART_RD));
	pop_up_menu->AddItem(restart_rd_mi);
	quit_rd_mi=new BMenuItem("Exit",new BMessage(QUIT_RD));
	pop_up_menu->AddItem(quit_rd_mi);
}
void RDDBIcon::Draw(BRect update_rect)
{
	if (Window())
	{
		LockLooper();
		DrawString("R*\0",BPoint(0,8));
		DrawString("d\0", BPoint(8,15));
		UnlockLooper();
	}
	BView::Draw(update_rect);
}

void RDDBIcon::AttachedToWindow()
{
	attached_to_window=true;
	pop_up_menu->SetTargetForItems(this);
	SetViewColor(Parent()->ViewColor());
}

void RDDBIcon::DetachedFromWindow()
{
	attached_to_window=false;
}

void RDDBIcon::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case RESTART_RD:
		{
			status_t error=B_NO_ERROR;
			BMessenger msgr(APP1SIG,-1,&error);
			BMessage reply;
			BMessage rmsg(RD_RESTART_RD);
			msgr.SendMessage(&rmsg,&reply,200000,300000);
			if ((reply.what!=B_OK) || (error!=B_OK))
				(new BAlert("RD","There has been a problem restarting the restart_daemon. Ironic, yes?","Drat!"))->Go(NULL);
			
		}break;
		case QUIT_RD:
		{
			status_t error=B_NO_ERROR;
			BMessenger msgr(APP1SIG,-1,&error);
			BMessage reply;
			BMessage rmsg(RD_SHUTDOWN_RD);
			msgr.SendMessage(&rmsg,&reply,200000,300000);
			if ((reply.what!=B_OK) || (error!=B_OK))
				(new BAlert("RD","There has been a problem shutting down the restart_daemon. Ironic, yes?","Drat!"))->Go(NULL);
		}break;
		default:
			BView::MessageReceived(msg);
	}
	
}
void RDDBIcon::MouseDown(BPoint point)
{
	uint32 buttons=0;
	BPoint cursor;
	GetMouse(&cursor,&buttons,true);
	ConvertToScreen(&cursor);
	cursor.y--;
	cursor.x--;
	/*
	In the future, we may want to contact the restart_daemon and build a list of
	applications that requested restarting and dynamically populate the pop-up
	menu with these items. In theory, we could allow the restart request to be removed
	for any given application with a simple menu selection by the user. For now,
	however, we will just show the restart_daemon specific menu items.
	*/
	pop_up_menu->Go(cursor,true,false,true);
}

status_t RDDBIcon::Archive(BMessage *archive, bool deep) const
{
	status_t status=B_OK;
	status=BView::Archive(archive,deep);
	if (deep)
	{
		
	}
	if (status==B_OK)
		status=archive->AddString("class","RDDBIcon");
	if (status==B_OK)
		status=archive->AddString("add_on",APP1SIG);
	return status;
}

RDDBIcon *RDDBIcon::Instantiate(BMessage *archive)
{
	if (validate_instantiation(archive,"RDDBIcon"))
		return (new RDDBIcon(archive));
	return NULL;
}


/*
$Id: rddbicon.cpp,v 1.1 2004/07/13 15:45:05 bls Exp $

$Log: rddbicon.cpp,v $
Revision 1.1  2004/07/13 15:45:05  bls
source files added

Revision 1.3  2004/07/03 22:17:12  bls
* More documentation!
* Switched over to the newer way of adding a Deskbar replicant: using AddItem(entry_ref *addon,int32 *id).
* Made a few replies to the Deskbar replicant asynchronous.
* Switched compiler flags over to optimize in a more permanent fashion.

I have also now contacted BeUnited about donating the application and source to them for
whomsoever wishes to take up development. I am also looking into adding support for
restart_daemon to OpenTracker. This will probably be a few weeks away however. More because
of lack of time than difficulty.

Revision 1.2  2004/07/03 18:29:32  bls
* Implemented the update and cancel features.
* Fixed the Deskbar icon by removing the bitmap that I was trying to draw there; now I use DrawString to create a viewable icon.
* Fixed an undiscovered problem with the micro_restarter: namely that because I wasn't doing a proper string comparison, the micro_restarter stopped restarting the restart_daemon itself when requested. The question is: Why did it work previously?
* Tweaks here and there.

Revision 1.1  2004/06/30 05:40:41  bls
I have begun the process of adding the Deskbar icon, and I also added some
more code to the restart_daemon application to flesh it out a bit more.
Strangely enough, I haven't broken anything, although the deskbar icon
is appearing as a blank white square. This appears to be caused by the
archiving process as the image is added to the Deskbar... It should be
loading the icon image from the resource file, and showing the letters
"RD" in black letters on a white background, but it doesn't seem to be
doing that... Strange. Any how, added more documentation as well.

*/
