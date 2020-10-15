gpio口测试

1、测试程序使用方法
	gpio_in num1~num2 value
	将gpio配置成输入模式，gpio口从num1到num2，将读取到的值打印出来，并与给定的value比较，不一致则会在屏幕打印ERROR
  		如 gpio_in 7~14 1
    	将gpio7至gpio14配置成输入模式，将读取到的value打印出来，并与1（高电平）比较，若读取的值与指定的不一致则会打印ERROR

	gpio_out pin~pin value
	将gpio配置成输出模式，gpio口从num1到num2，将value值输出到io口
    	如 gpio_out 7~14 1
    	将gpio7至gpio14配置成输出模式，输出高电平；

2、pin值计算
    gpioX_Y ->gpio(32*(X-1)+Y)
    如：gpio2_23 -> gpio(1*32+23)->gpio55
	该计算方法适用于cpu：imx6

3、测试步骤
	1）测试gpio输入是否正常
		将要测试的gpio口全部接到3.3v上，开发板执行：gpio_in num1~num2 1
			观察屏幕打印信息判断是否有错误
		将要测试的gpio口全部接地，开发板执行：gpio_in num1~num2 0
			观察屏幕打印信息判断是否有错误
		后续可以用事件代替打印错误
	2）测试gpio输出是否正常
		两组gpio口相连，如板A的gpio7～gpio14与板B的gpio7~14（gpio编号不必保持一致，引脚数量一致就可以了）
		设B的输入正常，测试A的输出
			A板执行：gpio_out 7~14 1		//输出高电平
			B板执行：gpio_in 7~14 1		//测试A输入的是否是高电平，如果不是则打印错误（可以用事件代替打印）
			A板执行：gpio_out 7~14 0		//输出低电平
			B板执行：gpio_in 7~14 0		//测试A输入的是否是低电平，如果不是则打印错误（可以用事件代替打印）
