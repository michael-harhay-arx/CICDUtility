#pragma pack(4)
typedef struct {char *name; void *address; unsigned long isFunction:1; unsigned long reserved:31;} ExeSymbol;
void __cdecl AboutBoxMenuItemCallback (int menuBar, int menuItem, void *callbackData, int panel);
int __cdecl AboutBoxOKCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl ArxCB_ExpandTestInfo (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl ArxCB_IdleTimer (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl MainPanelCallback (int panel, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl TabControlCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __UICallbackSymbolCount = 6;
ExeSymbol __UICallbackSymbols [6] =
{
 {"_AboutBoxMenuItemCallback", (void*)AboutBoxMenuItemCallback, 1, 0},
 {"_AboutBoxOKCallback", (void*)AboutBoxOKCallback, 1, 0},
 {"_ArxCB_ExpandTestInfo", (void*)ArxCB_ExpandTestInfo, 1, 0},
 {"_ArxCB_IdleTimer", (void*)ArxCB_IdleTimer, 1, 0},
 {"_MainPanelCallback", (void*)MainPanelCallback, 1, 0},
 {"_TabControlCallback", (void*)TabControlCallback, 1, 0}
};