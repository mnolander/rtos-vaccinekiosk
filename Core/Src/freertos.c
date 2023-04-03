/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId injectTaskHandle;
osThreadId selectTaskHandle;
osThreadId paymentTaskHandle;
osThreadId armTaskHandle;
osThreadId failsafeTaskHandle;
osMessageQId vaccineQueueHandle;
osMessageQId taskQueueHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartInjectTask(void const * argument);
void StartSelectTask(void const * argument);
void StartPaymentTask(void const * argument);
void StartArmTask(void const * argument);
void StartFailSafe(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of vaccineQueue */
  osMessageQDef(vaccineQueue, 1, uint16_t);
  vaccineQueueHandle = osMessageCreate(osMessageQ(vaccineQueue), NULL);

  /* definition and creation of taskQueue */
  osMessageQDef(taskQueue, 1, uint16_t);
  taskQueueHandle = osMessageCreate(osMessageQ(taskQueue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityHigh, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of injectTask */
  osThreadDef(injectTask, StartInjectTask, osPriorityBelowNormal, 0, 128);
  injectTaskHandle = osThreadCreate(osThread(injectTask), NULL);

  /* definition and creation of selectTask */
  osThreadDef(selectTask, StartSelectTask, osPriorityHigh, 0, 128);
  selectTaskHandle = osThreadCreate(osThread(selectTask), NULL);

  /* definition and creation of paymentTask */
  osThreadDef(paymentTask, StartPaymentTask, osPriorityAboveNormal, 0, 128);
  paymentTaskHandle = osThreadCreate(osThread(paymentTask), NULL);

  /* definition and creation of armTask */
  osThreadDef(armTask, StartArmTask, osPriorityNormal, 0, 128);
  armTaskHandle = osThreadCreate(osThread(armTask), NULL);

  /* definition and creation of failsafeTask */
  osThreadDef(failsafeTask, StartFailSafe, osPriorityHigh, 0, 128);
  failsafeTaskHandle = osThreadCreate(osThread(failsafeTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
  	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
  	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);

    xTaskNotify(selectTaskHandle, 0, eNoAction);
    vTaskResume(selectTaskHandle);
    vTaskSuspend(NULL);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartInjectTask */
/**
* @brief Function implementing the injectTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartInjectTask */
void StartInjectTask(void const * argument)
{
  /* USER CODE BEGIN StartInjectTask */
  /* Infinite loop */
  uint16_t recValue;
  int injectConfirm = 1;
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
  for(;;)
  {
    xQueueReceive(taskQueueHandle, &recValue, pdMS_TO_TICKS(10000));

    while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == GPIO_PIN_RESET){
      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_SET);
      HAL_Delay(100);
      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_RESET);
    }

    if(injectConfirm == 1){
      //Hooray you got your vaccine without dying!
      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
      xTaskNotify(defaultTaskHandle, 0, eNoAction);
      vTaskSuspend(NULL);
      vTaskResume(defaultTaskHandle);
    }
    else{
      xTaskNotify(failsafeTaskHandle, 0, eNoAction);
      vTaskSuspend(NULL);
      vTaskResume(failsafeTaskHandle);
    }
  }
  /* USER CODE END StartInjectTask */
}

/* USER CODE BEGIN Header_StartSelectTask */
/**
* @brief Function implementing the selectTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartSelectTask */
void StartSelectTask(void const * argument)
{
  /* USER CODE BEGIN StartSelectTask */
  /* Infinite loop */
	uint16_t selectedVaccine = 0;
  for(;;)
  {
	ulTaskNotifyTake(pdTRUE, 0);
    GPIO_PinState SelectPushButton = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2); // Read user input (select button)
    GPIO_PinState ChoosePushButton = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4); // Read user input (choose button)

    if(SelectPushButton == GPIO_PIN_RESET){ // If button is reversed then update GPIO_InitStruct.Pull = GPIO_PULLUP; in gpio.c
    	if(selectedVaccine == 0){ //If blue vaccine was last activated, switch to green
    		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
    		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
    		selectedVaccine = 1;
    	}
    	else if(selectedVaccine == 1){ //If green vaccine was last activated, switch to orange
    		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
    		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
    		selectedVaccine = 2;
    	}
    	else if(selectedVaccine == 2){ //If orange vaccine was last activated, switch to blue
    		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
    		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
    		selectedVaccine = 0;
    	}
    	vTaskDelay(250); //Delay so options don't switch too fast
    }

    if(ChoosePushButton == GPIO_PIN_RESET){
    	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
    	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
    	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
    	xQueueSendToFront(vaccineQueueHandle, &selectedVaccine, 10000); //Put selected vaccine into queue
    	xTaskNotify(paymentTaskHandle, 0, eNoAction);
    	vTaskSuspend(NULL);
    	vTaskResume(paymentTaskHandle);
    }
  }
  /* USER CODE END StartSelectTask */
}

