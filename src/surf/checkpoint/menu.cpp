#include <core/menu.h>
#include <surf/checkpoint/surf_checkpoint.h>

void CSurfCheckpointService::OpenCheckpointsMenu() {
	auto pController = GetPlayer()->GetController();
	if (!pController) {
		return;
	}

	auto wpMenu = MENU::Create(
		pController, MENU_CALLBACK_L(this) {
			switch (iItem) {
				case 0: {
					this->SaveCheckpoint();
					this->m_iCurrentCP = this->GetLatestCheckpoint();
					break;
				}
				case 1: {
					this->LoadCheckpoint(this->m_iCurrentCP);
					break;
				}
				case 2: {
					this->LoadPrev();
					break;
				}
				case 3: {
					this->LoadNext();
					break;
				}
				case 4: {
					this->DeleteCheckpoint(this->m_iCurrentCP);
					this->m_iCurrentCP--;
					this->ClampIndex(this->m_iCurrentCP);
					break;
				}
				case 5: {
					this->ResetCheckpoint();
					break;
				}
			}
		});

	if (wpMenu.expired()) {
		SDK_ASSERT(false);
		return;
	}

	auto pMenu = wpMenu.lock();
	pMenu->SetTitle("存点菜单");
	pMenu->AddItem("存点");
	pMenu->AddItem("读点");
	pMenu->AddItem("上一个");
	pMenu->AddItem("下一个");
	pMenu->AddItem("删除当前存点");
	pMenu->AddItem("重置");

	pMenu->Display();
}
