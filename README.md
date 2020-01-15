# Motivation 
  我們的動機是在系上用系櫃的時候發現櫃子佔了很大的空間，並且需要保持著每一個櫃子都 有們的形式，所以就想找一中方式可以讓櫃子們可以不用都有門進而有其他種排列的方式來 節省使用空間。並且結合影像辨識、網頁等系統來讓收納更加便利，讓我們能夠更方便地拿 取東⻄或查看櫃子裡有什麼東⻄來改變人類的生活。
# How to Practice and Difficulty
陳昱行( response to web and server):
這次我負責的部分是web server。前端使用react框架 後段使用nodejs+express，並 且使用restful api與socket溝通。中間有遇到一點困難事在restful接到request的時 候，要由server主動emit事件給所有client，這部分花了一點時間才實作出來。 這樣不管是我們的箱子或是網頁端只要可以收到socket emit的事件就可以更新資料
黃柏崴 ( response to develope board and IoT transmission ) : 我主要是負責STM32及Rpi開發版coding的部分。 其中，我利用thread讓機構能同時接收 BLE\RFID Card 的資訊讓櫃子的開啟能夠透過 不只一種的方式，並且利用板子上原有的 WiFi module 來傳送web的資料。 其中遇到不少問題，包跨用I2C與Rpi溝通的時候常常莫名其妙的可以用或是莫名其妙 的沒辦法作用，或許是兩邊的 Pull up resister 沒有共用或是 3v3 電壓沒有連接再一 起的關係，然後我們本來嘗試使用 Ardiu CAM OV7670來達到替物品拍照的功能並傳 送給 server 端進行辨識，但是 OV7670 似乎本身沒有 FIFO 晶片，所以拍出來的結果 資料量過大，並沒有辦法直接讓 STM32 接受。並且在其中，常常有 Mbed OS 版本 不同的問題常常網路上有 OS2 版本的 code 放到 OS5 上連 set 成 active program 都 沒有辦法，還有常常 Stm 在出 fault 的時候是直接進入faulting 的迴圈，沒有辦法簡 單明瞭的知道他的錯誤訊息出在哪裡，或許是Memory leak 或許是沒有 network

interface 等等的，他沒有一個明確的 Error code 似乎要自己 implement 。最後在實 際組裝所有 component 的時候也遇到輸出 Motor 的電壓似乎並不精確，會有不小心 變 high voltage 的狀況，讓我發現這個板子似乎是比較適合作為傳遞 sensor 資料的 板子，因為他很多傳遞模組都 integrate 在其中，要使用真的非常方便。但是似乎在 WiFi連接的時候模組常常會連接太多次而無法連接，要等他休息一下。
林辰宇 ( response to module implement and structure design ) : 我這一次負責特部分是整個project的機構的設計以及機械架構模組上的應用。在機構 的設計上，我一開始的想法參考了大型倉庫的運輸以及學校綜合大樓圖書館的設計， 希望能夠設計出一個能夠在儲存空間內部自行移動的機構。其中升降的機構我研究了 一下網路上利用了牙條運輸的自制3D列印機的設計。在設計過程中所遇到的問題有很 多，例如說在設計時把許多條件想的過於理想。平面運輸時忘記考慮摩擦力的部份， 後來更換了摩擦係數較小的平面解決。箱子經過升降平面時，因為高度差而無法順利 放置於升降平面的問題，最後則是將箱子的角落修成斜面解決。其中，3D列印時因為 會有尺寸誤差(如材料厚度)，所以常常會需要更改設計並重新列印。接著是馬達的 部分，這個機構我選用了兩種馬達。其中28BYJ-48用在兩層的平面運輸上，而42步 步進馬達則是用於升降台的升降機制。馬達的模組則試過了A4988以及ULN2003。 ULN2003是專為28BYJ-48設計的模組，因為網路上resource很多，所以使用上不是 個太大的問題。選擇A4988則是因為有許多自製3D列印機使用，故一開始選用，但是 因為網路上的3D列印機大多都是用rpi或arduino來做，所以轉到stm32上花了一點時 間研究，但最後似乎並不理想。
# Result
就像前面說的，我們的馬達似乎電壓不夠或是輸出不夠明確導致馬達的轉動非常的不順或是 無法順利轉動，也可能是 stepper 的模組 A4988 的連接不夠精確，常常會接觸不良。 不過我們 web server 以及傳遞資料、BLE、WiFi、RFID的 部分都非常的完整，能夠實際的 作用。 
# Reference and Data  
* Several Mbed OS example code Data to implement 3D printer
