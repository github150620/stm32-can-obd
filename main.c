#include "stm32f10x.h"
#include "stdio.h"

void USB_LP_CAN1_RX0_IRQHandler(void) {
	CanRxMsg msg;
	if (CAN_MessagePending(CAN1, CAN_FIFO0)) {
		CAN_Receive(CAN1, CAN_FIFO0, &msg);
		printf("StdId: %08x, ExtId: %08x, IDE: %d, RTR: %d, DLC: %d, ", msg.StdId, msg.ExtId, msg.IDE, msg.RTR, msg.DLC);
		printf("Data: %02x %02x %02x %02x %02x %02x %02x %02x\n", 
			msg.Data[0], 
			msg.Data[1], 
			msg.Data[2], 
			msg.Data[3], 
			msg.Data[4], 
			msg.Data[5], 
			msg.Data[6], 
			msg.Data[7]
		);
	}
}

void delay(int i) {
	while (i--);
}

void CAN1_Init() {
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	// PA11 CANRX
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// PA12 CANTX
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	CAN_InitTypeDef  CAN_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	
	CAN_StructInit(&CAN_InitStructure);
	CAN_InitStructure.CAN_TTCM      = DISABLE;
	CAN_InitStructure.CAN_ABOM      = DISABLE;
	CAN_InitStructure.CAN_AWUM      = DISABLE;
	CAN_InitStructure.CAN_NART      = DISABLE;
	CAN_InitStructure.CAN_RFLM      = DISABLE;
	CAN_InitStructure.CAN_TXFP      = DISABLE;
	CAN_InitStructure.CAN_Mode      = CAN_Mode_Normal;
	CAN_InitStructure.CAN_SJW       = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1       = CAN_BS1_3tq;
	CAN_InitStructure.CAN_BS2       = CAN_BS2_2tq;
	CAN_InitStructure.CAN_Prescaler = 12; // 500K
	//CAN_InitStructure.CAN_Prescaler = 24; // 250K
	//CAN_InitStructure.CAN_Prescaler = 48; // 125K

	CAN_Init(CAN1, &CAN_InitStructure);
}

void CAN1_FilterInit_11bit() {
	u32	id   = 0x7E0;
	u32	mask = 0x7FF;

	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	NVIC_InitTypeDef      NVIC_InitStructure;
	
	CAN_FilterInitStructure.CAN_FilterNumber         = 0;
	CAN_FilterInitStructure.CAN_FilterMode           = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale          = CAN_FilterScale_16bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh         = id<<5;
	CAN_FilterInitStructure.CAN_FilterIdLow          = id<<5;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh     = mask<<5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow      = mask<<5;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation     = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);  
}

void CAN1_FilterInit_29bit() {
	u32	id   = 0x18DAF110;
	u32	mask = 0x1FFFFFFF;

	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	NVIC_InitTypeDef      NVIC_InitStructure;
	
	CAN_FilterInitStructure.CAN_FilterNumber         = 0;
	CAN_FilterInitStructure.CAN_FilterMode           = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale          = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh         = (id>>13) & 0xFFFF;
	CAN_FilterInitStructure.CAN_FilterIdLow          = ((id<<3) & 0xFFFF) | CAN_Id_Extended | CAN_RTR_Data;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh     = (mask>>13) & 0xFFFF;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow      = (mask<<3) & 0xFFFF;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation     = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);  
}

int main() {
	printf("start...\n");
	CAN1_Init();
	CAN1_FilterInit_11bit();
	while(1) {
		txMsg.IDE = CAN_Id_Standard;
		txMsg.RTR = CAN_RTR_Data;
		txMsg.StdId = 0x7E0;
		txMsg.ExtId = 0x0;
		//txMsg.IDE = CAN_ID_EXT;
		//txMsg.RTR = CAN_RTR_Data;
		//txMsg.StdId = 0x0;
		//txMsg.ExtId = 0x18DB33F1;
		txMsg.DLC = 8;
		txMsg.Data[0] = 0x02;
		txMsg.Data[1] = 0x01;
		txMsg.Data[2] = 0x00;		
		printf("StdId: %08x, ExtId: %x, IDE: %d, RTR: %d, DLC: %d, ", txMsg.StdId, txMsg.ExtId, txMsg.IDE, txMsg.RTR, txMsg.DLC);
		printf("Data: %02x %02x %02x %02x %02x %02x %02x %02x\n", 
			txMsg.Data[0], 
			txMsg.Data[1], 
			txMsg.Data[2], 
			txMsg.Data[3], 
			txMsg.Data[4], 
			txMsg.Data[5], 
			txMsg.Data[6], 
			txMsg.Data[7]
		);
		//printf("transmit...\n");
		txmb = CAN_Transmit(CAN1, &txMsg);
		while (CAN_TransmitStatus(CAN1, txmb)!=CAN_TxStatus_Ok) {
			err = CAN_GetLastErrorCode(CAN1);
			if (err != 0) {
				printf("Error Code: %d\n", err);
				delay(10000000);
			}
		}

		delay(10000000);  
  }
}
