# Motivation 
  We are trying to use mbed system to utilized the usage of space.  
  And make the get/store thing more convient, even can do that through cellphone app and student Id Card.  
# Work Flow Chart  
![Image of workflow1](https://github.com/NTUEE-ESLab/2019-SmartCabinet/blob/master/workflow1.png)
![Image of workflow2](https://github.com/NTUEE-ESLab/2019-SmartCabinet/blob/master/workflow2.png)
  
We use STM32 L475 Discovery board as main character, which response to getting signals from BLE/RFID and transmitting data to web server to kown whether open the cabinet or not. Then Rpi would take a picture of the item, regonize it with AWS API, then transmit what the item is back to STM32 board through I2C tech. Finally, STM32 board would tell server what is inside the certain box and update the data. So, user can reach data by website to see what they have placed in the cabinet.


# How to Practice and Difficulty
* web and server :  
  Webserver using react frame as front end, and nodejs+express as back end. with restful api communicating with socket. During the restful getting request, server has to emit events to all client, which take us a little time. Under this architecture, no matter from our cabinet or web, as long as recieving  socket emit it can update data.  
  
*  develope board and IoT transmission :   
  Using thread making STM32 could handle BLE/RFID acessing at the same time. And use WiFi module onthe board to transs mit web's data.  
  While using I2C comunicatting with Rpi, we encounter the problems that it work not stably.Meybe the pull up resister is not using the same one or something else.Furthermore, in the begin, we want to use Ardiu CAM OV7670 as the camera to get the picture of the item placed, but with STM32 board, it seems the memory is too small to handle the module.So we using Rpi to implement the function.  
  While using motor, it weems there aretoo many pin on the board or something wrong like that. Motors can not move as expected.
  
* module implement and structure design :  
Fro the structure design, we using the architecture of 3D printer. But during the process, we encounter several problems. Such as the fraction froce with the transportting. And we change our material to get thourgh it. And the problem that the height between lift platfrom and normal one, which makes boxes transport not so smoothly. At last, we figure out to design the corner to round to get through. Last we using 3D printing to make our components, there are some size issue, because we didn't consider the materials thickness. After few times of attemping, we at last overcome it.  
  
# Result  
  As former mentioned, in the last, our function about IoT can work functionally. However, the part of controlling stepper motor is quite wierd. Somtimes it can work, while others can not.We think that maybe we can change the line from Dupont line to some more reliable ones.  
# Reference and Data  
* [link to web server]( http://140.112.18.177:7122 )
* Several Mbed OS example code Data to implement 3D printer
