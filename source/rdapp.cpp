/*!
\file
\brief The definition file for the RestarterDaemonApp.
*/
#include "rdapp.h"
#include <Roster.h>
#include <stdio.h>
#include <Alert.h>
#include "rddbicon.h"

/*!
\brief "What" value for timing BMessageRunner object.
*/
#define TICK 'tick'

RestarterDaemonApp::RestarterDaemonApp()
	:BApplication(APP1SIG)
{
	be_roster->StartWatching(be_app_messenger,B_REQUEST_LAUNCHED|B_REQUEST_QUIT);
	im_restarting=false;
	apps=new BList();
	BDeskbar deskbar;
	app_info ai;
	GetAppInfo(&ai);
	//Add deskbar icon the non-deprecated way...
	deskbar.AddItem(&ai.ref,&deskbar_icon_id);
}

RestarterDaemonApp::~RestarterDaemonApp()
{
	delete update_runner;
	if (!apps->IsEmpty())
	{
		app_struct *app;
		while(!apps->IsEmpty())
		{
			app=(app_struct*)apps->RemoveItem(0L);
			delete app;
		}
	}
	BDeskbar deskbar;
	deskbar.RemoveItem(deskbar_icon_id);
	delete apps;
}

void RestarterDaemonApp::ReadyToRun()
{
	BMessage update_msg(TICK);
	/*
	Run the update check twice a second, to make sure that we restart scheduled
	applications as quickly as possible without tying up a lot of resources.
	*/
	update_runner=new BMessageRunner(be_app_messenger,&update_msg,500000,-1);	
}