/* USER CODE BEGIN Header_StartPaymentTask */
/**
* @brief Function implementing the paymentTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartPaymentTask */
void StartPaymentTask(void const * argument)
{
  uint16_t selectedVaccine;
  for(;;)
  {
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	xQueueReceive(vaccineQueueHandle, &selectedVaccine, pdMS_TO_TICKS(10000));
	while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET){
		// Flash green LED
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
		vTaskDelay(500);
	}
    // Check payment button
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_RESET){
    	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
        // Put selected vaccine into queue
        xQueueSendToFront(vaccineQueueHandle, &selectedVaccine, 0);
        xTaskNotify(armTaskHandle, 0, eNoAction);
        vTaskSuspend(NULL);
        vTaskResume(armTaskHandle);
    }
  }
}

/* USER CODE BEGIN Header_StartArmTask */
/**
* @brief Function implementing the armTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartArmTask */
void StartArmTask(void const * argument)
{
  /* USER CODE BEGIN StartArmTask */
  /* Infinite loop */
	uint16_t selectedVaccine;
	TickType_t buttonPressTime = 0;
	TickType_t buttonHoldTime = pdMS_TO_TICKS(5000);
	int armSuccess, armAttempt = 0;
  for(;;)
  {
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    xQueueReceive(vaccineQueueHandle, &selectedVaccine, pdMS_TO_TICKS(10000));
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);

	while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == GPIO_PIN_SET){ //Flash red LED while waiting for arm button
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
		vTaskDelay(500);
	}

	while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == GPIO_PIN_RESET){
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
		  armAttempt = 1;
	      if (buttonPressTime == 0)
	      {
	        buttonPressTime = xTaskGetTickCount(); //Record time user put arm down
	      }

	      if ((xTaskGetTickCount() - buttonPressTime) >= buttonHoldTime) //Check if user has held button for 5 seconds
	      {
	        //Success!
	    	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
	        armSuccess = 1;
	        break;
	      }

	      if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == GPIO_PIN_SET) //Check if user released button
	      {
	        buttonPressTime = 0; //Reset time
	      }
	    }

	if(armAttempt == 1){
		if (armSuccess == 1)
		{
			buttonPressTime = 0;
	        xQueueSendToFront(taskQueueHandle, &armSuccess, 0);
	        xTaskNotify(injectTaskHandle, 0, eNoAction);
	        vTaskSuspend(NULL);
	        vTaskResume(injectTaskHandle);
		}
		else if (armSuccess == 0)
		{
	        xTaskNotify(failsafeTaskHandle, 0, eNoAction);
	        vTaskSuspend(NULL);
	        vTaskResume(failsafeTaskHandle);
		}
	}
  }
  /* USER CODE END StartArmTask */
}

/* USER CODE BEGIN Header_StartFailSafe */
/**
* @brief Function implementing the failsafeTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartFailSafe */
void StartFailSafe(void const * argument)
{
  /* USER CODE BEGIN StartFailSafe */
  /* Infinite loop */
	TickType_t alertTime = pdMS_TO_TICKS(30000);
  for(;;)
  {
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);

	while(1){
    if (xTaskGetTickCount() >= alertTime)
    {
        xTaskNotify(selectTaskHandle, 0, eNoAction);
        vTaskSuspend(NULL);
        vTaskResume(selectTaskHandle);
    }
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_2);
	HAL_Delay(700);
	}
  }
  /* USER CODE END StartFailSafe */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
