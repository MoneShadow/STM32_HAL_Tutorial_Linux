官方的HAL对于各个外设DMA调用都很喜欢跳来跳去
我这里大概梳理一下一次收发流程

串口助手发送数据
UART中的数据寄存器(DR)收到数据 触发接收寄存器非空(RXNE) DMA接收到RXNE开始搬运数据到rxbuffer(接收缓冲区)
DR空闲 触发总线空闲(IDLE) 跳转到中断向量表 进入中断处理函数(该HAL版本官方没有对空闲中断进行封装，这里手动封装) 该处理函数调用中止串口接收函数(IT模式)
中断接收函数内部会调用一系列内部函数(此处跳转及其套娃故不再展出) 最后会调用中断接收完成回调函数 (调用该函数的是内部函数static void UART_DMATxAbortCallback(DMA_HandleTypeDef *hdma)并不是中止串口接收函数调用的中断接收完成回调函数)
中断接收完成回调函数会处理rxbuffer -> ringbuffer 然后置环形缓冲区有数据标志 之后再次启动DMA串口接收函数 直到此刻整个中断完成
回到main中的while循环 检查到数据位置1且HAL_UART_STATE_READY 处理 数据位置0 ringbuffer -> txbuffer 开启DMA串口发送 发送完成 调用发送完成回调函数 判断ringbuffer中是否还有数据 有数据位置1 没有就不管
等待下一次空闲中断产生