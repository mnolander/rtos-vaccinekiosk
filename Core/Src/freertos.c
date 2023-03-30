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
osMessageQId vaccineQueueHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartInjectTask(void const * argument);
void StartSelectTask(void const * argument);
void StartPaymentTask(void const * argument);
void StartArmTask(void const * argument);

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

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityIdle, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of injectTask */
  osThreadDef(injectTask, StartInjectTask, osPriorityHigh, 0, 128);
  injectTaskHandle = osThreadCreate(osThread(injectTask), NULL);

  /* definition and creation of selectTask */
  osThreadDef(selectTask, StartSelectTask, osPriorityBelowNormal, 0, 128);
  selectTaskHandle = osThreadCreate(osThread(selectTask), NULL);

  /* definition and creation of paymentTask */
  osThreadDef(paymentTask, StartPaymentTask, osPriorityNormal, 0, 128);
  paymentTaskHandle = osThreadCreate(osThread(paymentTask), NULL);

  /* definition and creation of armTask */
  osThreadDef(armTask, StartArmTask, osPriorityAboveNormal, 0, 128);
  armTaskHandle = osThreadCreate(osThread(armTask), NULL);

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
    osDelay(1);
    GPIO_PinState UserPushButton = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2); // Read user input
    if(UserPushButton == GPIO_PIN_SET){ // If button is reversed then update GPIO_InitStruct.Pull = GPIO_PULLUP; in gpio.c
    	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
    }
    else{
    	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
    }
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
  for(;;)
  {
    osDelay(1);
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
	int selectedVaccine = 0;
  for(;;)
  {
    osDelay(1);
    GPIO_PinState SelectPushButton = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_?); // Read user input (select button)
    GPIO_PinState ChoosePushButton = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_?); // Read user input (choose button)
    if(SelectPushButton == GPIO_PIN_SET){
    	if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_15) == GPIO_PIN_SET){ //If blue vaccine was last activated, switch to green
    		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
    		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
    		selectedVaccine = 0;
    	}
    	else if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_12) == GPIO_PIN_SET){ //If green vaccine was last activated, switch to orange
    		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
    		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
    		selectedVaccine = 1;
    	}
    	else if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_13) == GPIO_PIN_SET){ //If orange vaccine was last activated, switch to blue
    		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
    		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
    		selectedVaccine = 2;
    	}
    }
    if(ChoosePushButton == GPIO_PIN_SET){
    	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
    	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
    	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
    	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
    	xQueueSendtoFront(vaccineQueueHandle, &selectedVaccine, 0); //Put selected vaccine into queue
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
  /* USER CODE BEGIN StartPaymentTask */
  /* Infinite loop */
	uint16_t recValue;
  for(;;)
  {
    osDelay(1);

    GPIO_TogglePin(GPIOD, GPIO_PIN_12);
    vTaskDelay(500);
    GPIO_TogglePin(GPIOD, GPIO_PIN_12);
    vTaskDelay(500);

    GPIO_PinState PaymentPushButton = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_?); // Read user input (payment button)

    if(PaymentPushButton == GPIO_PIN_SET){
    	//Queue again, move to arm state
    }

    xQueueReceive(vaccineQueueHandle, &recValue, pdMS_TO_TICKS(10000))
  }
  /* USER CODE END StartPaymentTask */
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
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartArmTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
