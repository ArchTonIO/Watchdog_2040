#include "apps/messaging/include/bitmaps.h"
#include "apps/messaging/include/contacts.h"
#include "apps/messaging/include/msg_manager.h"
#include "core/tools/include/launcher.h"

void notifications_menu_launch();

DEFINE_LAUNCHER(messaging_launcher,
    "Messaging",
    {"Send message", send_msg_icon, send_message},
    {"Read messages", read_messages_icon, read_messages},
    {"Contacts", contacts_icon, contacts_menu_launch},
    {"Scan", scan_contacts_icon, scan_and_send_message},
    {"Notifications", notifications_icon, notifications_menu_launch}, )

DEFINE_LAUNCHER(notifications_launcher,
    "Notifications",
    {"Enable notifications",
        enable_notifications_icon,
        enable_message_notifications},
    {"Disable notifications",
        disable_notifications_icon,
        disable_message_notifications})

void messaging_launch() { launcher_start_tui(&messaging_launcher); }
void notifications_menu_launch() {
  launcher_start_tui(&notifications_launcher);
}