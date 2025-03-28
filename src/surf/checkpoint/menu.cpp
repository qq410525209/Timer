#include <core/menu.h>
#include <surf/checkpoint/surf_checkpoint.h>

void CSurfCheckpointService::OpenCheckpointsMenu() {
	auto pMenu = MENU::Create(MENU_CALLBACK_L(this) {
		switch (iItem) {
			case 0: {
				break;
			}
			case 1: {
				break;
			}
			case 2: {
				break;
			}
			case 3: {
				break;
			}
			case 4: {
				break;
			}
			case 5: {
				break;
			}
			case 6: {
				break;
			}
		}
	});

	pMenu->SetTitle("存点菜单");

	pMenu->AddItem("存点");
	pMenu->AddItem("读点");
	pMenu->AddItem("上一个");
	pMenu->AddItem("下一个");
	pMenu->AddItem("删除当前存点");
	pMenu->AddItem("重置");

	pMenu->Display(GetPlayer()->GetPlayerPawn());
}
