// CopyRight: (c) Danil Bakhtin. All rights reserved


#include "View/HUDWidget.h"

void UHUDWidget::TriggerCloseSessionEvent()
{
    OnHUDClickCloseSession.Broadcast();
}