void RestarterDaemonApp::MessageReceived(BMessage *msg)
{
	
	switch(msg->what)
	{
		case TICK:
		{
			if (!apps->IsEmpty())
			{
				app_struct *current;
				time_t current_time=real_time_clock();
				team_id team;
				for (int32 i=0; i<apps->CountItems(); i++)
				{
					current=(app_struct*)apps->ItemAt(i);
					if (current->ref_set)
					{
						if (!be_roster->IsRunning(&current->ref))
						{
							if (current->restart_time<=current_time)
							{//Launch app only after requested restart time.
								be_roster->Launch(&current->ref,(BList*)NULL,&team);
							}
						}
					}
					else
					{
						if (!be_roster->IsRunning(current->app_sig.String()))
						{
							if (current->restart_time<=current_time)
							{
								be_roster->Launch(current->app_sig.String(),(BList*)NULL,&team);
							}
						}
					}
				}
			}
		}break;
		case RD_RESTART_APP:
		{
			app_struct *app=new app_struct;
			if (msg->HasRef("rd:ref"))
			{
				app->ref_set=true;
				msg->FindRef("rd:ref",&app->ref);
			}
			else
				app->ref_set=false;
			
			if (msg->HasString("rd:app_name"))
				msg->FindString("rd:app_name",&app->app_name);
			
			if (msg->HasString("be:signature"))
				msg->FindString("be:signature",&app->app_sig);
			else
				if (msg->HasString("rd:app_sig"))
					msg->FindString("rd:app_sig",&app->app_sig);
			
			if (msg->HasInt32("rd:restart_time"))
				msg->FindInt32("rd:restart_time",&app->restart_time);
			else
				app->restart_time=0l;
			
			if (msg->HasMessage("rd:start_message"))
			{
				/*
					If there is one or more BMessage that is to be sent at startup
					inside the BMessage, add them to our list for sending at app startup.
				*/
				BMessage temp_mesg;
				int32 count=0;
				type_code code;
				msg->GetInfo("rd:start_message",&code,&count);
				for (int32 i=0; i<count; i++)
				{
					msg->FindMessage("rd:start_message",i,&temp_mesg);
					app->messages->AddItem(new BMessage(temp_mesg));
				}
			}
			if (apps->IsEmpty())
			{
				apps->AddItem(app);
				msg->SendReply(RD_RESTARTD_OK,this);
			}
			else
			{
				app_struct *temp;
				BEntry new_entry,temp_entry;
				bool found=false;
				for (int32 i=0; i<apps->CountItems(); i++)
				{
					temp=(app_struct*)apps->ItemAt(i);
					if (app->ref_set && temp->ref_set)
					{
						new_entry.SetTo(&app->ref);
						temp_entry.SetTo(&temp->ref);
						if (new_entry==temp_entry)
						{
							found=true;
							break;
						}
					}
					else
					{
						if (app->app_sig.ICompare(temp->app_sig)==0)
						{
							found=true;
							break;
						}
						
					}
				}
				if (found)
				{//We already have this item schedule for restart; reject new one.
							msg->SendReply(RD_RESTARTD_ERROR,this);
							delete app;
				}
				else
				{//This is a new item, add it to the system.
					apps->AddItem(app);
					msg->SendReply(RD_RESTARTD_OK,this);
				}
				
			}
		}break;
		case RD_CANCEL_APP:
		{
				app_struct *temp;
				BEntry new_entry,temp_entry;
				bool found=false;
				BString app_sig;
				entry_ref ref;
				bool ref_set=false;
				if (msg->HasRef("rd:ref"))
				{
					msg->FindRef("rd:ref",&ref);
					
					ref_set=true;
				}
				else
				{
					if (msg->HasString("rd:app_sig"))
					{
						msg->FindString("rd:app_sig",&app_sig);
					}
					else
					{
						if (msg->HasString("be:signature"))
						{
							msg->FindString("be:signature",&app_sig);
						}
					}
				}
				for (int32 i=0; i<apps->CountItems(); i++)
				{
					temp=(app_struct*)apps->ItemAt(i);
					if (ref_set && temp->ref_set)
					{
						new_entry.SetTo(&ref);
						temp_entry.SetTo(&temp->ref);
						if (new_entry==temp_entry)
						{
							found=true;
							break;
						}
					}
					else
					{
						if (app_sig.ICompare(temp->app_sig)==0)
						{
							found=true;
							break;
						}
					}
				}
				if (found)
				{
					apps->RemoveItem(temp);
					delete temp;
					msg->SendReply(RD_RESTARTD_OK);
					
				}
				else
				{
					msg->SendReply(RD_RESTARTD_ERROR);
					
				}
				
		}break;
		case RD_UPDATE_APP:
		{
				app_struct *temp;
				BEntry new_entry,temp_entry;
				bool found=false;
				BString app_sig;
				entry_ref ref;
				bool ref_set=false;
				if (msg->HasRef("rd:ref"))
				{
					msg->FindRef("rd:ref",&ref);
					
					ref_set=true;
				}
				else
				{
					if (msg->HasString("rd:app_sig"))
					{
						msg->FindString("rd:app_sig",&app_sig);
					}
					else
					{
						if (msg->HasString("be:signature"))
						{
							msg->FindString("be:signature",&app_sig);
						}
					}
				}
				for (int32 i=0; i<apps->CountItems(); i++)
				{
					temp=(app_struct*)apps->ItemAt(i);
					if (ref_set && temp->ref_set)
					{
						new_entry.SetTo(&ref);
						temp_entry.SetTo(&temp->ref);
						if (new_entry==temp_entry)
						{
							found=true;
							break;
						}
					}
					else
					{
						if (app_sig.ICompare(temp->app_sig)==0)
						{
							found=true;
							break;
						}
					}
				}
				if (found)
				{//update existing item.
					msg->SendReply(RD_RESTARTD_OK,this);
					if (msg->HasRef("rd:ref"))
					{
						msg->FindRef("rd:ref",&temp->ref);
					}
					else
						temp->ref_set=false;
					
					if (msg->HasString("rd:app_name"))
						msg->FindString("rd:app_name",&temp->app_name);
					
					if (msg->HasString("be:signature"))
						msg->FindString("be:signature",&temp->app_sig);
					else
						if (msg->HasString("rd:app_sig"))
							msg->FindString("rd:app_sig",&temp->app_sig);
					
					if (msg->HasInt32("rd:restart_time"))
						msg->FindInt32("rd:restart_time",&temp->restart_time);
					else
						temp->restart_time=0l;
					
					if (msg->HasMessage("rd:start_message"))
					{
						for (int32 i=0; i<temp->messages->CountItems(); i++)
							delete (BMessage*)temp->messages->RemoveItem(1L);
						BMessage temp_mesg;
						int32 count=0;
						type_code code;
						msg->GetInfo("rd:start_message",&code,&count);
						for (int32 i=0; i<count; i++)
						{
							msg->FindMessage("rd:start_message",i,&temp_mesg);
							temp->messages->AddItem(new BMessage(temp_mesg));
						}
					}
				}
				else
				{//we don't have an entry for this item...
					msg->SendReply(RD_RESTARTD_ERROR,this);
				}
		}break;
		case RD_RESTART_RD:
		{
			im_restarting=true;
			be_app_messenger.SendMessage(B_QUIT_REQUESTED);
			msg->SendReply(B_OK,this);
			
		}break;
		case RD_SHUTDOWN_RD:
		{
			im_restarting=false;
			be_app_messenger.SendMessage(B_QUIT_REQUESTED);
			msg->SendReply(B_OK,this);
			
		}break;
		
		case B_SOME_APP_LAUNCHED:
		{
//			msg->PrintToStream();
			BString sig;
			const char *sig2=NULL;
			team_id team;
			msg->FindInt32("be:team",&team);
			msg->FindString("be:signature",&sig2);
			if (strcmp(sig2,APP2SIG)==0)
			{
				if (im_restarting)
				{
					be_app_messenger.SendMessage(B_QUIT_REQUESTED);
				}
				else
				{//we don't need our partner app...
					BMessenger *msgr=new BMessenger(APP2SIG);
					msgr->SendMessage(B_QUIT_REQUESTED);
					delete msgr;
				}
				
			}
			else
			{
				if (!apps->IsEmpty())
				{
					app_struct *item;
					entry_ref ref;
					bool has_ref=false;
					if (msg->HasRef("be:ref"))
					{
						msg->FindRef("be:ref",&ref);
						has_ref=true;
					}
					BEntry started_app(&ref),candidate_app;
					for (int32 i=0; i<apps->CountItems(); i++)
					{
						item=(app_struct*)apps->ItemAt(i);
						if (!item->ref_set)
						{
							if (item->app_sig.ICompare(sig)==0)
							{//a targeted application has restarted...
								apps->RemoveItem(i);
								
								BMessenger *msgr=new BMessenger(NULL,team);
								BMessage *mesg;
								for (int32 j=0; j<item->messages->CountItems(); j++)
								{
									mesg=(BMessage*)item->messages->ItemAt(j);
									msgr->SendMessage(mesg);
								}
								delete msgr;
								delete item;
								break;
							}
						}
						else
						{
							candidate_app.SetTo(&item->ref);
							if (candidate_app==started_app)
							{//a targeted application has restarted...
								apps->RemoveItem(i);
								BMessenger *msgr=new BMessenger(NULL,team);
								BMessage *mesg;
								for (int32 j=0; j<item->messages->CountItems(); j++)
								{
									mesg=(BMessage*)item->messages->ItemAt(j);
									msgr->SendMessage(mesg);
								}
								delete msgr;
								delete item;
								break;
							}
						}
					}
				}
			}
		}break;
		case B_SOME_APP_QUIT:
		{
			if (!apps->IsEmpty())
			{
				app_struct *item;
				time_t current_time=real_time_clock();
				bool found=false;
				entry_ref ref;
				msg->FindRef("be:ref",&ref);
				BString sig;
				msg->FindString("be:signature",&sig);
				BEntry quitting_app(&ref),candidate_app;
				for (int32 i=0;i<apps->CountItems(); i++)
				{
						item=(app_struct*)apps->ItemAt(i);
						if (!item->ref_set)
						{
							if (item->app_sig.ICompare(sig)==0)
							{
								found=true;
								break;
							}
						}else
						{
							candidate_app.SetTo(&item->ref);
							if (candidate_app==quitting_app)
							{
								found=true;
								break;
							}
						}
				}
				if (found)
				{
					if (item->restart_time<=current_time)
					{
						team_id team;
						BMessage *restart_message=new BMessage(RD_APP_RESTARTED);
						restart_message->AddInt32("rd:start_time",real_time_clock());
						
						item->messages->AddItem(restart_message,0);
/*
Note: It appears to be a bug in Be's BeOS R5.0.x that messages sent as part of the 
BRoster Launch() function don't actually get sent... It appears that only B_REFS_RECEIVED
messages are actually sent, and that all others get lost to the nether regions of
our machines...
*/						
						time_t current_time=real_time_clock();
						
						if (item->ref_set)
						{
							if (item->restart_time<=current_time)
								be_roster->Launch(&item->ref,(BList*)NULL,&team);
						}
						else
						{
							if (item->restart_time<=current_time)
								be_roster->Launch(item->app_sig.String(),(BList*)NULL,&team);
						}
						if (team==-1)
							(new BAlert("Restart Daemon","Error launching a targeted application","Drat"))->Go(NULL);
					}
				}
			}
			
		}break;
		default:
			BApplication::MessageReceived(msg);
	}
	
}

