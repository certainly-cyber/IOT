import sensor, image, time, math,pyb,time,lcd

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA) # we run out of memory if the resolution is much bigger...
sensor.skip_frames(30)
sensor.set_auto_gain(False)  # must turn this off to prevent image washout...
sensor.set_auto_whitebal(False)  # must turn this off to prevent image washout...
clock = time.clock()
lcd.init()
# 注意！与find_qrcodes不同，find_apriltags 不需要软件矫正畸变就可以工作。

# 注意，输出的姿态的单位是弧度，可以转换成角度，但是位置的单位是和你的大小有关，需要等比例换算

# f_x 是x的像素为单位的焦距。对于标准的OpenMV，应该等于2.8/3.984*656，这个值是用毫米为单位的焦距除以x方向的感光元件的长度，乘以x方向的感光元件的像素（OV7725）
# f_y 是y的像素为单位的焦距。对于标准的OpenMV，应该等于2.8/2.952*488，这个值是用毫米为单位的焦距除以y方向的感光元件的长度，乘以y方向的感光元件的像素（OV7725）x

# c_x 是图像的x中心位置
# c_y 是图像的y中心位置

f_x = (2.8 / 3.984) * 160 # 默认值
f_y = (2.8 / 2.952) * 120 # 默认值
c_x = 160 * 0.5 # 默认值(image.w * 0.5)
c_y = 120 * 0.5 # 默认值(image.h * 0.5)

uart = pyb.UART(3, 115200)
uart.write("hello")
i=0

def lowpass(vi,vk_1,sample,cutfreq):
    rc = 1.0 / 2.0 / 3,1415926 / 5122
    cof1 = 1/(1 + 0.031830988 * 100)
    cof2 = (3.1830988) / (1+3.1830988)
    vk = vi * cof1 + vk_1 * cof2
    return vk

def degrees(radians):
    return (180 * radians) / math.pi

while(True):
    clock.tick()
    lcd.display(sensor.snapshot())
    img = sensor.snapshot()
    for tag in img.find_apriltags(fx=f_x, fy=f_y, cx=c_x, cy=c_y): # 默认为TAG36H11
        img.draw_rectangle(tag.rect(),color=(255,0,0))
        lcd.display(img)
        if i == 0:
            distance = (tag.x_translation()*15.3/200*80+80, tag.y_translation()*15.3/200*80+80, tag.z_translation()*15.3/200*80+80)
            distance_old = distance
            i = i+1
            uart.write("line %f,%f,%f,%f,RED\r\n" % (distance[0],distance[1],distance_old[0],distance_old[1]))
        else:
            distance = (tag.x_translation()*15.3/200*80+80, tag.y_translation()*15.3/200*80+80, tag.z_translation()*15.3/200*80+80)
            distancelp=(lowpass(distance[0],distance_old[0],100,5),lowpass(distance[1],distance_old[1],100,5),\
            lowpass(distance[2],distance_old[2],100,5))
            uart.write("line %d,%d,%d,%d,RED\r\n" % (distance[0],distance[1],distance_old[0],distance_old[1]))
            distance_old = distance
            if distance[0] < 80:
                uart.write("error\r\n")
    time.sleep(10)