bool RestarterDaemonApp::QuitRequested()
{
	if ((im_restarting) && (!be_roster->IsRunning(APP2SIG)))
	{
			be_roster->Launch(APP2SIG);
			return false;
	}
	else
	{
		if ((be_roster->IsRunning(APP2SIG)) && (im_restarting))
			{
				snooze(100000);
				
			}
			
	}
	
	be_roster->StopWatching(be_app_messenger);
	return true;
}

/*
$Id: rdapp.cpp,v 1.1 2004/07/13 15:45:05 bls Exp $

$Log: rdapp.cpp,v $
Revision 1.1  2004/07/13 15:45:05  bls
source files added

Revision 1.7  2004/07/03 22:17:12  bls
* More documentation!
* Switched over to the newer way of adding a Deskbar replicant: using AddItem(entry_ref *addon,int32 *id).
* Made a few replies to the Deskbar replicant asynchronous.
* Switched compiler flags over to optimize in a more permanent fashion.

I have also now contacted BeUnited about donating the application and source to them for
whomsoever wishes to take up development. I am also looking into adding support for
restart_daemon to OpenTracker. This will probably be a few weeks away however. More because
of lack of time than difficulty.

Revision 1.6  2004/07/03 18:29:32  bls
* Implemented the update and cancel features.
* Fixed the Deskbar icon by removing the bitmap that I was trying to draw there; now I use DrawString to create a viewable icon.
* Fixed an undiscovered problem with the micro_restarter: namely that because I wasn't doing a proper string comparison, the micro_restarter stopped restarting the restart_daemon itself when requested. The question is: Why did it work previously?
* Tweaks here and there.

Revision 1.5  2004/06/30 05:40:41  bls
I have begun the process of adding the Deskbar icon, and I also added some
more code to the restart_daemon application to flesh it out a bit more.
Strangely enough, I haven't broken anything, although the deskbar icon
is appearing as a blank white square. This appears to be caused by the
archiving process as the image is added to the Deskbar... It should be
loading the icon image from the resource file, and showing the letters
"RD" in black letters on a white background, but it doesn't seem to be
doing that... Strange. Any how, added more documentation as well.

Revision 1.4  2004/06/29 20:26:43  bls
Even more documentation added.

Revision 1.3  2004/06/29 04:21:22  bls
More documenting; added more #defines for future functionality. Cleaned
up a bit more code. Still much more documenting to do.

Revision 1.2  2004/06/28 05:12:25  bls
Added CVS keywords to most files, added Doxygen file as well for documentation.

*/
